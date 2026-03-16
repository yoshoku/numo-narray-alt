#ifndef NUMO_NARRAY_MH_EACH_H
#define NUMO_NARRAY_MH_EACH_H 1

#define DEF_NARRAY_EACH_METHOD_FUNC(tDType)                                                    \
  static void iter_##tDType##_each(na_loop_t* const lp) {                                      \
    size_t n;                                                                                  \
    size_t s1;                                                                                 \
    char* p1;                                                                                  \
    size_t* idx1;                                                                              \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);                                                         \
    tDType x;                                                                                  \
    VALUE y;                                                                                   \
    if (idx1) {                                                                                \
      for (size_t i = 0; i < n; i++) {                                                         \
        GET_DATA_INDEX(p1, idx1, tDType, x);                                                   \
        y = m_data_to_num(x);                                                                  \
        rb_yield(y);                                                                           \
      }                                                                                        \
    } else {                                                                                   \
      for (size_t i = 0; i < n; i++) {                                                         \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        y = m_data_to_num(x);                                                                  \
        rb_yield(y);                                                                           \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_each(VALUE self) {                                                     \
    ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };                                                  \
    ndfunc_t ndf = { iter_##tDType##_each, FULL_LOOP_NIP, 1, 0, ain, 0 };                      \
    na_ndloop(&ndf, 1, self);                                                                  \
    return self;                                                                               \
  }

#endif /* NUMO_NARRAY_MH_EACH_H */
