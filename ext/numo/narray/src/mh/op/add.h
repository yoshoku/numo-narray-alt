#ifndef NUMO_NARRAY_MH_OP_ADD_H
#define NUMO_NARRAY_MH_OP_ADD_H 1

#include "binary_func.h"

#define DEF_NARRAY_ADD_METHOD_FUNC(tDType, tNAryClass)                                         \
  static void iter_##tDType##_add(na_loop_t* const lp) {                                       \
    ITER_BINARY_INIT_VARS()                                                                    \
    if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                    \
        is_aligned(p3, sizeof(tDType))) {                                                      \
      if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {              \
        ITER_BINARY_INPLACE_OR_NEW_ARY(add, tDType)                                            \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&        \
          is_aligned_step(s3, sizeof(tDType))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                  \
            ITER_BINARY_INPLACE_OR_NEW_SCL(add, tDType)                                        \
          } else {                                                                             \
            ITER_BINARY_NEW_PTR_SCL(add, tDType)                                               \
          }                                                                                    \
        } else {                                                                               \
          ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(add, tDType)                                      \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_BINARY_FALLBACK_LOOP(add, tDType)                                                     \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, tDType, tNAryClass)                                                \
  DEF_BINARY_FUNC(add, '+', tDType, tNAryClass)

#define DEF_NARRAY_INT8_ADD_METHOD_FUNC(tDType, tNAryClass)                                    \
  static void iter_##tDType##_add(na_loop_t* const lp) {                                       \
    ITER_BINARY_INIT_VARS()                                                                    \
    if (s2 == 0) {                                                                             \
      if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                      \
        ITER_BINARY_INPLACE_OR_NEW_SCL(add, tDType)                                            \
      } else {                                                                                 \
        ITER_BINARY_NEW_PTR_SCL(add, tDType)                                                   \
      }                                                                                        \
    } else {                                                                                   \
      ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(add, tDType)                                          \
    }                                                                                          \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, tDType, tNAryClass)                                                \
  DEF_BINARY_FUNC(add, '+', tDType, tNAryClass)

#define DEF_NARRAY_ROBJ_ADD_METHOD_FUNC()                                                      \
  static void iter_robject_add(na_loop_t* const lp) {                                          \
    ITER_BINARY_INIT_VARS()                                                                    \
    if (s2 == 0) {                                                                             \
      if (s1 == sizeof(robject) && s3 == sizeof(robject)) {                                    \
        ITER_BINARY_INPLACE_OR_NEW_SCL(add, robject)                                           \
      } else {                                                                                 \
        ITER_BINARY_NEW_PTR_SCL(add, robject)                                                  \
      }                                                                                        \
    } else {                                                                                   \
      ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(add, robject)                                         \
    }                                                                                          \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, robject, numo_cRObject)                                            \
  static VALUE robject_add(VALUE self, VALUE other) {                                          \
    return robject_add_self(self, other);                                                      \
  }

#define DEF_NARRAY_SFLT_ADD_SSE2_METHOD_FUNC()                                                 \
  static void iter_sfloat_add(na_loop_t* const lp) {                                           \
    size_t i = 0;                                                                              \
    ITER_BINARY_INIT_VARS()                                                                    \
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
              ((sfloat*)p1)[i] = m_add(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < cnt; i++) {                                                        \
              ((sfloat*)p3)[i] = m_add(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          }                                                                                    \
                                                                                               \
          cnt_simd_loop = (n - i) % num_pack;                                                  \
                                                                                               \
          if (p1 == p3) {                                                                      \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_ps(&((sfloat*)p1)[i]);                                              \
              b = _mm_load_ps(&((sfloat*)p2)[i]);                                              \
              a = _mm_add_ps(a, b);                                                            \
              _mm_store_ps(&((sfloat*)p1)[i], a);                                              \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_ps(&((sfloat*)p1)[i]);                                              \
              b = _mm_load_ps(&((sfloat*)p2)[i]);                                              \
              a = _mm_add_ps(a, b);                                                            \
              _mm_stream_ps(&((sfloat*)p3)[i], a);                                             \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        if (cnt_simd_loop != 0) {                                                              \
          if (p1 == p3) {                                                                      \
            for (; i < n; i++) {                                                               \
              ((sfloat*)p1)[i] = m_add(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n; i++) {                                                               \
              ((sfloat*)p3)[i] = m_add(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
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
                for (i = 0; i < cnt; i++) {                                                    \
                  ((sfloat*)p1)[i] = m_add(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (i = 0; i < cnt; i++) {                                                    \
                  ((sfloat*)p3)[i] = m_add(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
                                                                                               \
              cnt_simd_loop = (n - i) % num_pack;                                              \
                                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_ps(&((sfloat*)p1)[i]);                                          \
                  a = _mm_add_ps(a, b);                                                        \
                  _mm_store_ps(&((sfloat*)p1)[i], a);                                          \
                }                                                                              \
              } else {                                                                         \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_ps(&((sfloat*)p1)[i]);                                          \
                  a = _mm_add_ps(a, b);                                                        \
                  _mm_stream_ps(&((sfloat*)p3)[i], a);                                         \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
            if (cnt_simd_loop != 0) {                                                          \
              if (p1 == p3) {                                                                  \
                for (; i < n; i++) {                                                           \
                  ((sfloat*)p1)[i] = m_add(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (; i < n; i++) {                                                           \
                  ((sfloat*)p3)[i] = m_add(((sfloat*)p1)[i], *(sfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p3 = m_add(*(sfloat*)p1, *(sfloat*)p2);                                \
              p1 += s1;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p1 = m_add(*(sfloat*)p1, *(sfloat*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(sfloat*)p3 = m_add(*(sfloat*)p1, *(sfloat*)p2);                                \
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
      z = m_add(x, y);                                                                         \
      SET_DATA_STRIDE(p3, s3, sfloat, z);                                                      \
    }                                                                                          \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, sfloat, numo_cSFloat)                                              \
  DEF_BINARY_FUNC(add, '+', sfloat, numo_cSFloat)

#define DEF_NARRAY_DFLT_ADD_SSE2_METHOD_FUNC()                                                 \
  static void iter_dfloat_add(na_loop_t* const lp) {                                           \
    size_t i = 0;                                                                              \
    ITER_BINARY_INIT_VARS()                                                                    \
                                                                                               \
    size_t cnt;                                                                                \
    size_t cnt_simd_loop = -1;                                                                 \
    __m128d a;                                                                                 \
    __m128d b;                                                                                 \
    size_t num_pack;                                                                           \
    num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dfloat);                                           \
                                                                                               \
    if (is_aligned(p1, sizeof(dfloat)) && is_aligned(p2, sizeof(dfloat)) &&                    \
        is_aligned(p3, sizeof(dfloat))) {                                                      \
      if (s1 == sizeof(dfloat) && s2 == sizeof(dfloat) && s3 == sizeof(dfloat)) {              \
        if ((n >= num_pack) &&                                                                 \
            is_same_aligned3(                                                                  \
              &((dfloat*)p1)[i], &((dfloat*)p2)[i], &((dfloat*)p3)[i], SIMD_ALIGNMENT_SIZE     \
            )) {                                                                               \
          cnt = get_count_of_elements_not_aligned_to_simd_size(                                \
            &((dfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dfloat)                             \
          );                                                                                   \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < cnt; i++) {                                                        \
              ((dfloat*)p1)[i] = m_add(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < cnt; i++) {                                                        \
              ((dfloat*)p3)[i] = m_add(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
            }                                                                                  \
          }                                                                                    \
                                                                                               \
          cnt_simd_loop = (n - i) % num_pack;                                                  \
                                                                                               \
          if (p1 == p3) {                                                                      \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_pd(&((dfloat*)p1)[i]);                                              \
              b = _mm_load_pd(&((dfloat*)p2)[i]);                                              \
              a = _mm_add_pd(a, b);                                                            \
              _mm_store_pd(&((dfloat*)p1)[i], a);                                              \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              a = _mm_load_pd(&((dfloat*)p1)[i]);                                              \
              b = _mm_load_pd(&((dfloat*)p2)[i]);                                              \
              a = _mm_add_pd(a, b);                                                            \
              _mm_stream_pd(&((dfloat*)p3)[i], a);                                             \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
                                                                                               \
        if (cnt_simd_loop != 0) {                                                              \
          if (p1 == p3) {                                                                      \
            for (; i < n; i++) {                                                               \
              ((dfloat*)p1)[i] = m_add(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
            }                                                                                  \
          } else {                                                                             \
            for (; i < n; i++) {                                                               \
              ((dfloat*)p3)[i] = m_add(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
                                                                                               \
      if (is_aligned_step(s1, sizeof(dfloat)) && is_aligned_step(s2, sizeof(dfloat)) &&        \
          is_aligned_step(s3, sizeof(dfloat))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(dfloat) && s3 == sizeof(dfloat)) {                                  \
            b = _mm_load1_pd(&((dfloat*)p2)[0]);                                               \
            if ((n >= num_pack) &&                                                             \
                is_same_aligned2(&((dfloat*)p1)[i], &((dfloat*)p3)[i], SIMD_ALIGNMENT_SIZE)) { \
              cnt = get_count_of_elements_not_aligned_to_simd_size(                            \
                &((dfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dfloat)                         \
              );                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < cnt; i++) {                                                         \
                  ((dfloat*)p1)[i] = m_add(((dfloat*)p1)[i], *(dfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (; i < cnt; i++) {                                                         \
                  ((dfloat*)p3)[i] = m_add(((dfloat*)p1)[i], *(dfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
                                                                                               \
              cnt_simd_loop = (n - i) % num_pack;                                              \
                                                                                               \
              if (p1 == p3) {                                                                  \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_pd(&((dfloat*)p1)[i]);                                          \
                  a = _mm_add_pd(a, b);                                                        \
                  _mm_store_pd(&((dfloat*)p1)[i], a);                                          \
                }                                                                              \
              } else {                                                                         \
                for (; i < n - cnt_simd_loop; i += num_pack) {                                 \
                  a = _mm_load_pd(&((dfloat*)p1)[i]);                                          \
                  a = _mm_add_pd(a, b);                                                        \
                  _mm_stream_pd(&((dfloat*)p3)[i], a);                                         \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
                                                                                               \
            if (cnt_simd_loop != 0) {                                                          \
              if (p1 == p3) {                                                                  \
                for (; i < n; i++) {                                                           \
                  ((dfloat*)p1)[i] = m_add(((dfloat*)p1)[i], *(dfloat*)p2);                    \
                }                                                                              \
              } else {                                                                         \
                for (; i < n; i++) {                                                           \
                  ((dfloat*)p3)[i] = m_add(((dfloat*)p1)[i], *(dfloat*)p2);                    \
                }                                                                              \
              }                                                                                \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(dfloat*)p3 = m_add(*(dfloat*)p1, *(dfloat*)p2);                                \
              p1 += s1;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (i = 0; i < n; i++) {                                                          \
              *(dfloat*)p1 = m_add(*(dfloat*)p1, *(dfloat*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (i = 0; i < n; i++) {                                                          \
              *(dfloat*)p3 = m_add(*(dfloat*)p1, *(dfloat*)p2);                                \
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
      dfloat x;                                                                                \
      dfloat y;                                                                                \
      dfloat z;                                                                                \
      GET_DATA_STRIDE(p1, s1, dfloat, x);                                                      \
      GET_DATA_STRIDE(p2, s2, dfloat, y);                                                      \
      z = m_add(x, y);                                                                         \
      SET_DATA_STRIDE(p3, s3, dfloat, z);                                                      \
    }                                                                                          \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, dfloat, numo_cDFloat)                                              \
  DEF_BINARY_FUNC(add, '+', dfloat, numo_cDFloat)

#ifdef NUMO_USE_NEON
/* NEON optimized sfloat add method */
#define DEF_NARRAY_SFLT_ADD_NEON_METHOD_FUNC()                                                 \
  static void iter_sfloat_add(na_loop_t* const lp) {                                           \
    ITER_BINARY_INIT_VARS()                                                                    \
    if (is_aligned(p1, sizeof(sfloat)) && is_aligned(p2, sizeof(sfloat)) &&                    \
        is_aligned(p3, sizeof(sfloat))) {                                                      \
      if (s1 == sizeof(sfloat) && s2 == sizeof(sfloat) && s3 == sizeof(sfloat)) {              \
        ITER_BINARY_INPLACE_OR_NEW_ARY_NEON_SFLOAT(add, vaddq_f32)                             \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(sfloat)) && is_aligned_step(s2, sizeof(sfloat)) &&        \
          is_aligned_step(s3, sizeof(sfloat))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(sfloat) && s3 == sizeof(sfloat)) {                                  \
            ITER_BINARY_INPLACE_OR_NEW_SCL_NEON_SFLOAT(add, vaddq_f32)                         \
          } else {                                                                             \
            ITER_BINARY_NEW_PTR_SCL(add, sfloat)                                               \
          }                                                                                    \
        } else {                                                                               \
          ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(add, sfloat)                                      \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_BINARY_FALLBACK_LOOP(add, sfloat)                                                     \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, sfloat, numo_cSFloat)                                              \
  DEF_BINARY_FUNC(add, '+', sfloat, numo_cSFloat)

/* NEON optimized dfloat add method */
#define DEF_NARRAY_DFLT_ADD_NEON_METHOD_FUNC()                                                 \
  static void iter_dfloat_add(na_loop_t* const lp) {                                           \
    ITER_BINARY_INIT_VARS()                                                                    \
    if (is_aligned(p1, sizeof(dfloat)) && is_aligned(p2, sizeof(dfloat)) &&                    \
        is_aligned(p3, sizeof(dfloat))) {                                                      \
      if (s1 == sizeof(dfloat) && s2 == sizeof(dfloat) && s3 == sizeof(dfloat)) {              \
        ITER_BINARY_INPLACE_OR_NEW_ARY_NEON_DFLOAT(add, vaddq_f64)                             \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(dfloat)) && is_aligned_step(s2, sizeof(dfloat)) &&        \
          is_aligned_step(s3, sizeof(dfloat))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(dfloat) && s3 == sizeof(dfloat)) {                                  \
            ITER_BINARY_INPLACE_OR_NEW_SCL_NEON_DFLOAT(add, vaddq_f64)                         \
          } else {                                                                             \
            ITER_BINARY_NEW_PTR_SCL(add, dfloat)                                               \
          }                                                                                    \
        } else {                                                                               \
          ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(add, dfloat)                                      \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_BINARY_FALLBACK_LOOP(add, dfloat)                                                     \
  }                                                                                            \
  DEF_BINARY_SELF_FUNC(add, dfloat, numo_cDFloat)                                              \
  DEF_BINARY_FUNC(add, '+', dfloat, numo_cDFloat)
#endif /* NUMO_USE_NEON */

#endif /* NUMO_NARRAY_MH_OP_ADD_H */
