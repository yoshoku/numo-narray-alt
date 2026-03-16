#ifndef NUMO_NARRAY_MH_TO_A_H
#define NUMO_NARRAY_MH_TO_A_H 1

#define DEF_NARRAY_TO_A_METHOD_FUNC(tDType)                                                    \
  static void iter_##tDType##_to_a(na_loop_t* const lp) {                                      \
    size_t n;                                                                                  \
    size_t s1;                                                                                 \
    char* p1;                                                                                  \
    size_t* idx1;                                                                              \
    tDType x;                                                                                  \
    volatile VALUE a, y;                                                                       \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);                                                         \
    a = rb_ary_new2(n);                                                                        \
    rb_ary_push(lp->args[1].value, a);                                                         \
    if (idx1) {                                                                                \
      for (size_t i = 0; i < n; i++) {                                                         \
        GET_DATA_INDEX(p1, idx1, tDType, x);                                                   \
        y = m_data_to_num(x);                                                                  \
        rb_ary_push(a, y);                                                                     \
      }                                                                                        \
    } else {                                                                                   \
      for (size_t i = 0; i < n; i++) {                                                         \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        y = m_data_to_num(x);                                                                  \
        rb_ary_push(a, y);                                                                     \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_to_a(VALUE self) {                                                     \
    ndfunc_arg_in_t ain[3] = { { Qnil, 0 }, { sym_loop_opt }, { sym_option } };                \
    ndfunc_arg_out_t aout[1] = { { rb_cArray, 0 } };                                           \
    ndfunc_t ndf = { iter_##tDType##_to_a, FULL_LOOP_NIP, 3, 1, ain, aout };                   \
    return na_ndloop_cast_narray_to_rarray(&ndf, self, Qnil);                                  \
  }

#endif /* NUMO_NARRAY_MH_TO_A_H */
