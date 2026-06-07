#ifndef NUMO_NARRAY_MH_MAP_H
#define NUMO_NARRAY_MH_MAP_H 1

#define DEF_NARRAY_MAP_METHOD_FUNC(tDType, tNAryClass)                                         \
  static void iter_##tDType##_map(na_loop_t* const lp) {                                       \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    size_t* idx1;                                                                              \
    size_t* idx2;                                                                              \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);                                                         \
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);                                                         \
    tDType x;                                                                                  \
    printf("\e[35mn=%zu s1=%zd s2=%zd\e[0m\n", n, s1, s2); \
    if (idx1) {                                                                                \
      if (idx2) {                                                                              \
	puts("\e[34midx1 and idx2\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_INDEX(p1, idx1, tDType, x);                                                 \
          x = m_map(x);                                                                        \
          SET_DATA_INDEX(p2, idx2, tDType, x);                                                 \
        }                                                                                      \
      } else {                                                                                 \
	puts("\e[34midx1 not idx2\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_INDEX(p1, idx1, tDType, x);                                                 \
          x = m_map(x);                                                                        \
          SET_DATA_STRIDE(p2, s2, tDType, x);                                                  \
        }                                                                                      \
      }                                                                                        \
    } else {                                                                                   \
      if (idx2) {                                                                              \
	puts("\e[34midx2 not idx1\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_STRIDE(p1, s1, tDType, x);                                                  \
          x = m_map(x);                                                                        \
          SET_DATA_INDEX(p2, idx2, tDType, x);                                                 \
        }                                                                                      \
      } else {                                                                                 \
	puts("\e[34mneither idx1 nor idx2\e[0m"); \
        if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType))) {                \
	  puts("\e[34m both aligned\e[0m"); \
          if (s1 == sizeof(tDType) && s2 == sizeof(tDType)) {                                  \
	    puts("\e[34m same size\e[0m"); \
            for (size_t i = 0; i < n; i++) {                                                   \
	      printf("  \e[36mi=%zu\e[0m\n", i); \
		x = ((tDType*)p1)[i]; \
		/* VALUE y = m_data_to_num(x); */ \
		/* y = rb_yield(y); */ \
		/* x = m_num_to_data(y); */ \
		/* x = m_map(x); */ \
		x = m_num_to_data(rb_yield(m_data_to_num(x))); \
              /*((tDType*)p2)[i] = m_map(((tDType*)p1)[i]);*/                                      \
              ((tDType*)p2)[i] = x;                                      \
            }                                                                                  \
            return;                                                                            \
          }                                                                                    \
          if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType))) {    \
	    puts("\e[34m aligned steps\e[0m"); \
            for (size_t i = 0; i < n; i++) {                                                   \
              *(tDType*)p2 = m_map(*(tDType*)p1);                                              \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
            return;                                                                            \
          }                                                                                    \
        }                                                                                      \
	puts("\e[34m fallback\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_STRIDE(p1, s1, tDType, x);                                                  \
          x = m_map(x);                                                                        \
          SET_DATA_STRIDE(p2, s2, tDType, x);                                                  \
        }                                                                                      \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_map(VALUE self) {                                                      \
    ndfunc_arg_in_t ain[1] = { { tNAryClass, 0 } };                                            \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_map, FULL_LOOP, 1, 1, ain, aout };                        \
    return na_ndloop(&ndf, 1, self);                                                           \
  }

#define DEF_NARRAY_INT8_MAP_METHOD_FUNC(tDType, tNAryClass)                                    \
  static void iter_##tDType##_map(na_loop_t* const lp) {                                       \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    size_t* idx1;                                                                              \
    size_t* idx2;                                                                              \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);                                                         \
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);                                                         \
    tDType x;                                                                                  \
    if (idx1) {                                                                                \
      if (idx2) {                                                                              \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_INDEX(p1, idx1, tDType, x);                                                 \
          x = m_map(x);                                                                        \
          SET_DATA_INDEX(p2, idx2, tDType, x);                                                 \
        }                                                                                      \
      } else {                                                                                 \
	puts("\e[34mCase 2\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_INDEX(p1, idx1, tDType, x);                                                 \
          x = m_map(x);                                                                        \
          SET_DATA_STRIDE(p2, s2, tDType, x);                                                  \
        }                                                                                      \
      }                                                                                        \
    } else {                                                                                   \
      if (idx2) {                                                                              \
	puts("\e[34mCase 3\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          GET_DATA_STRIDE(p1, s1, tDType, x);                                                  \
          x = m_map(x);                                                                        \
          SET_DATA_INDEX(p2, idx2, tDType, x);                                                 \
        }                                                                                      \
      } else {                                                                                 \
	puts("\e[34mCase 4\e[0m"); \
        for (size_t i = 0; i < n; i++) {                                                       \
          *(tDType*)p2 = m_map(*(tDType*)p1);                                                  \
          p1 += s1;                                                                            \
          p2 += s2;                                                                            \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_map(VALUE self) {                                                      \
    ndfunc_arg_in_t ain[1] = { { tNAryClass, 0 } };                                            \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_map, FULL_LOOP, 1, 1, ain, aout };                        \
    return na_ndloop(&ndf, 1, self);                                                           \
  }

#define DEF_NARRAY_ROBJ_MAP_METHOD_FUNC()                                                      \
  DEF_NARRAY_INT8_MAP_METHOD_FUNC(robject, numo_cRObject)

#endif /* NUMO_NARRAY_MH_MAP_H */
