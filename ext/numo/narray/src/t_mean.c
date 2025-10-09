/*
  t_mean.c
  Numo::NArray Alternative

  created on: 2025-10-08
  Copyright (C) 2025 Atsushi Tatsuma
*/
#include <ruby.h>

#include "numo/narray.h"
#include "numo/template.h"

#define DEF_INT_MEAN_FUNC(tDType, tRtDType, tNAryClass, tRtNAryClass)                          \
  static void iter_mean_##tDType(na_loop_t* const lp) {                                        \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    size_t count = 0;                                                                          \
    tRtDType sum = 0;                                                                          \
    for (size_t i = n; i--;) {                                                                 \
      sum += (tRtDType)(*(tDType*)p1);                                                         \
      p1 += s1;                                                                                \
      count++;                                                                                 \
    }                                                                                          \
                                                                                               \
    *(tRtDType*)p2 = sum / (tRtDType)count;                                                    \
  }                                                                                            \
                                                                                               \
  static void iter_mean_nan_##tDType(na_loop_t* const lp) {                                    \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    size_t count = 0;                                                                          \
    tRtDType tmp = 0;                                                                          \
    tRtDType sum = 0;                                                                          \
    for (size_t i = n; i--;) {                                                                 \
      tmp = (tRtDType)(*(tDType*)p1);                                                          \
      p1 += s1;                                                                                \
      if (tmp == tmp) {                                                                        \
        sum += tmp;                                                                            \
        count++;                                                                               \
      }                                                                                        \
    }                                                                                          \
                                                                                               \
    *(tRtDType*)p2 = sum / (tRtDType)count;                                                    \
  }                                                                                            \
                                                                                               \
  static VALUE _mean_##tDType(int argc, VALUE* argv, VALUE self) {                             \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { sym_reduce, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { tRtNAryClass, 0 } };                                        \
    ndfunc_t ndf = { iter_mean_##tDType, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout }; \
    VALUE reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, iter_mean_nan_##tDType);    \
    VALUE v = na_ndloop(&ndf, 2, self, reduce);                                                \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }

DEF_INT_MEAN_FUNC(BIT_DIGIT, double, numo_cBit, numo_cDFloat)
DEF_INT_MEAN_FUNC(int8_t, double, numo_cInt8, numo_cDFloat)
DEF_INT_MEAN_FUNC(int16_t, double, numo_cInt16, numo_cDFloat)
DEF_INT_MEAN_FUNC(int32_t, double, numo_cInt32, numo_cDFloat)
DEF_INT_MEAN_FUNC(int64_t, double, numo_cInt64, numo_cDFloat)
DEF_INT_MEAN_FUNC(u_int8_t, double, numo_cUInt8, numo_cDFloat)
DEF_INT_MEAN_FUNC(u_int16_t, double, numo_cUInt16, numo_cDFloat)
DEF_INT_MEAN_FUNC(u_int32_t, double, numo_cUInt32, numo_cDFloat)
DEF_INT_MEAN_FUNC(u_int64_t, double, numo_cUInt64, numo_cDFloat)

#undef DEF_INT_MEAN_FUNC

void Init_nary_mean(void) {
  rb_define_method(numo_cBit, "mean", _mean_BIT_DIGIT, -1);
  rb_define_method(numo_cInt8, "mean", _mean_int8_t, -1);
  rb_define_method(numo_cInt16, "mean", _mean_int16_t, -1);
  rb_define_method(numo_cInt32, "mean", _mean_int32_t, -1);
  rb_define_method(numo_cInt64, "mean", _mean_int64_t, -1);
  rb_define_method(numo_cUInt8, "mean", _mean_u_int8_t, -1);
  rb_define_method(numo_cUInt16, "mean", _mean_u_int16_t, -1);
  rb_define_method(numo_cUInt32, "mean", _mean_u_int32_t, -1);
  rb_define_method(numo_cUInt64, "mean", _mean_u_int64_t, -1);
}
