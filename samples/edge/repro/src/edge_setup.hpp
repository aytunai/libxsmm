#ifndef EDGE_REPRODUCER_SETUP_HPP
#define EDGE_REPRODUCER_SETUP_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <ctime>

#ifdef PP_USE_OMP
#include <omp.h>
#else
#define omp_get_max_threads() 1
#define omp_get_num_threads() 1
#define omp_get_thread_num()  0
#endif

#include "constants.hpp"
#include "edge_helper.hpp"

// matrix file name (hard-coded)
const std::string C_MAT_DIR(void) {
  const char* l_envMatsDir = std::getenv("MATS_DIR");
  if ( ( l_envMatsDir == NULL           ) ||
       ( std::strlen(l_envMatsDir) == 0 )    ) {
    std::cout << "Error: Invalid path for matrices." << std::endl;
    exit(-1);
  }
  return (std::string(l_envMatsDir)+"/");
}

const std::string C_MAT_NAME(std::string type, unsigned int idx) {
  return (C_MAT_DIR()+"tet4_"+std::to_string(ORDER-1)+"_"+type+"_"+std::to_string(idx)+"_csc.mtx");
}

const std::string C_STIFFT_NAME(unsigned int idx) { return C_MAT_NAME("stiffT", idx); }
const std::string C_STIFF_NAME (unsigned int idx) { return C_MAT_NAME("stiffV", idx); }
const std::string C_FLUXL_NAME (unsigned int idx) { return C_MAT_NAME("fluxL", idx); }
const std::string C_FLUXN_NAME (unsigned int idx) { return C_MAT_NAME("fluxN", idx); }
const std::string C_FLUXT_NAME (unsigned int idx) { return C_MAT_NAME("fluxT", idx); }
const std::string C_MSTAR_NAME (void)             { return (C_MAT_DIR()+"tet4_starMatrix_csr.mtx"); }
const std::string C_FLUXSOLV_NAME (void)          { return (C_MAT_DIR()+"tet4_fluxMatrix_csr_de.mtx"); }


namespace edge {
  namespace reproducers {
    /* reroducer mode code */
    const unsigned int C_MODE_LOCAL = 0x00000001;
    const unsigned int C_MODE_NEIGH = 0x00000010;
    const unsigned int C_MODE_FULL  = 0x00000011;

    /* setup functions
     * ANY memory associated to the structures/pointers is allocated WITHIN the setup function scope
     */
    void setupDg  ( t_dg  & io_dg );
    void cleanupDg( t_dg  & io_dg );

    void setupStarM( unsigned int  const     i_nEl,
                     t_matStar           (** io_starM)[N_DIM] );

    void cleanupStarM( t_matStar  (* io_starM)[N_DIM] );

    void setupFluxSolv( unsigned int  const     i_nEl,
                        t_fluxSolver        (** io_fluxSolvers)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] );

    void cleanupFluxSolv( t_fluxSolver  (* io_fluxSolvers)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] );

    void setupKernel( edge::data::MmXsmmFused< real_base >  & io_mm );

    void setupTensor( unsigned int  const     i_nEl,
                      real_base           (** io_dofs)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS],
                      real_base           (** io_tInt)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS] );

    void cleanupTensor( real_base  (* io_dofs)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS],
                        real_base  (* io_tInt)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS] );

    void setupScratchMem( t_scratchMem   ** io_scratchMem );
    void cleanupScratchMem( t_scratchMem  * io_scratchMem );

    void setupPseudoMesh( unsigned int const    i_reproMode,
                          unsigned int const    i_nElements,
                          t_elementChars      ** io_elChars,
                          t_faceChars         ** io_faChars,
                          unsigned int       (** io_elFa)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                          unsigned int       (** io_elFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                          unsigned short     (** io_fIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                          unsigned short     (** io_vIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] );

    void cleanupPseudoMesh( unsigned int const      i_reproMode,
                            t_elementChars      * io_elChars,
                            t_faceChars         * io_faChars,
                            unsigned int       (* io_elFa)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                            unsigned int       (* io_elFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                            unsigned short     (* io_fIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                            unsigned short     (* io_vIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] );
  }
}


/* * * * * * * * * * * * * */


void edge::reproducers::setupDg( t_dg & i_dg ) {
  /*
   * Implementation based on https://github.com/3343/edge/blob/develop/src/dg/setup_ader.inc#L55-L231
   */
  if ( C_MAT_DIR() == "" ) {
    std::cout << "Error: Invalid path for matrices." << std::endl;
    exit(-1);
  } else {
    std::cout << "Read matrices from " << C_MAT_DIR() << " for DG matrices and element private matrices." << std::endl;
  }

#if defined PP_T_KERNELS_XSMM
  std::vector< real_base >    l_matVal;
  std::vector< unsigned int > l_matColPtr;
  std::vector< unsigned int > l_matRowIdx;

  /*
   * 1. stiffT : transposed stiffness matrices - TimePred
   */
  std::vector< real_base >    l_stiffTVal[N_DIM];
  std::vector< unsigned int > l_stiffTColPtr[N_DIM];
  std::vector< unsigned int > l_stiffTRowIdx[N_DIM];
  for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
    readSparseMatrixCsc(C_STIFFT_NAME(l_di), l_stiffTVal[l_di], l_stiffTColPtr[l_di], l_stiffTRowIdx[l_di]);
  }

  // hierarchical setup
  unsigned int l_nzCols = N_ELEMENT_MODES;
  unsigned int l_nzRows = N_ELEMENT_MODES;
  for ( unsigned int l_de = 1; l_de < ORDER; l_de++ ) {
    // determine non-zero block for the next iteration
    l_nzCols = CE_N_ELEMENT_MODES( T_SDISC.ELEMENT, ORDER-l_de );

    // add data for the first CK-stiff matrix or shrinking stiff matrices
    if ( l_de == 1 || true ) {
      for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
        selectSubSparseMatrixCsc( l_stiffTVal[l_di], l_stiffTColPtr[l_di], l_stiffTRowIdx[l_di],
                                  l_nzRows, l_nzCols,
                                  l_matVal, l_matColPtr, l_matRowIdx );
        i_dg.mat.stiffT[(l_de-1)*N_DIM+l_di] = new real_base[l_matVal.size()];
        for ( unsigned int l_nz = 0; l_nz < l_matVal.size(); l_nz++ )
          i_dg.mat.stiffT[(l_de-1)*N_DIM+l_di][l_nz] = l_matVal[l_nz];
      }
    }
    else {
      assert(false);
      for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
        i_dg.mat.stiffT[(l_de-1)*N_DIM+l_di] = i_dg.mat.stiffT[(l_de-2)*N_DIM+l_di];
        /* suggested modification to : https://github.com/3343/edge/blob/develop/src/dg/setup_ader.inc#L126 */
      }
    }

    // reduce relevant rows due to generated zero block
    l_nzRows = l_nzCols;
  }

  /*
   * 2. stiff  : stiffness matrices - VolInt
   */
  for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
    readSparseMatrixCsc(C_STIFF_NAME(l_di), l_matVal, l_matColPtr, l_matRowIdx);
    i_dg.mat.stiff[l_di] = new real_base[l_matVal.size()];
    for ( unsigned int l_nz = 0; l_nz < l_matVal.size(); l_nz++ )
      i_dg.mat.stiff[l_di][l_nz] = l_matVal[l_nz];
  }

  /*
   * 3. fluxL  : local contribution flux matrices - SurfInt
   */
  for ( unsigned short l_fl = 0; l_fl < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fl++ ) {
    readSparseMatrixCsc(C_FLUXL_NAME(l_fl), l_matVal, l_matColPtr, l_matRowIdx);
    i_dg.mat.fluxL[l_fl] = new real_base[l_matVal.size()];
    for ( unsigned int l_nz = 0; l_nz < l_matVal.size(); l_nz++ )
      i_dg.mat.fluxL[l_fl][l_nz] = l_matVal[l_nz];
  }

  /*
   * 4. fluxN  : neighboring contribution flux matrix - SurfInt
   */
  for ( unsigned short l_fn = 0; l_fn < N_FLUXN_MATRICES; l_fn++ ) {
    readSparseMatrixCsc(C_FLUXN_NAME(l_fn), l_matVal, l_matColPtr, l_matRowIdx);
    i_dg.mat.fluxN[l_fn] = new real_base[l_matVal.size()];
    for ( unsigned int l_nz = 0; l_nz < l_matVal.size(); l_nz++ )
      i_dg.mat.fluxN[l_fn][l_nz] = l_matVal[l_nz];
  }

  /*
   * 5. fluxT  : "transposed" flux matrices - SurfInt
   */
  for ( unsigned short l_ft = 0; l_ft < C_ENT[T_SDISC.ELEMENT].N_FACES; l_ft++ ) {
    readSparseMatrixCsc(C_FLUXT_NAME(l_ft), l_matVal, l_matColPtr, l_matRowIdx);
    i_dg.mat.fluxT[l_ft] = new real_base[l_matVal.size()];
    for ( unsigned int l_nz = 0; l_nz < l_matVal.size(); l_nz++ )
      i_dg.mat.fluxT[l_ft][l_nz] = l_matVal[l_nz];
  }

#else
#error
#endif

  return;
}

void edge::reproducers::cleanupDg( t_dg & io_dg ) {
  /* 1. stiffT */
  for ( unsigned int l_de = 1; l_de < ORDER; l_de++ ) {
    for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
      if ( io_dg.mat.stiffT[(l_de-1)*N_DIM+l_di] != NULL )
        delete[] io_dg.mat.stiffT[(l_de-1)*N_DIM+l_di];
    }
  }
  /* 2. stiff */
  for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ )
    if ( io_dg.mat.stiff[l_di] != NULL )
      delete[] io_dg.mat.stiff[l_di];
  /* 3. fluxL */
  for ( unsigned short l_fl = 0; l_fl < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fl++ )
    if ( io_dg.mat.fluxL[l_fl] != NULL )
      delete[] io_dg.mat.fluxL[l_fl];
  /* 4. fluxN */
  for ( unsigned short l_fn = 0; l_fn < N_FLUXN_MATRICES; l_fn++ )
    if ( io_dg.mat.fluxN[l_fn] != NULL )
      delete[] io_dg.mat.fluxN[l_fn];
  /* 5. fluxT */
  for ( unsigned short l_ft = 0; l_ft < C_ENT[T_SDISC.ELEMENT].N_FACES; l_ft++ )
    if ( io_dg.mat.fluxT[l_ft] != NULL )
      delete[] io_dg.mat.fluxT[l_ft];

  return;
}

void edge::reproducers::setupStarM( unsigned int  const     i_nEl,
                                    t_matStar           (** io_starM)[N_DIM] ) {
  std::vector< real_base >    l_mStarVal;
  std::vector< unsigned int > l_mStarRowPtr;
  std::vector< unsigned int > l_mStarColIdx;
  readSparseMatrixCsr(C_MSTAR_NAME(), l_mStarVal, l_mStarRowPtr, l_mStarColIdx);

  *io_starM = (t_matStar (*)[N_DIM]) new t_matStar[i_nEl*N_DIM];

#ifdef PP_USE_OMP
  #pragma omp parallel for firstprivate( i_nEl, io_starM, l_mStarVal )
#endif
  for ( unsigned int l_el = 0; l_el < i_nEl; l_el++ ) {
    for ( unsigned int l_di = 0; l_di < N_DIM; l_di++ ) {
      for ( unsigned int l_nz = 0; l_nz < N_MAT_STAR; l_nz++ ) {
        (*io_starM)[l_el][l_di].mat[l_nz] = l_mStarVal[l_nz];
      }
    }
  }

  return;
}

void edge::reproducers::cleanupStarM( t_matStar (* io_starM)[N_DIM] ) {
  t_matStar *l_starM = (t_matStar *)io_starM;
  if ( l_starM != NULL ) delete[] l_starM;

  return;
}

void edge::reproducers::setupFluxSolv( unsigned int  const     i_nEl,
                                       t_fluxSolver        (** io_fluxSolvers)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] ) {
  std::vector< real_base >    l_fSolvVal;
  std::vector< unsigned int > l_fSolvRowPtr;
  std::vector< unsigned int > l_fSolvColIdx;
  readSparseMatrixCsr(C_FLUXSOLV_NAME(), l_fSolvVal, l_fSolvRowPtr, l_fSolvColIdx);

  *io_fluxSolvers = (t_fluxSolver (*)[C_ENT[T_SDISC.ELEMENT].N_FACES]) new t_fluxSolver[i_nEl*C_ENT[T_SDISC.ELEMENT].N_FACES];

#ifdef PP_USE_OMP
  #pragma omp parallel for firstprivate( i_nEl, io_fluxSolvers, l_fSolvVal )
#endif
  for ( unsigned int l_el = 0; l_el < i_nEl; l_el++ ) {
    for ( unsigned int l_fa = 0; l_fa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fa++ ) {
      for ( unsigned int l_i = 0; l_i < N_QUANTITIES; l_i++ ) {
        for ( unsigned int l_j = 0; l_j < N_QUANTITIES; l_j++ ) {
          (*io_fluxSolvers)[l_el][l_fa].solver[l_i][l_j] = l_fSolvVal[l_i*N_QUANTITIES+l_j];
        }
      }
    }
  }

  return;
}

void edge::reproducers::cleanupFluxSolv( t_fluxSolver (* io_fluxSolvers)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] ) {
  t_fluxSolver *l_fluxSolvers = (t_fluxSolver *)io_fluxSolvers;
  if ( l_fluxSolvers != NULL ) delete[] l_fluxSolvers;

  return;
}

void edge::reproducers::setupTensor( unsigned int  const     i_nEl,
                                     real_base           (** io_dofs)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS],
                                     real_base           (** io_tInt)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS] ) {

  posix_memalign( (void **)io_dofs,
                  ALIGNMENT.ELEMENT_MODES.PRIVATE,
                  (size_t)(i_nEl*N_QUANTITIES*N_ELEMENT_MODES*N_CRUNS*sizeof(real_base)) );
  posix_memalign( (void **)io_tInt,
                  ALIGNMENT.ELEMENT_MODES.PRIVATE,
                  (size_t)(i_nEl*N_QUANTITIES*N_ELEMENT_MODES*N_CRUNS*sizeof(real_base)) );

  const real_base l_scale = 1.0 / RAND_MAX / 1000000.0;

#ifdef PP_REPRODUCER_VALIDATE // generate identical input for valication
  srand(0);
#else
  srand(time(0));
#endif
  for ( unsigned int l_el = 0; l_el < std::min((unsigned int)10, i_nEl); l_el++ ) {
    for ( unsigned int l_qt = 0; l_qt < N_QUANTITIES; l_qt++ ) {
      for ( unsigned int l_md = 0; l_md < N_ELEMENT_MODES; l_md++ ) {
        for ( unsigned int l_cfr = 0; l_cfr < N_CRUNS; l_cfr++ ) {
          (*io_dofs)[l_el][l_qt][l_md][l_cfr] = ((real_base)rand() / RAND_MAX / 1000000.0);
          (*io_tInt)[l_el][l_qt][l_md][l_cfr] = ((real_base)rand() / RAND_MAX / 1000000.0);
        }
      }
    }
  }

#ifdef PP_USE_OMP
  #pragma omp parallel for firstprivate( i_nEl, io_dofs, io_tInt )
#endif
  for ( unsigned int l_el = std::min((unsigned int)10, i_nEl); l_el < i_nEl; l_el++ ) {
    for ( unsigned int l_qt = 0; l_qt < N_QUANTITIES; l_qt++ ) {
      for ( unsigned int l_md = 0; l_md < N_ELEMENT_MODES; l_md++ ) {
        for ( unsigned int l_cfr = 0; l_cfr < N_CRUNS; l_cfr++ ) {
          (*io_dofs)[l_el][l_qt][l_md][l_cfr] = (*io_dofs)[l_el%10][l_qt][l_md][l_cfr];
          (*io_tInt)[l_el][l_qt][l_md][l_cfr] = (*io_tInt)[l_el%10][l_qt][l_md][l_cfr];
        }
      }
    }
  }

  return ;
}

void edge::reproducers::cleanupTensor ( real_base  (* io_dofs)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS],
                                        real_base  (* io_tInt)[N_QUANTITIES][N_ELEMENT_MODES][N_CRUNS] ) {
  free( (void *)io_dofs );
  free( (void *)io_tInt );

  return ;
}

void edge::reproducers::setupScratchMem ( t_scratchMem  ** io_scratchMem ) {

  posix_memalign( (void **)io_scratchMem,
                  ALIGNMENT.BASE.HEAP,
                  (size_t)sizeof( t_scratchMem ) );

  return ;
}

void edge::reproducers::cleanupScratchMem ( t_scratchMem  * io_scratchMem ) {
  free( (void *)io_scratchMem );

  return ;
}


void edge::reproducers::setupKernel ( edge::data::MmXsmmFused< real_base >  & io_mm ) {
  /*
   * Implementation based on https://github.com/3343/edge/blob/develop/src/impl/elastic/setup.inc#L242-L463
   */
  if ( C_MAT_DIR() == "" ) {
    std::cout << "Error: Invalid path for matrices." << std::endl;
    exit(-1);
  } else {
    std::cout << "Read matrices from " << C_MAT_DIR() << " to set up kernels." << std::endl;
  }

  /*
   * Derive sparse AoSoA-LIBXSMM kernels.
   *
   * 1) Cauchy Kovalewski
   */
  // get sparse, transposed stiffness matrices
  std::vector< real_base >    l_stiffTVal[N_DIM];
  std::vector< unsigned int > l_stiffTColPtr[N_DIM];
  std::vector< unsigned int > l_stiffTRowIdx[N_DIM];
  for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
    readSparseMatrixCsc(C_STIFFT_NAME(l_di), l_stiffTVal[l_di], l_stiffTColPtr[l_di], l_stiffTRowIdx[l_di]);
  }

  // get csr star matrix
  std::vector< real_base >    l_mStarVal;
  std::vector< unsigned int > l_mStarRowPtr;
  std::vector< unsigned int > l_mStarColIdx;
  readSparseMatrixCsr(C_MSTAR_NAME(), l_mStarVal, l_mStarRowPtr, l_mStarColIdx);
  assert( l_mStarVal.size() == N_MAT_STAR );

  // exploit potential zero-block generation in recursive CK

  // nz-blocks
  unsigned int l_nzCols = N_ELEMENT_MODES;
  unsigned int l_nzRows = N_ELEMENT_MODES;

  // iterate over derivatives (recursive calls)
  for ( unsigned short l_de = 1; l_de < ORDER; l_de++ ) {
    // determine non-zero block in the next iteration
    l_nzCols = CE_N_ELEMENT_MODES( T_SDISC.ELEMENT, ORDER-l_de );

    // generate libxsmm kernel for transposed stiffness matrices
    for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
      std::vector< real_base >    l_matVal;
      std::vector< unsigned int > l_matColPtr;
      std::vector< unsigned int > l_matRowIdx;
      selectSubSparseMatrixCsc( l_stiffTVal[l_di], l_stiffTColPtr[l_di], l_stiffTRowIdx[l_di],
                                l_nzRows, l_nzCols,
                                l_matVal, l_matColPtr, l_matRowIdx );

      io_mm.add(  false,
                 &l_matColPtr[0],  &l_matRowIdx[0], &l_matVal[0],
                  N_QUANTITIES, l_nzCols, l_nzRows,
                  N_ELEMENT_MODES, 0, l_nzCols,
                  real_base(1.0), real_base(0.0),
                  LIBXSMM_PREFETCH_NONE );
    }
    // generate libxsmm kernel for star matrix
    io_mm.add(  true,
               &l_mStarRowPtr[0],  &l_mStarColIdx[0], &l_mStarVal[0],
                N_QUANTITIES, l_nzCols, N_QUANTITIES,
                0, l_nzCols, N_ELEMENT_MODES,
                real_base(1.0), real_base(1.0),
                LIBXSMM_PREFETCH_NONE );

    // reduce relevant rows due to generated zero block
    l_nzRows = l_nzCols;
  }

  /*
   * 2) add volume kernels
   */
  // check that size is one "order" less
  unsigned int l_nzBl = CE_N_ELEMENT_MODES( T_SDISC.ELEMENT, ORDER-1 );

  // sparse stiffness matrices
  for ( unsigned short l_di = 0; l_di < N_DIM; l_di++ ) {
    std::vector< real_base >    l_stiffVal;
    std::vector< unsigned int > l_stiffColPtr;
    std::vector< unsigned int > l_stiffRowIdx;
    readSparseMatrixCsc(C_STIFF_NAME(l_di), l_stiffVal, l_stiffColPtr, l_stiffRowIdx);

    io_mm.add(  false,
               &l_stiffColPtr[0], &l_stiffRowIdx[0], &l_stiffVal[0],
                N_QUANTITIES, N_ELEMENT_MODES, l_nzBl,
                l_nzBl, 0, N_ELEMENT_MODES,
                real_base(1.0), real_base(1.0),
                LIBXSMM_PREFETCH_NONE ); // Remark: Star matrix is multiplied first
  }

  // star matrix
  io_mm.add(  true,
             &l_mStarRowPtr[0],  &l_mStarColIdx[0], &l_mStarVal[0],
              N_QUANTITIES, l_nzBl, N_QUANTITIES,
              0, N_ELEMENT_MODES, l_nzBl,
              real_base(1.0), real_base(0.0),
              LIBXSMM_PREFETCH_NONE );

  /*
   * 3) surface kernels
   */
  // local contribution flux matrices
  for ( unsigned short l_fl = 0; l_fl < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fl++ ) {
    std::vector< real_base >    l_fluxVal;
    std::vector< unsigned int > l_fluxColPtr;
    std::vector< unsigned int > l_fluxRowIdx;
    readSparseMatrixCsc( C_FLUXL_NAME(l_fl), l_fluxVal, l_fluxColPtr, l_fluxRowIdx );

    io_mm.add(  false,
               &l_fluxColPtr[0],  &l_fluxRowIdx[0], &l_fluxVal[0],
                N_QUANTITIES, N_FACE_MODES, N_ELEMENT_MODES,
                N_ELEMENT_MODES, 0, N_FACE_MODES,
                real_base(1.0), real_base(0.0),
                LIBXSMM_PREFETCH_NONE );
  }

  // neighboring contribution flux matrices
  for( unsigned short l_fn = 0; l_fn < N_FLUXN_MATRICES; l_fn++ ) {
    std::vector< real_base >    l_fluxVal;
    std::vector< unsigned int > l_fluxColPtr;
    std::vector< unsigned int > l_fluxRowIdx;
    readSparseMatrixCsc( C_FLUXN_NAME(l_fn), l_fluxVal, l_fluxColPtr, l_fluxRowIdx );

    io_mm.add(  false,
               &l_fluxColPtr[0],  &l_fluxRowIdx[0], &l_fluxVal[0],
                N_QUANTITIES, N_FACE_MODES, N_ELEMENT_MODES,
                N_ELEMENT_MODES, 0, N_FACE_MODES,
                real_base(1.0), real_base(0.0),
                LIBXSMM_PREFETCH_NONE );
  }

  // transposed flux matrices
  for( unsigned short l_ft = 0; l_ft < C_ENT[T_SDISC.ELEMENT].N_FACES; l_ft++ ) {
    std::vector< real_base >    l_fluxVal;
    std::vector< unsigned int > l_fluxColPtr;
    std::vector< unsigned int > l_fluxRowIdx;
    readSparseMatrixCsc( C_FLUXT_NAME(l_ft), l_fluxVal, l_fluxColPtr, l_fluxRowIdx );

    io_mm.add(  false,
               &l_fluxColPtr[0],  &l_fluxRowIdx[0], &l_fluxVal[0],
                N_QUANTITIES, N_ELEMENT_MODES, N_FACE_MODES,
                N_FACE_MODES, 0, N_ELEMENT_MODES,
                real_base(1.0), real_base(1.0),
                LIBXSMM_PREFETCH_NONE );
  }

  // flux solver
  std::vector< real_base >    l_fSolvVal;
  std::vector< unsigned int > l_fSolvRowPtr;
  std::vector< unsigned int > l_fSolvColIdx;
  readSparseMatrixCsr(C_FLUXSOLV_NAME(), l_fSolvVal, l_fSolvRowPtr, l_fSolvColIdx);
  assert( l_fSolvVal.size() == N_QUANTITIES*N_QUANTITIES );

  io_mm.add(  true,
             &l_fSolvRowPtr[0],  &l_fSolvColIdx[0], &l_fSolvVal[0],
              N_QUANTITIES, N_FACE_MODES, N_QUANTITIES,
              0, N_FACE_MODES, N_FACE_MODES,
              real_base(1.0), real_base(0.0),
              LIBXSMM_PREFETCH_BL2_VIA_C );

  return;
}


/* full version */
void edge::reproducers::setupPseudoMesh ( unsigned int const     i_reproMode,
                                          unsigned int const     i_nElements,
                                          t_elementChars      ** io_elChars,
                                          t_faceChars         ** io_faChars,
                                          unsigned int       (** io_elFa)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                          unsigned int       (** io_elFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                          unsigned short     (** io_fIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                          unsigned short     (** io_vIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] ) {
  if ( (i_reproMode & C_MODE_LOCAL) == C_MODE_LOCAL ) {
    // zero init - disable read/write recvs
    *io_elChars = (t_elementChars *) new t_elementChars[i_nElements];
    for ( unsigned int l_el = 0; l_el < i_nElements; l_el++ ) (*io_elChars)[l_el].spType = 0;
  }

  if ( (i_reproMode & C_MODE_NEIGH) == C_MODE_NEIGH ) {
    // zero init - force derive neighboring elmt from pseudo mesh
    *io_faChars = (t_faceChars *) new t_faceChars;
    (*io_faChars)->spType = 0;

    // use one pseudo face for all elements
    *io_elFa = (unsigned int (*)[ C_ENT[T_SDISC.ELEMENT].N_FACES ]) new unsigned int[ i_nElements * C_ENT[T_SDISC.ELEMENT].N_FACES ];
    for ( unsigned int l_el = 0; l_el < i_nElements; l_el++ ) {
      for ( unsigned int l_fa = 0; l_fa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fa++ ) {
        (*io_elFa)[l_el][l_fa] = 0;
      }
    }

    /* setup pseudo mesh - *io_elFaEl    : neighboring element id
     *                     *io_fIdElFaEl : neighboring face id
     *                     *io_vIdElFaEl : neighboring face orientation
     */
  #ifdef PP_REPRODUCER_VALIDATE
    srand(10);
  #else
    srand(time(0));
  #endif
    *io_elFaEl = (unsigned int (*)[ C_ENT[T_SDISC.ELEMENT].N_FACES ]) new unsigned int[ i_nElements * C_ENT[T_SDISC.ELEMENT].N_FACES ];
    *io_fIdElFaEl = (unsigned short (*)[ C_ENT[T_SDISC.ELEMENT].N_FACES ]) new unsigned short[ i_nElements * C_ENT[T_SDISC.ELEMENT].N_FACES ];
    *io_vIdElFaEl = (unsigned short (*)[ C_ENT[T_SDISC.ELEMENT].N_FACES ]) new unsigned short[ i_nElements * C_ENT[T_SDISC.ELEMENT].N_FACES ];
    for ( unsigned int l_el = 0; l_el < i_nElements; l_el++ ) {
      for ( unsigned int l_fa = 0; l_fa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fa++ ) {
        (*io_elFaEl)[l_el][l_fa] = (unsigned int)((unsigned int)rand() % i_nElements);
        (*io_fIdElFaEl)[l_el][l_fa] = (unsigned short)(unsigned short)rand() % C_ENT[T_SDISC.ELEMENT].N_FACES;
        (*io_vIdElFaEl)[l_el][l_fa] = 0;
      }
    }
  }

  return;
}

void edge::reproducers::cleanupPseudoMesh ( unsigned int const    i_reproMode,
                                            t_elementChars      * io_elChars,
                                            t_faceChars         * io_faChars,
                                            unsigned int       (* io_elFa)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                            unsigned int       (* io_elFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                            unsigned short     (* io_fIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ],
                                            unsigned short     (* io_vIdElFaEl)[ C_ENT[T_SDISC.ELEMENT].N_FACES ] ) {
  if ( (i_reproMode & C_MODE_LOCAL) == C_MODE_LOCAL ) {
    if ( io_faChars != NULL ) delete io_faChars;
  }
  if ( (i_reproMode & C_MODE_NEIGH) == C_MODE_NEIGH ) {
    if ( io_elFa != NULL ) delete[] (unsigned int *)io_elFa;
    if ( io_elFaEl != NULL ) delete[] (unsigned int *)io_elFaEl;
    if ( io_fIdElFaEl != NULL ) delete[] (unsigned short *)io_fIdElFaEl;
    if ( io_vIdElFaEl != NULL ) delete[] (unsigned short *)io_vIdElFaEl;
  }

  return;
}

#endif

