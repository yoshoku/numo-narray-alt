#ifndef NUMO_NARRAY_MH_OP_BINARY_FUNC_H
#define NUMO_NARRAY_MH_OP_BINARY_FUNC_H 1

#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#define NUMO_USE_NEON 1
#endif

#define ITER_BINARY_INIT_VARS()                                                                \
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

#define ITER_BINARY_INPLACE_OR_NEW_ARY(fOpFunc, tDType)                                        \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p1)[i] = m_##fOpFunc(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p3)[i] = m_##fOpFunc(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
    }                                                                                          \
  }

#ifdef NUMO_USE_NEON
/* NEON vectorized binary operation for sfloat (float32) arrays */
#define ITER_BINARY_INPLACE_OR_NEW_ARY_NEON_SFLOAT(fOpFunc, neonOp)                            \
  {                                                                                            \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(sfloat);                              \
    size_t i = 0;                                                                              \
    size_t cnt = get_count_of_elements_not_aligned_to_simd_size(p1, SIMD_ALIGNMENT_SIZE,       \
                                                                sizeof(sfloat));              \
    size_t cnt_simd_loop;                                                                      \
    if (p1 == p3) {                                                                            \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((sfloat*)p1)[i] = m_##fOpFunc(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float32x4_t a = vld1q_f32(&((sfloat*)p1)[i]);                                          \
        float32x4_t b = vld1q_f32(&((sfloat*)p2)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f32(&((sfloat*)p1)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((sfloat*)p1)[i] = m_##fOpFunc(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
      }                                                                                        \
    } else {                                                                                   \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((sfloat*)p3)[i] = m_##fOpFunc(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float32x4_t a = vld1q_f32(&((sfloat*)p1)[i]);                                          \
        float32x4_t b = vld1q_f32(&((sfloat*)p2)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f32(&((sfloat*)p3)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((sfloat*)p3)[i] = m_##fOpFunc(((sfloat*)p1)[i], ((sfloat*)p2)[i]);                    \
      }                                                                                        \
    }                                                                                          \
  }

/* NEON vectorized binary operation for dfloat (float64) arrays */
#define ITER_BINARY_INPLACE_OR_NEW_ARY_NEON_DFLOAT(fOpFunc, neonOp)                            \
  {                                                                                            \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dfloat);                              \
    size_t i = 0;                                                                              \
    size_t cnt = get_count_of_elements_not_aligned_to_simd_size(p1, SIMD_ALIGNMENT_SIZE,       \
                                                                sizeof(dfloat));              \
    size_t cnt_simd_loop;                                                                      \
    if (p1 == p3) {                                                                            \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((dfloat*)p1)[i] = m_##fOpFunc(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float64x2_t a = vld1q_f64(&((dfloat*)p1)[i]);                                          \
        float64x2_t b = vld1q_f64(&((dfloat*)p2)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f64(&((dfloat*)p1)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((dfloat*)p1)[i] = m_##fOpFunc(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
      }                                                                                        \
    } else {                                                                                   \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((dfloat*)p3)[i] = m_##fOpFunc(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float64x2_t a = vld1q_f64(&((dfloat*)p1)[i]);                                          \
        float64x2_t b = vld1q_f64(&((dfloat*)p2)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f64(&((dfloat*)p3)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((dfloat*)p3)[i] = m_##fOpFunc(((dfloat*)p1)[i], ((dfloat*)p2)[i]);                    \
      }                                                                                        \
    }                                                                                          \
  }

/* NEON vectorized scalar operation for sfloat (float32) arrays */
#define ITER_BINARY_INPLACE_OR_NEW_SCL_NEON_SFLOAT(fOpFunc, neonOp)                            \
  {                                                                                            \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(sfloat);                              \
    size_t i = 0;                                                                              \
    size_t cnt = get_count_of_elements_not_aligned_to_simd_size(p1, SIMD_ALIGNMENT_SIZE,       \
                                                                sizeof(sfloat));              \
    size_t cnt_simd_loop;                                                                      \
    float32x4_t b = vdupq_n_f32(*(sfloat*)p2);                                                 \
    if (p1 == p3) {                                                                            \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((sfloat*)p1)[i] = m_##fOpFunc(((sfloat*)p1)[i], *(sfloat*)p2);                        \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float32x4_t a = vld1q_f32(&((sfloat*)p1)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f32(&((sfloat*)p1)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((sfloat*)p1)[i] = m_##fOpFunc(((sfloat*)p1)[i], *(sfloat*)p2);                        \
      }                                                                                        \
    } else {                                                                                   \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((sfloat*)p3)[i] = m_##fOpFunc(((sfloat*)p1)[i], *(sfloat*)p2);                        \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float32x4_t a = vld1q_f32(&((sfloat*)p1)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f32(&((sfloat*)p3)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((sfloat*)p3)[i] = m_##fOpFunc(((sfloat*)p1)[i], *(sfloat*)p2);                        \
      }                                                                                        \
    }                                                                                          \
  }

/* NEON vectorized scalar operation for dfloat (float64) arrays */
#define ITER_BINARY_INPLACE_OR_NEW_SCL_NEON_DFLOAT(fOpFunc, neonOp)                            \
  {                                                                                            \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dfloat);                              \
    size_t i = 0;                                                                              \
    size_t cnt = get_count_of_elements_not_aligned_to_simd_size(p1, SIMD_ALIGNMENT_SIZE,       \
                                                                sizeof(dfloat));              \
    size_t cnt_simd_loop;                                                                      \
    float64x2_t b = vdupq_n_f64(*(dfloat*)p2);                                                 \
    if (p1 == p3) {                                                                            \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((dfloat*)p1)[i] = m_##fOpFunc(((dfloat*)p1)[i], *(dfloat*)p2);                        \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float64x2_t a = vld1q_f64(&((dfloat*)p1)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f64(&((dfloat*)p1)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((dfloat*)p1)[i] = m_##fOpFunc(((dfloat*)p1)[i], *(dfloat*)p2);                        \
      }                                                                                        \
    } else {                                                                                   \
      for (i = 0; i < cnt && i < n; i++) {                                                     \
        ((dfloat*)p3)[i] = m_##fOpFunc(((dfloat*)p1)[i], *(dfloat*)p2);                        \
      }                                                                                        \
      cnt_simd_loop = (n - i) / num_pack;                                                      \
      for (size_t j = 0; j < cnt_simd_loop; j++, i += num_pack) {                              \
        float64x2_t a = vld1q_f64(&((dfloat*)p1)[i]);                                          \
        a = neonOp(a, b);                                                                      \
        vst1q_f64(&((dfloat*)p3)[i], a);                                                       \
      }                                                                                        \
      for (; i < n; i++) {                                                                     \
        ((dfloat*)p3)[i] = m_##fOpFunc(((dfloat*)p1)[i], *(dfloat*)p2);                        \
      }                                                                                        \
    }                                                                                          \
  }
#endif /* NUMO_USE_NEON */

#define ITER_BINARY_INPLACE_OR_NEW_ARY_ZERODIV(fOpFunc, tDType)                                \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      if ((((tDType*)p2)[i]) == 0) {                                                           \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      ((tDType*)p1)[i] = m_##fOpFunc(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      if ((((tDType*)p2)[i]) == 0) {                                                           \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      ((tDType*)p3)[i] = m_##fOpFunc(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
    }                                                                                          \
  }

#define ITER_BINARY_INPLACE_OR_NEW_PTR_ARY(fOpFunc, tDType)                                    \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      *(tDType*)p1 = m_##fOpFunc(*(tDType*)p1, *(tDType*)p2);                                  \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      *(tDType*)p3 = m_##fOpFunc(*(tDType*)p1, *(tDType*)p2);                                  \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
      p3 += s3;                                                                                \
    }                                                                                          \
  }

#define ITER_BINARY_INPLACE_OR_NEW_PTR_ARY_ZERODIV(fOpFunc, tDType)                            \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      if ((*(tDType*)p2) == 0) {                                                               \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      *(tDType*)p1 = m_##fOpFunc(*(tDType*)p1, *(tDType*)p2);                                  \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      if ((*(tDType*)p2) == 0) {                                                               \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      *(tDType*)p3 = m_##fOpFunc(*(tDType*)p1, *(tDType*)p2);                                  \
      p1 += s1;                                                                                \
      p2 += s2;                                                                                \
      p3 += s3;                                                                                \
    }                                                                                          \
  }

#define ITER_BINARY_INPLACE_OR_NEW_SCL(fOpFunc, tDType)                                        \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p1)[i] = m_##fOpFunc(((tDType*)p1)[i], *(tDType*)p2);                          \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p3)[i] = m_##fOpFunc(((tDType*)p1)[i], *(tDType*)p2);                          \
    }                                                                                          \
  }

#define ITER_BINARY_NEW_PTR_SCL(fOpFunc, tDType)                                               \
  for (size_t i = 0; i < n; i++) {                                                             \
    *(tDType*)p3 = m_##fOpFunc(*(tDType*)p1, *(tDType*)p2);                                    \
    p1 += s1;                                                                                  \
    p3 += s3;                                                                                  \
  }

#define ITER_BINARY_FALLBACK_LOOP(fOpFunc, tDType)                                             \
  for (size_t i = 0; i < n; i++) {                                                             \
    tDType x;                                                                                  \
    tDType y;                                                                                  \
    tDType z;                                                                                  \
    GET_DATA_STRIDE(p1, s1, tDType, x);                                                        \
    GET_DATA_STRIDE(p2, s2, tDType, y);                                                        \
    z = m_##fOpFunc(x, y);                                                                     \
    SET_DATA_STRIDE(p3, s3, tDType, z);                                                        \
  }

#define DEF_BINARY_SELF_FUNC(fOpFunc, tDType, tNAryClass)                                      \
  static VALUE tDType##_##fOpFunc##_self(VALUE self, VALUE other) {                            \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { tNAryClass, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_##fOpFunc, STRIDE_LOOP, 2, 1, ain, aout };                \
    return na_ndloop(&ndf, 2, self, other);                                                    \
  }

#define DEF_BINARY_FUNC(fOpFunc, sRbOp, tDType, tNAryClass)                                    \
  static VALUE tDType##_##fOpFunc(VALUE self, VALUE other) {                                   \
    VALUE klass = na_upcast(rb_obj_class(self), rb_obj_class(other));                          \
    if (klass == tNAryClass) {                                                                 \
      return tDType##_##fOpFunc##_self(self, other);                                           \
    } else {                                                                                   \
      VALUE v = rb_funcall(klass, id_cast, 1, self);                                           \
      return rb_funcall(v, sRbOp, 1, other);                                                   \
    }                                                                                          \
  }

#endif /* NUMO_NARRAY_MH_OP_BINARY_FUNC_H */
