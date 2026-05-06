#ifndef NUMO_NARRAY_MH_MULSUM_H
#define NUMO_NARRAY_MH_MULSUM_H 1

#define DEF_FLT_MULSUM_NAN_ITER_FUNC(tDType)                                                   \
  static void iter_##tDType##_mulsum_nan(na_loop_t* const lp) {                                \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    char* p3;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    ssize_t s3;                                                                                \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    INIT_PTR(lp, 1, p2, s2);                                                                   \
    INIT_PTR(lp, 2, p3, s3);                                                                   \
    if (s3 == 0) {                                                                             \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType)) {                                    \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum_nan(((tDType*)p1)[i], ((tDType*)p2)[i], *(tDType*)p3);                    \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType))) {      \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum_nan(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                            \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      tDType z;                                                                                \
      GET_DATA(p3, tDType, z);                                                                 \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x;                                                                              \
        tDType y;                                                                              \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        m_mulsum_nan(x, y, z);                                                                 \
      }                                                                                        \
      SET_DATA(p3, tDType, z);                                                                 \
    } else {                                                                                   \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {            \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum_nan(((tDType*)p1)[i], ((tDType*)p2)[i], ((tDType*)p3)[i]);                \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&      \
            is_aligned_step(s3, sizeof(tDType))) {                                             \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum_nan(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                            \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
            p3 += s3;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x;                                                                              \
        tDType y;                                                                              \
        tDType z;                                                                              \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        GET_DATA(p3, tDType, z);                                                               \
        m_mulsum_nan(x, y, z);                                                                 \
        SET_DATA_STRIDE(p3, s3, tDType, z);                                                    \
      }                                                                                        \
    }                                                                                          \
  }

#define DEF_FLT_MULSUM_RUBY_FUNCS(tDType, tNAryClass)                                          \
  static VALUE tDType##_mulsum_self(int argc, VALUE* argv, VALUE self) {                       \
    if (argc < 1) {                                                                            \
      rb_raise(rb_eArgError, "wrong number of arguments (%d for >=1)", argc);                  \
    }                                                                                          \
                                                                                               \
    ndfunc_arg_in_t ain[4] = {                                                                 \
      { tNAryClass, 0 }, { tNAryClass, 0 }, { sym_reduce, 0 }, { sym_init, 0 }                 \
    };                                                                                         \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_mulsum, STRIDE_LOOP_NIP, 4, 1, ain, aout };               \
    VALUE naryv[2] = { self, argv[0] };                                                        \
    VALUE reduce =                                                                             \
      na_reduce_dimension(argc - 1, argv + 1, 2, naryv, &ndf, iter_##tDType##_mulsum_nan);     \
    VALUE v = na_ndloop(&ndf, 4, self, argv[0], reduce, m_mulsum_init);                        \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_mulsum(int argc, VALUE* argv, VALUE self) {                            \
    if (argc < 1) {                                                                            \
      rb_raise(rb_eArgError, "wrong number of arguments (%d for >=1)", argc);                  \
    }                                                                                          \
                                                                                               \
    VALUE klass = na_upcast(rb_obj_class(self), rb_obj_class(argv[0]));                        \
    if (klass == tNAryClass) {                                                                 \
      return tDType##_mulsum_self(argc, argv, self);                                           \
    }                                                                                          \
                                                                                               \
    VALUE v = rb_funcall(klass, id_cast, 1, self);                                             \
                                                                                               \
    return rb_funcallv_kw(v, rb_intern("mulsum"), argc, argv, RB_PASS_CALLED_KEYWORDS);        \
  }

#define DEF_INT_MULSUM_RUBY_FUNCS(tDType, tNAryClass)                                          \
  static VALUE tDType##_mulsum_self(int argc, VALUE* argv, VALUE self) {                       \
    if (argc < 1) {                                                                            \
      rb_raise(rb_eArgError, "wrong number of arguments (%d for >=1)", argc);                  \
    }                                                                                          \
                                                                                               \
    ndfunc_arg_in_t ain[4] = {                                                                 \
      { tNAryClass, 0 }, { tNAryClass, 0 }, { sym_reduce, 0 }, { sym_init, 0 }                 \
    };                                                                                         \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_mulsum, STRIDE_LOOP_NIP, 4, 1, ain, aout };               \
    VALUE naryv[2] = { self, argv[0] };                                                        \
    VALUE reduce = na_reduce_dimension(argc - 1, argv + 1, 2, naryv, &ndf, 0);                 \
    VALUE v = na_ndloop(&ndf, 4, self, argv[0], reduce, m_mulsum_init);                        \
                                                                                               \
    return rb_funcall(v, rb_intern("extract"), 0);                                             \
  }                                                                                            \
                                                                                               \
  static VALUE tDType##_mulsum(int argc, VALUE* argv, VALUE self) {                            \
    if (argc < 1) {                                                                            \
      rb_raise(rb_eArgError, "wrong number of arguments (%d for >=1)", argc);                  \
    }                                                                                          \
                                                                                               \
    VALUE klass = na_upcast(rb_obj_class(self), rb_obj_class(argv[0]));                        \
    if (klass == tNAryClass) {                                                                 \
      return tDType##_mulsum_self(argc, argv, self);                                           \
    }                                                                                          \
                                                                                               \
    VALUE v = rb_funcall(klass, id_cast, 1, self);                                             \
                                                                                               \
    return rb_funcallv_kw(v, rb_intern("mulsum"), argc, argv, RB_PASS_CALLED_KEYWORDS);        \
  }

#define DEF_NARRAY_FLT_MULSUM_METHOD_FUNC(tDType, tNAryClass)                                  \
  static void iter_##tDType##_mulsum(na_loop_t* const lp) {                                    \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    char* p3;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    ssize_t s3;                                                                                \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    INIT_PTR(lp, 1, p2, s2);                                                                   \
    INIT_PTR(lp, 2, p3, s3);                                                                   \
    if (s3 == 0) {                                                                             \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType)) {                                    \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(((tDType*)p1)[i], ((tDType*)p2)[i], *(tDType*)p3);                        \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType))) {      \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      tDType z;                                                                                \
      GET_DATA(p3, tDType, z);                                                                 \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x;                                                                              \
        tDType y;                                                                              \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        m_mulsum(x, y, z);                                                                     \
      }                                                                                        \
      SET_DATA(p3, tDType, z);                                                                 \
    } else {                                                                                   \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {            \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(((tDType*)p1)[i], ((tDType*)p2)[i], ((tDType*)p3)[i]);                    \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&      \
            is_aligned_step(s3, sizeof(tDType))) {                                             \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
            p3 += s3;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x;                                                                              \
        tDType y;                                                                              \
        tDType z;                                                                              \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        GET_DATA(p3, tDType, z);                                                               \
        m_mulsum(x, y, z);                                                                     \
        SET_DATA_STRIDE(p3, s3, tDType, z);                                                    \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
  DEF_FLT_MULSUM_NAN_ITER_FUNC(tDType)                                                         \
  DEF_FLT_MULSUM_RUBY_FUNCS(tDType, tNAryClass)

#define DEF_NARRAY_INT_MULSUM_METHOD_FUNC(tDType, tNAryClass)                                  \
  static void iter_##tDType##_mulsum(na_loop_t* const lp) {                                    \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    char* p3;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    ssize_t s3;                                                                                \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    INIT_PTR(lp, 1, p2, s2);                                                                   \
    INIT_PTR(lp, 2, p3, s3);                                                                   \
    if (s3 == 0) {                                                                             \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType)) {                                    \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(((tDType*)p1)[i], ((tDType*)p2)[i], *(tDType*)p3);                        \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType))) {      \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      tDType z;                                                                                \
      GET_DATA(p3, tDType, z);                                                                 \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x, y;                                                                           \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        m_mulsum(x, y, z);                                                                     \
      }                                                                                        \
      SET_DATA(p3, tDType, z);                                                                 \
    } else {                                                                                   \
      if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                  \
          is_aligned(p3, sizeof(tDType))) {                                                    \
        if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {            \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(((tDType*)p1)[i], ((tDType*)p2)[i], ((tDType*)p3)[i]);                    \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&      \
            is_aligned_step(s3, sizeof(tDType))) {                                             \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(tDType*)p1, *(tDType*)p2, *(tDType*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
            p3 += s3;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      for (size_t i = 0; i < n; i++) {                                                         \
        tDType x, y, z;                                                                        \
        GET_DATA_STRIDE(p1, s1, tDType, x);                                                    \
        GET_DATA_STRIDE(p2, s2, tDType, y);                                                    \
        GET_DATA(p3, tDType, z);                                                               \
        m_mulsum(x, y, z);                                                                     \
        SET_DATA_STRIDE(p3, s3, tDType, z);                                                    \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
  DEF_INT_MULSUM_RUBY_FUNCS(tDType, tNAryClass)

#define DEF_MULSUM_SFLT_SSE2_ITER_FUNC()                                                       \
  static void iter_sfloat_mulsum(na_loop_t* const lp) {                                        \
    size_t i = 0;                                                                              \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    char* p3;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    ssize_t s3;                                                                                \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    INIT_PTR(lp, 1, p2, s2);                                                                   \
    INIT_PTR(lp, 2, p3, s3);                                                                   \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(sfloat);                              \
    if (s3 == 0) {                                                                             \
      if (is_aligned(p1, sizeof(sfloat)) && is_aligned(p2, sizeof(sfloat)) &&                  \
          is_aligned(p3, sizeof(sfloat))) {                                                    \
        if (s1 == sizeof(sfloat) && s2 == sizeof(sfloat)) {                                    \
          sfloat z;                                                                            \
          GET_DATA(p3, sfloat, z);                                                             \
          if (n >= num_pack &&                                                                 \
              is_same_aligned2(&((sfloat*)p1)[i], &((sfloat*)p2)[i], SIMD_ALIGNMENT_SIZE)) {   \
            size_t cnt = (size_t)get_count_of_elements_not_aligned_to_simd_size(               \
              &((sfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(sfloat)                           \
            );                                                                                 \
            for (; i < cnt; i++) {                                                             \
              m_mulsum(((sfloat*)p1)[i], ((sfloat*)p2)[i], z);                                 \
            }                                                                                  \
            size_t cnt_simd_loop = (n - i) % num_pack;                                         \
            __m128 acc = _mm_setzero_ps();                                                     \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              __m128 a = _mm_load_ps(&((sfloat*)p1)[i]);                                       \
              __m128 b = _mm_load_ps(&((sfloat*)p2)[i]);                                       \
              acc = _mm_add_ps(acc, _mm_mul_ps(a, b));                                         \
            }                                                                                  \
            __m128 shuf = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));                   \
            acc = _mm_add_ps(acc, shuf);                                                       \
            shuf = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(0, 1, 2, 3));                          \
            acc = _mm_add_ps(acc, shuf);                                                       \
            z += _mm_cvtss_f32(acc);                                                           \
          }                                                                                    \
          for (; i < n; i++) {                                                                 \
            m_mulsum(((sfloat*)p1)[i], ((sfloat*)p2)[i], z);                                   \
          }                                                                                    \
          SET_DATA(p3, sfloat, z);                                                             \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(sfloat)) && is_aligned_step(s2, sizeof(sfloat))) {      \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(sfloat*)p1, *(sfloat*)p2, *(sfloat*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      sfloat z;                                                                                \
      GET_DATA(p3, sfloat, z);                                                                 \
      for (size_t i = 0; i < n; i++) {                                                         \
        sfloat x;                                                                              \
        sfloat y;                                                                              \
        GET_DATA_STRIDE(p1, s1, sfloat, x);                                                    \
        GET_DATA_STRIDE(p2, s2, sfloat, y);                                                    \
        m_mulsum(x, y, z);                                                                     \
      }                                                                                        \
      SET_DATA(p3, sfloat, z);                                                                 \
    } else {                                                                                   \
      if (is_aligned(p1, sizeof(sfloat)) && is_aligned(p2, sizeof(sfloat)) &&                  \
          is_aligned(p3, sizeof(sfloat))) {                                                    \
        if (s1 == sizeof(sfloat) && s2 == sizeof(sfloat) && s3 == sizeof(sfloat)) {            \
          if (n >= num_pack &&                                                                 \
              is_same_aligned3(                                                                \
                &((sfloat*)p1)[i], &((sfloat*)p2)[i], &((sfloat*)p3)[i], SIMD_ALIGNMENT_SIZE   \
              )) {                                                                             \
            size_t cnt = (size_t)get_count_of_elements_not_aligned_to_simd_size(               \
              &((sfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(sfloat)                           \
            );                                                                                 \
            for (; i < cnt; i++) {                                                             \
              m_mulsum(((sfloat*)p1)[i], ((sfloat*)p2)[i], ((sfloat*)p3)[i]);                  \
            }                                                                                  \
            size_t cnt_simd_loop = (n - i) % num_pack;                                         \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              __m128 a = _mm_load_ps(&((sfloat*)p1)[i]);                                       \
              __m128 b = _mm_load_ps(&((sfloat*)p2)[i]);                                       \
              __m128 c = _mm_load_ps(&((sfloat*)p3)[i]);                                       \
              _mm_store_ps(&((sfloat*)p3)[i], _mm_add_ps(_mm_mul_ps(a, b), c));                \
            }                                                                                  \
          }                                                                                    \
          for (; i < n; i++) {                                                                 \
            m_mulsum(((sfloat*)p1)[i], ((sfloat*)p2)[i], ((sfloat*)p3)[i]);                    \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(sfloat)) && is_aligned_step(s2, sizeof(sfloat)) &&      \
            is_aligned_step(s3, sizeof(sfloat))) {                                             \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(sfloat*)p1, *(sfloat*)p2, *(sfloat*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
            p3 += s3;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      for (size_t i = 0; i < n; i++) {                                                         \
        sfloat x;                                                                              \
        sfloat y;                                                                              \
        sfloat z;                                                                              \
        GET_DATA_STRIDE(p1, s1, sfloat, x);                                                    \
        GET_DATA_STRIDE(p2, s2, sfloat, y);                                                    \
        GET_DATA(p3, sfloat, z);                                                               \
        m_mulsum(x, y, z);                                                                     \
        SET_DATA_STRIDE(p3, s3, sfloat, z);                                                    \
      }                                                                                        \
    }                                                                                          \
  }

#define DEF_MULSUM_DFLT_SSE2_ITER_FUNC()                                                       \
  static void iter_dfloat_mulsum(na_loop_t* const lp) {                                        \
    size_t i = 0;                                                                              \
    size_t n;                                                                                  \
    char* p1;                                                                                  \
    char* p2;                                                                                  \
    char* p3;                                                                                  \
    ssize_t s1;                                                                                \
    ssize_t s2;                                                                                \
    ssize_t s3;                                                                                \
    INIT_COUNTER(lp, n);                                                                       \
    INIT_PTR(lp, 0, p1, s1);                                                                   \
    INIT_PTR(lp, 1, p2, s2);                                                                   \
    INIT_PTR(lp, 2, p3, s3);                                                                   \
    const size_t num_pack = SIMD_ALIGNMENT_SIZE / sizeof(dfloat);                              \
    if (s3 == 0) {                                                                             \
      if (is_aligned(p1, sizeof(dfloat)) && is_aligned(p2, sizeof(dfloat)) &&                  \
          is_aligned(p3, sizeof(dfloat))) {                                                    \
        if (s1 == sizeof(dfloat) && s2 == sizeof(dfloat)) {                                    \
          dfloat z;                                                                            \
          GET_DATA(p3, dfloat, z);                                                             \
          if (n >= num_pack &&                                                                 \
              is_same_aligned2(&((dfloat*)p1)[i], &((dfloat*)p2)[i], SIMD_ALIGNMENT_SIZE)) {   \
            size_t cnt = (size_t)get_count_of_elements_not_aligned_to_simd_size(               \
              &((dfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dfloat)                           \
            );                                                                                 \
            for (; i < cnt; i++) {                                                             \
              m_mulsum(((dfloat*)p1)[i], ((dfloat*)p2)[i], z);                                 \
            }                                                                                  \
            size_t cnt_simd_loop = (n - i) % num_pack;                                         \
            __m128d acc = _mm_setzero_pd();                                                    \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              __m128d a = _mm_load_pd(&((dfloat*)p1)[i]);                                      \
              __m128d b = _mm_load_pd(&((dfloat*)p2)[i]);                                      \
              acc = _mm_add_pd(acc, _mm_mul_pd(a, b));                                         \
            }                                                                                  \
            __m128d shuf = _mm_shuffle_pd(acc, acc, 1);                                        \
            acc = _mm_add_pd(acc, shuf);                                                       \
            z += _mm_cvtsd_f64(acc);                                                           \
          }                                                                                    \
          for (; i < n; i++) {                                                                 \
            m_mulsum(((dfloat*)p1)[i], ((dfloat*)p2)[i], z);                                   \
          }                                                                                    \
          SET_DATA(p3, dfloat, z);                                                             \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(dfloat)) && is_aligned_step(s2, sizeof(dfloat))) {      \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(dfloat*)p1, *(dfloat*)p2, *(dfloat*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      dfloat z;                                                                                \
      GET_DATA(p3, dfloat, z);                                                                 \
      for (size_t i = 0; i < n; i++) {                                                         \
        dfloat x;                                                                              \
        dfloat y;                                                                              \
        GET_DATA_STRIDE(p1, s1, dfloat, x);                                                    \
        GET_DATA_STRIDE(p2, s2, dfloat, y);                                                    \
        m_mulsum(x, y, z);                                                                     \
      }                                                                                        \
      SET_DATA(p3, dfloat, z);                                                                 \
    } else {                                                                                   \
      if (is_aligned(p1, sizeof(dfloat)) && is_aligned(p2, sizeof(dfloat)) &&                  \
          is_aligned(p3, sizeof(dfloat))) {                                                    \
        if (s1 == sizeof(dfloat) && s2 == sizeof(dfloat) && s3 == sizeof(dfloat)) {            \
          if (n >= num_pack &&                                                                 \
              is_same_aligned3(                                                                \
                &((dfloat*)p1)[i], &((dfloat*)p2)[i], &((dfloat*)p3)[i], SIMD_ALIGNMENT_SIZE   \
              )) {                                                                             \
            size_t cnt = (size_t)get_count_of_elements_not_aligned_to_simd_size(               \
              &((dfloat*)p1)[i], SIMD_ALIGNMENT_SIZE, sizeof(dfloat)                           \
            );                                                                                 \
            for (; i < cnt; i++) {                                                             \
              m_mulsum(((dfloat*)p1)[i], ((dfloat*)p2)[i], ((dfloat*)p3)[i]);                  \
            }                                                                                  \
            size_t cnt_simd_loop = (n - i) % num_pack;                                         \
            for (; i < n - cnt_simd_loop; i += num_pack) {                                     \
              __m128d a = _mm_load_pd(&((dfloat*)p1)[i]);                                      \
              __m128d b = _mm_load_pd(&((dfloat*)p2)[i]);                                      \
              __m128d c = _mm_load_pd(&((dfloat*)p3)[i]);                                      \
              _mm_store_pd(&((dfloat*)p3)[i], _mm_add_pd(_mm_mul_pd(a, b), c));                \
            }                                                                                  \
          }                                                                                    \
          for (; i < n; i++) {                                                                 \
            m_mulsum(((dfloat*)p1)[i], ((dfloat*)p2)[i], ((dfloat*)p3)[i]);                    \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
        if (is_aligned_step(s1, sizeof(dfloat)) && is_aligned_step(s2, sizeof(dfloat)) &&      \
            is_aligned_step(s3, sizeof(dfloat))) {                                             \
          for (size_t i = 0; i < n; i++) {                                                     \
            m_mulsum(*(dfloat*)p1, *(dfloat*)p2, *(dfloat*)p3);                                \
            p1 += s1;                                                                          \
            p2 += s2;                                                                          \
            p3 += s3;                                                                          \
          }                                                                                    \
          return;                                                                              \
        }                                                                                      \
      }                                                                                        \
      for (size_t i = 0; i < n; i++) {                                                         \
        dfloat x;                                                                              \
        dfloat y;                                                                              \
        dfloat z;                                                                              \
        GET_DATA_STRIDE(p1, s1, dfloat, x);                                                    \
        GET_DATA_STRIDE(p2, s2, dfloat, y);                                                    \
        GET_DATA(p3, dfloat, z);                                                               \
        m_mulsum(x, y, z);                                                                     \
        SET_DATA_STRIDE(p3, s3, dfloat, z);                                                    \
      }                                                                                        \
    }                                                                                          \
  }

#define DEF_NARRAY_SFLT_MULSUM_SSE2_METHOD_FUNC()                                              \
  DEF_MULSUM_SFLT_SSE2_ITER_FUNC()                                                             \
  DEF_FLT_MULSUM_NAN_ITER_FUNC(sfloat)                                                         \
  DEF_FLT_MULSUM_RUBY_FUNCS(sfloat, numo_cSFloat)

#define DEF_NARRAY_DFLT_MULSUM_SSE2_METHOD_FUNC()                                              \
  DEF_MULSUM_DFLT_SSE2_ITER_FUNC()                                                             \
  DEF_FLT_MULSUM_NAN_ITER_FUNC(dfloat)                                                         \
  DEF_FLT_MULSUM_RUBY_FUNCS(dfloat, numo_cDFloat)

#endif /* NUMO_NARRAY_MH_MULSUM_H */
