#ifndef NUMO_NARRAY_MH_OP_SUB_H
#define NUMO_NARRAY_MH_OP_SUB_H 1

#ifndef PARALLEL_FOR_OMP_SIMD
#if defined(_OPENMP)
#include <omp.h>
#if _OPENMP >= 201511
#define PARALLEL_FOR_OMP_SIMD _Pragma("omp parallel for simd")
#else
#define PARALLEL_FOR_OMP_SIMD
#endif /* _OPENMP >= 201511 */
#else
#define PARALLEL_FOR_OMP_SIMD
#endif /* defined(_OPENMP) */
#endif /* PARALLEL_FOR_OMP */

/* Common macro for variable declaration and initialization */
#define ITER_SUB_INIT_VARS()                                                                   \
  size_t n;                                                                                    \
  char* p1;                                                                                    \
  char* p2;                                                                                    \
  char* p3;                                                                                    \
  ssize_t s1;                                                                                  \
  ssize_t s2;                                                                                  \
  ssize_t s3;                                                                                  \
  INIT_COUNTER(lp, n);                                                                         \
  INIT_PTR(lp, 0, p1, s1);                                                                     \
  INIT_PTR(lp, 1, p2, s2);                                                                     \
  INIT_PTR(lp, 2, p3, s3);

/* Common macro for in-place or new array operation */
#define ITER_SUB_INPLACE_OR_NEW_P2_ARY(tDType)                                                 \
  if (p1 == p3) {                                                                              \
    PARALLEL_FOR_OMP_SIMD                                                                      \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p1)[i] = m_sub(((tDType*)p1)[i], ((tDType*)p2)[i]);                            \
    }                                                                                          \
  } else {                                                                                     \
    PARALLEL_FOR_OMP_SIMD                                                                      \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p3)[i] = m_sub(((tDType*)p1)[i], ((tDType*)p2)[i]);                            \
    }                                                                                          \
  }

#define ITER_SUB_INPLACE_OR_NEW_P2_SCL(tDType)                                                 \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p1)[i] = m_sub(((tDType*)p1)[i], *(tDType*)p2);                                \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p3)[i] = m_sub(((tDType*)p1)[i], *(tDType*)p2);                                \
    }                                                                                          \
  }

#define ITER_SUB_INPLACE_OR_NEW_PTR_P2_ARY(tDType)                                             \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      *(tDType*)p1 = m_sub(*(tDType*)p1, *(tDType*)p2);                                        \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      *(tDType*)p3 = m_sub(*(tDType*)p1, *(tDType*)p2);                                        \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
      p3 += s3;                                                                                \
    }                                                                                          \
  }

#define ITER_SUB_NEW_PTR_P2_SCL(tDType)                                                        \
  for (size_t i = 0; i < n; i++) {                                                             \
    *(tDType*)p3 = m_sub(*(tDType*)p1, *(tDType*)p2);                                          \
    p1 += s1;                                                                                  \
    p3 += s3;                                                                                  \
  }

/* Common macro for fallback loop with stride */
#define ITER_SUB_FALLBACK_LOOP(tDType)                                                         \
  for (size_t i = 0; i < n; i++) {                                                             \
    tDType x;                                                                                  \
    tDType y;                                                                                  \
    tDType z;                                                                                  \
    GET_DATA_STRIDE(p1, s1, tDType, x);                                                        \
    GET_DATA_STRIDE(p2, s2, tDType, y);                                                        \
    z = m_sub(x, y);                                                                           \
    SET_DATA_STRIDE(p3, s3, tDType, z);                                                        \
  }

/* Common macro for add_self function */
#define DEF_SUB_SELF_FUNC(tDType, tNAryClass)                                                  \
  static VALUE tDType##_sub_self(VALUE self, VALUE other) {                                    \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { tNAryClass, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_sub, STRIDE_LOOP, 2, 1, ain, aout };                      \
    return na_ndloop(&ndf, 2, self, other);                                                    \
  }

/* Common macro for add function */
#define DEF_SUB_FUNC(tDType, tNAryClass)                                                       \
  static VALUE tDType##_sub(VALUE self, VALUE other) {                                         \
    VALUE klass = na_upcast(rb_obj_class(self), rb_obj_class(other));                          \
    if (klass == tNAryClass) {                                                                 \
      return tDType##_sub_self(self, other);                                                   \
    } else {                                                                                   \
      VALUE v = rb_funcall(klass, id_cast, 1, self);                                           \
      return rb_funcall(v, '-', 1, other);                                                     \
    }                                                                                          \
  }

#define DEF_NARRAY_SUB_METHOD_FUNC(tDType, tNAryClass)                                         \
  static void iter_##tDType##_sub(na_loop_t* const lp) {                                       \
    ITER_SUB_INIT_VARS()                                                                       \
                                                                                               \
    if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                    \
        is_aligned(p3, sizeof(tDType))) {                                                      \
      if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {              \
        ITER_SUB_INPLACE_OR_NEW_P2_ARY(tDType)                                                 \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&        \
          is_aligned_step(s3, sizeof(tDType))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                  \
            ITER_SUB_INPLACE_OR_NEW_P2_SCL(tDType)                                             \
          } else {                                                                             \
            ITER_SUB_NEW_PTR_P2_SCL(tDType)                                                    \
          }                                                                                    \
        } else {                                                                               \
          ITER_SUB_INPLACE_OR_NEW_PTR_P2_ARY(tDType)                                           \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_SUB_FALLBACK_LOOP(tDType)                                                             \
  }                                                                                            \
                                                                                               \
  DEF_SUB_SELF_FUNC(tDType, tNAryClass)                                                        \
  DEF_SUB_FUNC(tDType, tNAryClass)

#define DEF_NARRAY_INT8_SUB_METHOD_FUNC(tDType, tNAryClass)                                    \
  static void iter_##tDType##_sub(na_loop_t* const lp) {                                       \
    ITER_SUB_INIT_VARS()                                                                       \
                                                                                               \
    if (s2 == 0) {                                                                             \
      if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                      \
        ITER_SUB_INPLACE_OR_NEW_P2_SCL(tDType)                                                 \
      } else {                                                                                 \
        ITER_SUB_NEW_PTR_P2_SCL(tDType)                                                        \
      }                                                                                        \
    } else {                                                                                   \
      ITER_SUB_INPLACE_OR_NEW_PTR_P2_ARY(tDType)                                               \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_SUB_SELF_FUNC(tDType, tNAryClass)                                                        \
  DEF_SUB_FUNC(tDType, tNAryClass)

#define DEF_NARRAY_ROBJ_SUB_METHOD_FUNC()                                                      \
  static void iter_robject_sub(na_loop_t* const lp) {                                          \
    ITER_SUB_INIT_VARS()                                                                       \
                                                                                               \
    if (s2 == 0) {                                                                             \
      if (s1 == sizeof(robject) && s3 == sizeof(robject)) {                                    \
        ITER_SUB_INPLACE_OR_NEW_P2_SCL(robject)                                                \
      } else {                                                                                 \
        ITER_SUB_NEW_PTR_P2_SCL(robject)                                                       \
      }                                                                                        \
    } else {                                                                                   \
      ITER_SUB_INPLACE_OR_NEW_PTR_P2_ARY(robject)                                              \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_SUB_SELF_FUNC(robject, numo_cRObject)                                                    \
  static VALUE robject_sub(VALUE self, VALUE other) {                                          \
    return robject_sub_self(self, other);                                                      \
  }

#define DEF_NARRAY_SFLT_SUB_SSE2_METHOD_FUNC()                                                 \
  static void iter_sfloat_sub(na_loop_t* const lp) {                                           \
    size_t i = 0;                                                                              \
    ITER_ADD_INIT_VARS()                                                                       \
                                                                                               \
    size_t cnt;                                                                                \
    size_t cnt_simd_loop = -1;                                                                 \
    __m128 a;                                                                                  \
    __m128 b;                                                                                  \
    size_t num_pack;                                                                           \
    num_pack = SIMD_ALIGNMENT_SIZE / sizeof(sfloat);                                           \
                                                                                               \
    if (is_aligned(p1, sizeof(sfloat)) && is_aligned(p2, sizeof(sfloat)) &&                    \
        is_aligned(p3, sizeof(sfloat))) {                                                      \
      if (s1 == sizeof(sfloat) && s2 == sizeof(sfloat) && s3 == sizeof(sfloat)) {              \
        if ((n >= num_pack) &&                                                                 \
            is_same_aligned3(                                                                  \
              &((sfloat*)p1)[i], &((sfloat*)p2)[i], &((sfloat*)p3)[i], SIMD_ALIGNMENT_SIZE     \
            )) {                                                                               \
          cnt = get_count_of_elements_not_aligned_to_simd_size(                                \
            &((sfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(sfloat)                             \
          );                                                                                   \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < cnt; i++) {                                                        \
              ((sfloat*)p1)[i] = m_sub(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < cnt; i++) {                                                        \
              ((sfloat*)p3)[i] = m_sub(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          }                                                                                    \
                                                                                               \
          cnt_simd_loop = (n - i) % num_pack;                                                  \
                                                                                               \
          if (p1 == p3) {                                                                      \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_ps(&((sfloat*)p1)[i]);                                              \
              b = _mm_load_ps(&((sfloat*)p2)[i]);                                              \
              a = _mm_sub_ps(a, b);                                                            \
              _mm_store_ps(&((sfloat*)p1)[i], a);                                              \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_ps(&((sfloat*)p1)[i]);                                              \
              b = _mm_load_ps(&((sfloat*)p2)[i]);                                              \
              a = _mm_sub_ps(a, b);                                                            \
              _mm_stream_ps(&((sfloat*)p3)[i], a);                                             \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        if (cnt_simd_loop != 0) {                                                              \
          if (p1 == p3) {                                                                      \
            for (; i < n; i++) {                                                               \
              ((sfloat*)p1)[i] = m_sub(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n; i++) {                                                               \
              ((sfloat*)p3)[i] = m_sub(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
                                                                                               \
      if (is_aligned_step(s1, sizeof(sfloat)) && is_aligned_step(s2, sizeof(sfloat)) &&        \
          is_aligned_step(s3, sizeof(sfloat))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(sfloat) && s3 == sizeof(sfloat)) {                                  \
            b = _mm_load1_ps(&((sfloat*)p2)[0]);                                               \
            if ((n >= num_pack) &&                                                             \
                is_same_aligned2(&((sfloat*)p1)[i], &((sfloat*)p3)[i], SIMD_ALIGNMENT_SIZE)) { \
              cnt = get_count_of_elements_not_aligned_to_simd_size(                            \
                &((sfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(sfloat)                         \
              );                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < cnt; i++) {                                                         \
                  ((sfloat*)p1)[i] = m_sub(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (; i < cnt; i++) {                                                         \
                  ((sfloat*)p3)[i] = m_sub(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
                                                                                               \
              cnt_simd_loop = (n - i) % num_pack;                                              \
                                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_ps(&((sfloat*)p1)[i]);                                          \
                  a = _mm_sub_ps(a, b);                                                        \
                  _mm_store_ps(&((sfloat*)p1)[i], a);                                          \
                }                                                                              \
              } else {                                                                         \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_ps(&((sfloat*)p1)[i]);                                          \
                  a = _mm_sub_ps(a, b);                                                        \
                  _mm_stream_ps(&((sfloat*)p3)[i], a);                                         \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
            if (cnt_simd_loop != 0) {                                                          \
              if (p1 == p3) {                                                                  \
                for (; i < n; i++) {                                                           \
                  ((sfloat*)p1)[i] = m_sub(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (; i < n; i++) {                                                           \
                  ((sfloat*)p3)[i] = m_sub(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p3 = m_sub(*(sfloat*)p1, *(sfloat*)p2);                                \
              p1 += s1;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p1 = m_sub(*(sfloat*)p1, *(sfloat*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p3 = m_sub(*(sfloat*)p1, *(sfloat*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    for (i = 0; i < n; i++) {                                                                  \
      sfloat x;                                                                                \
      sfloat y;                                                                                \
      sfloat z;                                                                                \
      GET_DATA_STRIDE(p1, s1, sfloat, x);                                                      \
      GET_DATA_STRIDE(p2, s2, sfloat, y);                                                      \
      z = m_sub(x, y);                                                                         \
      SET_DATA_STRIDE(p3, s3, sfloat, z);                                                      \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_SUB_SELF_FUNC(sfloat, numo_cSFloat)                                                      \
  DEF_SUB_FUNC(sfloat, numo_cSFloat)

#define DEF_NARRAY_DFLT_SUB_SSE2_METHOD_FUNC()                                                 \
  static void iter_dfloat_sub(na_loop_t* const lp) {                                           \
    size_t i = 0;                                                                              \
    ITER_ADD_INIT_VARS()                                                                       \
                                                                                               \
    size_t cnt;                                                                                \
    size_t cnt_simd_loop = -1;                                                                 \
    __m128d a;                                                                                 \
    __m128d b;                                                                                 \
    size_t num_pack;                                                                           \
    num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dtype);                                            \
                                                                                               \
    if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&                      \
        is_aligned(p3, sizeof(dtype))) {                                                       \
      if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {                 \
        if ((n >= num_pack) &&                                                                 \
            is_same_aligned3(                                                                  \
              &((dtype*)p1)[i], &((dtype*)p2)[i], &((dtype*)p3)[i], SIMD_ALIGNMENT_SIZE        \
            )) {                                                                               \
          cnt = get_count_of_elements_not_aligned_to_simd_size(                                \
            &((dtype*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dtype)                               \
          );                                                                                   \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < cnt; i++) {                                                        \
              ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);                       \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < cnt; i++) {                                                        \
              ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);                       \
            }                                                                                  \
          }                                                                                    \
                                                                                               \
          cnt_simd_loop = (n - i) % num_pack;                                                  \
                                                                                               \
          if (p1 == p3) {                                                                      \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_pd(&((dtype*)p1)[i]);                                               \
              b = _mm_load_pd(&((dtype*)p2)[i]);                                               \
              a = _mm_sub_pd(a, b);                                                            \
              _mm_store_pd(&((dtype*)p1)[i], a);                                               \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_pd(&((dtype*)p1)[i]);                                               \
              b = _mm_load_pd(&((dtype*)p2)[i]);                                               \
              a = _mm_sub_pd(a, b);                                                            \
              _mm_stream_pd(&((dtype*)p3)[i], a);                                              \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        if (cnt_simd_loop != 0) {                                                              \
          if (p1 == p3) {                                                                      \
            for (; i < n; i++) {                                                               \
              ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);                       \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n; i++) {                                                               \
              ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);                       \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
                                                                                               \
      if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&          \
          is_aligned_step(s3, sizeof(dtype))) {                                                \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {                                    \
            b = _mm_load1_pd(&((dtype*)p2)[0]);                                                \
            if ((n >= num_pack) &&                                                             \
                is_same_aligned2(&((dtype*)p1)[i], &((dtype*)p3)[i], SIMD_ALIGNMENT_SIZE)) {   \
              cnt = get_count_of_elements_not_aligned_to_simd_size(                            \
                &((dtype*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dtype)                           \
              );                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < cnt; i++) {                                                         \
                  ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);                       \
                }                                                                              \
              } else {                                                                         \
                for (; i < cnt; i++) {                                                         \
                  ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);                       \
                }                                                                              \
              }                                                                                \
                                                                                               \
              cnt_simd_loop = (n - i) % num_pack;                                              \
                                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_pd(&((dtype*)p1)[i]);                                           \
                  a = _mm_sub_pd(a, b);                                                        \
                  _mm_store_pd(&((dtype*)p1)[i], a);                                           \
                }                                                                              \
              } else {                                                                         \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_pd(&((dtype*)p1)[i]);                                           \
                  a = _mm_sub_pd(a, b);                                                        \
                  _mm_stream_pd(&((dtype*)p3)[i], a);                                          \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
            if (cnt_simd_loop != 0) {                                                          \
              if (p1 == p3) {                                                                  \
                for (; i < n; i++) {                                                           \
                  ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);                       \
                }                                                                              \
              } else {                                                                         \
                for (; i < n; i++) {                                                           \
                  ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);                       \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(dtype*)p3 = m_sub(*(dtype*)p1, *(dtype*)p2);                                   \
              p1 += s1;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < n; i++) {                                                          \
              *(dtype*)p1 = m_sub(*(dtype*)p1, *(dtype*)p2);                                   \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(dtype*)p3 = m_sub(*(dtype*)p1, *(dtype*)p2);                                   \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    for (i = 0; i < n; i++) {                                                                  \
      dtype x;                                                                                 \
      dtype y;                                                                                 \
      dtype z;                                                                                 \
      GET_DATA_STRIDE(p1, s1, dtype, x);                                                       \
      GET_DATA_STRIDE(p2, s2, dtype, y);                                                       \
      z = m_sub(x, y);                                                                         \
      SET_DATA_STRIDE(p3, s3, dtype, z);                                                       \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_SUB_SELF_FUNC(dfloat, numo_cDFloat)                                                      \
  DEF_SUB_FUNC(dfloat, numo_cDFloat)

#endif /* NUMO_NARRAY_MH_OP_SUB_H */
