#ifndef NUMO_NARRAY_MH_FILL_H
#define NUMO_NARRAY_MH_FILL_H 1

#define DEF_NARRAY_FILL_METHOD_FUNC(tDType)                                                    \
  static void iter_##tDType##_fill(na_loop_t* const lp) {                                      \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    ssize_t s1;                                                                                \
    size_t* idx1;                                                                              \
    VALUE x = lp->option;                                                                      \
    tDType y;                                                                                  \
                                                                                               \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);                                                         \
    y = m_num_to_data(x);                                                                      \
                                                                                               \
    if (idx1) {                                                                                \
      for (size_t i = 0; i < n; i++) {                                                         \
        SET_DATA_INDEX(p1, idx1, tDType, y);                                                   \
      }                                                                                        \
    } else {                                                                                   \
      for (size_t i = 0; i < n; i++) {                                                         \
        SET_DATA_STRIDE(p1, s1, tDType, y);                                                    \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_fill(VALUE self, VALUE val) {                                          \
    ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { sym_option } };                             \
    ndfunc_t ndf = { iter_##tDType##_fill, FULL_LOOP, 2, 0, ain, 0 };                          \
    na_ndloop(&ndf, 2, self, val);                                                             \
    return self;                                                                               \
  }

#endif /* NUMO_NARRAY_MH_FILL_H */
