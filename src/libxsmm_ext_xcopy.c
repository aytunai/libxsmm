/******************************************************************************
* Copyright (c) Intel Corporation - All rights reserved.                      *
* This file is part of the LIBXSMM library.                                   *
*                                                                             *
* For information on the license, see the LICENSE file.                       *
* Further information: https://github.com/hfp/libxsmm/                        *
* SPDX-License-Identifier: BSD-3-Clause                                       *
******************************************************************************/
/* Hans Pabst (Intel Corp.)
******************************************************************************/
#include "libxsmm_xcopy.h"
#include "libxsmm_ext.h"

#define LIBXSMM_MCOPY_MT(MT, NT, M, N) ((MT) <= (M) && (NT) <= (N) && (64U * 64U) <= (((unsigned int)(M)) * (N)))


LIBXSMM_APIEXT void libxsmm_matcopy_omp(void* out, const void* in, unsigned int typesize,
  libxsmm_blasint m, libxsmm_blasint n, libxsmm_blasint ldi, libxsmm_blasint ldo)
{
  LIBXSMM_INIT
  if (0 < typesize && 256 > typesize && m <= ldi && m <= ldo && out != in &&
    ((NULL != out && 0 < m && 0 < n) || (0 == m && 0 == n)))
  {
    if (0 < m && 0 < n) {
#if defined(_OPENMP)
# if (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT)) && !defined(LIBXSMM_XCOPY_MELTW)
      int prefetch = 0;
# endif
      unsigned int tm, tn, ts;
      if (NULL != in) { /* mcopy */
# if (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT)) && !defined(LIBXSMM_XCOPY_MELTW)
        prefetch = libxsmm_mcopy_prefetch;
# endif
        tm = LIBXSMM_UPDIV(libxsmm_mcopy_mbytes, typesize);
        tn = (unsigned int)(libxsmm_mcopy_nscale * tm);
        ts = libxsmm_mcopy_mbytes;
      }
      else { /* mzero */
        tm = LIBXSMM_UPDIV(libxsmm_mzero_mbytes, typesize);
        tn = (unsigned int)(libxsmm_mzero_nscale * tm);
        ts = libxsmm_mzero_mbytes;
      }
      if (0 == tm) tm = m;
      if (0 == tn) tn = LIBXSMM_MIN(LIBXSMM_XCOPY_TILE_MIN, n);
      if (0 != ts && ts < (tm * tn * typesize)) {
        tm = LIBXSMM_MAX(ts / (tn * typesize), LIBXSMM_XCOPY_TILE_MIN);
      }
      if (LIBXSMM_MCOPY_MT(tm, tn, (unsigned int)m, (unsigned int)n)) { /* consider problem-size */
        libxsmm_xcopykernel kernel;
        kernel.ptr = NULL;
# if (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT))
        if (0 != (2 & libxsmm_xcopy_jit)) { /* JIT'ted matrix-copy permitted? */
#   if defined(LIBXSMM_XCOPY_MELTW)
          const libxsmm_blasint sldi = ldi * typesize, sldo = ldo * typesize;
          if (NULL != in) { /* mcopy */
            kernel.meltw_copy = libxsmm_dispatch_meltw_copy(
              (libxsmm_blasint)tm * typesize, (libxsmm_blasint)tn * typesize,
              &sldi, &sldo, LIBXSMM_DATATYPE_I8, LIBXSMM_DATATYPE_I8);
          }
          else { /* mzero */
            kernel.meltw_zero = libxsmm_dispatch_meltw_zero(
              (libxsmm_blasint)tm * typesize, (libxsmm_blasint)tn * typesize,
              &sldi, &sldo, LIBXSMM_DATATYPE_I8, LIBXSMM_DATATYPE_I8);
          }
#   else
          const libxsmm_mcopy_descriptor* desc;
          libxsmm_descriptor_blob blob;
          if (NULL != (desc = libxsmm_mcopy_descriptor_init(&blob,
            typesize, tm, tn, (unsigned int)ldo, (unsigned int)ldi,
            NULL != in ? 0 : LIBXSMM_MATCOPY_FLAG_ZERO_SOURCE,
            prefetch, NULL/*default unroll*/)))
          {
            kernel.xmcopy = libxsmm_dispatch_mcopy(desc);
          }
#   endif
        }
# endif
# if defined(LIBXSMM_EXT_TASKS) && 0/* implies _OPENMP */
        if (0 == omp_get_active_level())
# else
        if (0 == omp_in_parallel())
# endif
        { /* enable internal parallelization */
          const int nthreads = omp_get_max_threads();
# if defined(LIBXSMM_EXT_TASKS)
          if (0 >= libxsmm_xcopy_taskscale)
# endif
          {
#           pragma omp parallel num_threads(nthreads)
# if (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT))
            libxsmm_matcopy_thread_internal(out, in, typesize,
              (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
              tm, tn, kernel, omp_get_thread_num(), nthreads);
#else
            libxsmm_matcopy_thread_internal(out, in, typesize,
              (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
              tm, tn, kernel, omp_get_thread_num(), nthreads);
#endif
          }
# if defined(LIBXSMM_EXT_TASKS)
          else { /* tasks requested */
            const int ntasks = nthreads * libxsmm_xcopy_taskscale;
#           pragma omp parallel num_threads(nthreads)
            { /* first thread discovering work will launch all tasks */
#             pragma omp single nowait /* anyone is good */
              { int tid;
                for (tid = 0; tid < ntasks; ++tid) {
#                 pragma omp task untied
                  libxsmm_matcopy_thread_internal(out, in, typesize,
                    (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
                    tm, tn, kernel, tid, ntasks);
                }
              }
            }
          }
# endif
        }
        else { /* assume external parallelization */
# if defined(LIBXSMM_EXT_TASKS) /* implies _OPENMP */
          const int nthreads = omp_get_num_threads();
          const int ntasks = (0 == libxsmm_xcopy_taskscale
            ? (LIBXSMM_XCOPY_TASKSCALE)
            : libxsmm_xcopy_taskscale) * nthreads;
          int tid;
          for (tid = 0; tid < ntasks; ++tid) {
#           pragma omp task untied
            libxsmm_matcopy_thread_internal(out, in, typesize,
              (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
              tm, tn, kernel, tid, ntasks);
          }
          if (0 == libxsmm_nosync) { /* allow to omit synchronization */
#           pragma omp taskwait
          }
# elif (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT))
          libxsmm_matcopy_thread_internal(out, in, typesize,
            (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
            tm, tn, kernel, 0/*tid*/, 1/*nthreads*/);
# else
          libxsmm_matcopy_thread_internal(out, in, typesize,
            (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
            tm, tn, kernel, 0/*tid*/, 1/*nthreads*/);
# endif
        }
      }
      else
#endif /*defined(_OPENMP)*/
      if (NULL != in) { /* no MT, or small problem-size */
        LIBXSMM_XCOPY_NONJIT(LIBXSMM_MCOPY_KERNEL,
          typesize, out, in, ldi, ldo, 0, m, 0, n);
      }
      else { /* no MT, or small problem-size */
        /* coverity[ptr_arith] */
        LIBXSMM_XCOPY_NONJIT(LIBXSMM_MZERO_KERNEL,
          typesize, out, in, ldi, ldo, 0, m, 0, n);
      }
    }
  }
  else {
    static int error_once = 0;
    if ( 0 != libxsmm_verbosity /* library code is expected to be mute */
      && 1 == LIBXSMM_ATOMIC_ADD_FETCH(&error_once, 1, LIBXSMM_ATOMIC_RELAXED))
    {
      if (NULL == out) {
        fprintf(stderr, "LIBXSMM ERROR: the matrix-copy input and/or output is NULL!\n");
      }
      else if (out == in) {
        fprintf(stderr, "LIBXSMM ERROR: output and input of the matrix-copy must be different!\n");
      }
      else if (0 == typesize || 256 <= typesize) {
        fprintf(stderr, "LIBXSMM ERROR: invalid type-size for matrix-copy specified!\n");
      }
      else if (ldi < m || ldo < m) {
        fprintf(stderr, "LIBXSMM ERROR: the leading dimension(s) of the matrix-copy is/are too small!\n");
      }
      else if (0 > m || 0 > n) {
        fprintf(stderr, "LIBXSMM ERROR: the matrix extent(s) of the matrix-copy is/are negative!\n");
      }
    }
  }
}


LIBXSMM_APIEXT void libxsmm_otrans_omp(void* out, const void* in, unsigned int typesize,
  libxsmm_blasint m, libxsmm_blasint n, libxsmm_blasint ldi, libxsmm_blasint ldo)
{
  static int error_once = 0;
  LIBXSMM_INIT
  if (0 < typesize && 256 > typesize && m <= ldi && n <= ldo &&
    ((NULL != out && NULL != in && 0 < m && 0 < n) || (0 == m && 0 == n)))
  {
    if (0 < m && 0 < n) {
      if (out != in) {
#if defined(_OPENMP)
        unsigned int tm = LIBXSMM_UPDIV(libxsmm_tcopy_mbytes, typesize);
        unsigned int tn = (unsigned int)(libxsmm_tcopy_nscale * tm);
        if (0 == tm) tm = m;
        if (0 == tn) tn = LIBXSMM_MIN(LIBXSMM_XCOPY_TILE_MIN, n);
        if (0 != libxsmm_tcopy_mbytes && libxsmm_tcopy_mbytes < (tm * tn * typesize)) {
          tm = LIBXSMM_MAX(libxsmm_tcopy_mbytes / (tn * typesize), LIBXSMM_XCOPY_TILE_MIN);
        }
        if (tm <= (unsigned int)m && tn <= (unsigned int)n) { /* consider problem-size */
          libxsmm_xcopykernel kernel;
          kernel.ptr = NULL;
# if defined(LIBXSMM_EXT_TASKS) /* implies _OPENMP */
          if (0 == omp_get_active_level())
# else
          if (0 == omp_in_parallel())
# endif
          { /* enable internal parallelization */
            const int nthreads = omp_get_max_threads();
# if defined(LIBXSMM_EXT_TASKS)
            if (0 >= libxsmm_xcopy_taskscale)
# endif
            {
#             pragma omp parallel num_threads(nthreads)
              { /* coverity[divide_by_zero] */
                libxsmm_otrans_thread_internal(out, in, typesize,
                  (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
                  tm, tn, kernel, omp_get_thread_num(), nthreads);
              }
            }
# if defined(LIBXSMM_EXT_TASKS)
            else { /* tasks requested */
              const int ntasks = nthreads * libxsmm_xcopy_taskscale;
#             pragma omp parallel num_threads(nthreads)
              { /* first thread discovering work will launch all tasks */
#               pragma omp single nowait /* anyone is good */
                { int tid;
                  for (tid = 0; tid < ntasks; ++tid) {
#                   pragma omp task untied
                    libxsmm_otrans_thread_internal(out, in, typesize,
                      (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
                      tm, tn, kernel, tid, ntasks);
                  }
                }
              }
            }
# endif
          }
          else { /* assume external parallelization */
# if defined(LIBXSMM_EXT_TASKS) /* implies _OPENMP */
            const int nthreads = omp_get_num_threads();
            const int ntasks = (0 == libxsmm_xcopy_taskscale
              ? (LIBXSMM_XCOPY_TASKSCALE)
              : libxsmm_xcopy_taskscale) * nthreads;
            int tid;
            for (tid = 0; tid < ntasks; ++tid) {
#             pragma omp task untied
              libxsmm_otrans_thread_internal(out, in, typesize,
                (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
                tm, tn, kernel, tid, ntasks);
            }
            if (0 == libxsmm_nosync) { /* allow to omit synchronization */
#             pragma omp taskwait
            }
# else    /* coverity[divide_by_zero] */
            libxsmm_otrans_thread_internal(out, in, typesize,
              (unsigned int)m, (unsigned int)n, (unsigned int)ldi, (unsigned int)ldo,
              tm, tn, kernel, 0/*tid*/, 1/*nthreads*/);
# endif
          }
        }
        else
#endif /*defined(_OPENMP)*/
        { /* no MT, or small problem-size */
#if (defined(LIBXSMM_XCOPY_JIT) && 0 != (LIBXSMM_XCOPY_JIT))
          libxsmm_xcopykernel kernel;
          const libxsmm_trans_descriptor* desc;
          libxsmm_descriptor_blob blob;
          kernel.ptr = NULL;
          if (0 != (1 & libxsmm_xcopy_jit) /* JIT'ted transpose permitted? */
            && NULL != (desc = libxsmm_trans_descriptor_init(&blob, typesize, (unsigned int)m, (unsigned int)n, (unsigned int)ldo))
            && NULL != (kernel.xtrans = libxsmm_dispatch_trans(desc))) /* JIT-kernel available */
          {
            LIBXSMM_TCOPY_CALL(kernel, typesize, in, ldi, out, ldo);
          }
          else
#endif
          {
            LIBXSMM_XCOPY_NONJIT(LIBXSMM_TCOPY_KERNEL,
              typesize, out, in, ldi, ldo, 0, m, 0, n);
          }
        }
      }
      else if (ldi == ldo) {
        libxsmm_itrans/*TODO: omp*/(out, typesize, m, n, ldi);
      }
      else if (0 != libxsmm_verbosity /* library code is expected to be mute */
        && 1 == LIBXSMM_ATOMIC_ADD_FETCH(&error_once, 1, LIBXSMM_ATOMIC_RELAXED))
      {
        fprintf(stderr, "LIBXSMM ERROR: output and input of the transpose must be different!\n");
      }
    }
  }
  else {
    if (0 != libxsmm_verbosity /* library code is expected to be mute */
     && 1 == LIBXSMM_ATOMIC_ADD_FETCH(&error_once, 1, LIBXSMM_ATOMIC_RELAXED))
    {
      if (NULL == out || NULL == in) {
        fprintf(stderr, "LIBXSMM ERROR: the transpose input and/or output is NULL!\n");
      }
      else if (out == in) {
        fprintf(stderr, "LIBXSMM ERROR: output and input of the transpose must be different!\n");
      }
      else if (0 == typesize || 256 <= typesize) {
        fprintf(stderr, "LIBXSMM ERROR: invalid type-size for matrix-transpose specified!\n");
      }
      else if (ldi < m || ldo < n) {
        fprintf(stderr, "LIBXSMM ERROR: the leading dimension(s) of the transpose is/are too small!\n");
      }
      else if (0 > m || 0 > n) {
        fprintf(stderr, "LIBXSMM ERROR: the matrix extent(s) of the transpose is/are negative!\n");
      }
    }
  }
}


#if defined(LIBXSMM_BUILD) && defined(LIBXSMM_BUILD_EXT) && (!defined(LIBXSMM_NOFORTRAN) || defined(__clang_analyzer__))

/* implementation provided for Fortran 77 compatibility */
LIBXSMM_APIEXT void LIBXSMM_FSYMBOL(libxsmm_matcopy_omp)(void* /*out*/, const void* /*in*/, const int* /*typesize*/,
  const libxsmm_blasint* /*m*/, const libxsmm_blasint* /*n*/, const libxsmm_blasint* /*ldi*/, const libxsmm_blasint* /*ldo*/);
LIBXSMM_APIEXT void LIBXSMM_FSYMBOL(libxsmm_matcopy_omp)(void* out, const void* in, const int* typesize,
  const libxsmm_blasint* m, const libxsmm_blasint* n, const libxsmm_blasint* ldi, const libxsmm_blasint* ldo)
{
  libxsmm_blasint ldx;
  LIBXSMM_ASSERT(NULL != typesize && 0 < *typesize && NULL != m);
  ldx = *(NULL != ldi ? ldi : m);
  libxsmm_matcopy_omp(out, in, (unsigned int)*typesize, *m, *(NULL != n ? n : m), ldx, NULL != ldo ? *ldo : ldx);
}



/* implementation provided for Fortran 77 compatibility */
LIBXSMM_APIEXT void LIBXSMM_FSYMBOL(libxsmm_otrans_omp)(void* /*out*/, const void* /*in*/, const int* /*typesize*/,
  const libxsmm_blasint* /*m*/, const libxsmm_blasint* /*n*/, const libxsmm_blasint* /*ldi*/, const libxsmm_blasint* /*ldo*/);
LIBXSMM_APIEXT void LIBXSMM_FSYMBOL(libxsmm_otrans_omp)(void* out, const void* in, const int* typesize,
  const libxsmm_blasint* m, const libxsmm_blasint* n, const libxsmm_blasint* ldi, const libxsmm_blasint* ldo)
{
  libxsmm_blasint ldx;
  LIBXSMM_ASSERT(NULL != typesize && 0 < *typesize && NULL != m);
  ldx = *(NULL != ldi ? ldi : m);
  libxsmm_otrans_omp(out, in, (unsigned int)*typesize, *m, *(NULL != n ? n : m), ldx, NULL != ldo ? *ldo : ldx);
}

#endif /*defined(LIBXSMM_BUILD) && defined(LIBXSMM_BUILD_EXT) && (!defined(LIBXSMM_NOFORTRAN) || defined(__clang_analyzer__))*/

