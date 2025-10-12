/*
  mean.h
  Numo::NArray Alternative

  created on: 2025-10-08
  Copyright (C) 2025 Atsushi Tatsuma
*/
#ifndef NUMO_NARRAY_MH_MEAN_H
#define NUMO_NARRAY_MH_MEAN_H 1

#define DEF_NARRAY_MEAN_METHOD_FUNC(tDType, tRtDType, tNAryClass, tRtNAryClass)               \
  static void iter_##tDType##_mean(na_loop_t* const lp) {                                     \
    size_t n;                                                                                 \
    char* p1;                                                                                 \
    char* p2;                                                                                 \
    ssize_t s1;                                                                               \
                                                                                              \
    INIT_COUNTER(lp, n);                                                                      \
    INIT_PTR(lp, 0, p1, s1);                                                                  \
    p2 = NDL_PTR(lp, 1);                                                                      \
                                                                                              \
    size_t count = 0;                                                                         \
    tRtDType sum = m_zero;                                                                    \
    for (size_t i = n; i--;) {                                                                \
      const tRtDType x = (tRtDType)(*(tDType*)p1);                                            \
      p1 += s1;                                                                               \
      sum = m_add(sum, x);                                                                    \
      count++;                                                                                \
    }                                                                                         \
                                                                                              \
    *(tRtDType*)p2 = m_div_r(sum, count);                                                     \
  }                                                                                           \
                                                                                              \
  static void iter_##tDType##_mean_nan(na_loop_t* const lp) {                                 \
    size_t n;                                                                                 \
    char* p1;                                                                                 \
    char* p2;                                                                                 \
    ssize_t s1;                                                                               \
                                                                                              \
    INIT_COUNTER(lp, n);                                                                      \
    INIT_PTR(lp, 0, p1, s1);                                                                  \
    p2 = NDL_PTR(lp, 1);                                                                      \
                                                                                              \
    size_t count = 0;                                                                         \
    tRtDType sum = m_zero;                                                                    \
    for (size_t i = n; i--;) {                                                                \
      const tRtDType tmp = (tRtDType)(*(tDType*)p1);                                          \
      p1 += s1;                                                                               \
      if (not_nan(tmp)) {                                                                     \
        sum = m_add(sum, tmp);                                                                \
        count++;                                                                              \
      }                                                                                       \
    }                                                                                         \
                                                                                              \
    *(tRtDType*)p2 = m_div_r(sum, count);                                                     \
  }                                                                                           \
                                                                                              \
  static VALUE tDType##_mean(int argc, VALUE* argv, VALUE self) {                             \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { sym_reduce, 0 } };                        \
    ndfunc_arg_out_t aout[1] = { { tRtNAryClass, 0 } };                                       \
    ndfunc_t ndf = {                                                                          \
      iter_##tDType##_mean, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout                \
    };                                                                                        \
    VALUE reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, iter_##tDType##_mean_nan); \
    VALUE v = na_ndloop(&ndf, 2, self, reduce);                                               \
                                                                                              \
    return rb_funcall(v, rb_intern("extract"), 0);                                            \
  }

#endif // NUMO_NARRAY_MH_MEAN_H
