/*
  rms.h
  Numo::NArray Alternative

  created on: 2025-10-15
  Copyright (C) 2025 Atsushi Tatsuma
*/
#ifndef NUMO_NARRAY_MH_RMS_H
#define NUMO_NARRAY_MH_RMS_H 1

#define DEF_NARRAY_FLT_RMS_METHOD_FUNC(tDType, tRtDType, tNAryClass, tRtNAryClass)             \
  static void iter_##tDType##_rms(na_loop_t* const lp) {                                       \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    *(tRtDType*)p2 = f_rms(n, p1, s1);                                                         \
  }                                                                                            \
                                                                                               \
  static void iter_##tDType##_rms_nan(na_loop_t* const lp) {                                   \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    *(tRtDType*)p2 = f_rms_nan(n, p1, s1);                                                     \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_rms(int argc, VALUE* argv, VALUE self) {                               \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { sym_reduce, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { tRtNAryClass, 0 } };                                        \
    ndfunc_t ndf = {                                                                           \
      iter_##tDType##_rms, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout                  \
    };                                                                                         \
    VALUE reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, iter_##tDType##_rms_nan);   \
    VALUE v = na_ndloop(&ndf, 2, self, reduce);                                                \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }

#define DEF_NARRAY_INT_RMS_METHOD_FUNC(tDType, tNAryClass)                                     \
  static void iter_##tDType##_rms(na_loop_t* const lp) {                                       \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    *(double*)p2 = f_rms(n, p1, s1);                                                           \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_rms(int argc, VALUE* argv, VALUE self) {                               \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { sym_reduce, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { numo_cDFloat, 0 } };                                        \
    ndfunc_t ndf = {                                                                           \
      iter_##tDType##_rms, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout                  \
    };                                                                                         \
    VALUE reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);                         \
    VALUE v = na_ndloop(&ndf, 2, self, reduce);                                                \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }

#define DEF_NARRAY_BIT_RMS_METHOD_FUNC()                                                       \
  static void iter_bit_rms(na_loop_t* const lp) {                                              \
    size_t n;                                                                                  \
    BIT_DIGIT* p1;                                                                             \
    size_t ps1;                                                                                \
    ssize_t s1;                                                                                \
    size_t* idx1;                                                                              \
    char* p2;                                                                                  \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_BIT_IDX(lp, 0, p1, ps1, s1, idx1);                                                \
    p2 = NDL_PTR(lp, 1);                                                                       \
                                                                                               \
    *(double*)p2 = f_rms(n, p1, ps1, s1, idx1);                                                \
  }                                                                                            \
                                                                                               \
  static VALUE bit_rms(int argc, VALUE* argv, VALUE self) {                                    \
    ndfunc_arg_in_t ain[2] = { { numo_cBit, 0 }, { sym_reduce, 0 } };                          \
    ndfunc_arg_out_t aout[1] = { { numo_cDFloat, 0 } };                                        \
    ndfunc_t ndf = { iter_bit_rms, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout };       \
    VALUE reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);                         \
    VALUE v = na_ndloop(&ndf, 2, self, reduce);                                                \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }

#endif // NUMO_NARRAY_MH_RMS_H
