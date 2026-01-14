#ifndef NUMO_NARRAY_MH_OP_MOD_H
#define NUMO_NARRAY_MH_OP_MOD_H 1

/* Common macro for variable declaration and initialization */
#define ITER_MOD_INIT_VARS()                                                                   \
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

/* Common macro for in-place or new array operation */
#define ITER_MOD_INPLACE_OR_NEW_P2_SCL(tDType)                                                 \
  if (p1 == p3) {                                                                              \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p1)[i] = m_mod(((tDType*)p1)[i], *(tDType*)p2);                                \
    }                                                                                          \
  } else {                                                                                     \
    for (size_t i = 0; i < n; i++) {                                                           \
      ((tDType*)p3)[i] = m_mod(((tDType*)p1)[i], *(tDType*)p2);                                \
    }                                                                                          \
  }

#define ITER_MOD_NEW_PTR_P2_SCL(tDType)                                                        \
  for (size_t i = 0; i < n; i++) {                                                             \
    *(tDType*)p3 = m_mod(*(tDType*)p1, *(tDType*)p2);                                          \
    p1 += s1;                                                                                  \
    p3 += s3;                                                                                  \
  }

/* Common macro for fallback loop with stride */
#define ITER_MOD_FALLBACK_LOOP(tDType)                                                         \
  for (size_t i = 0; i < n; i++) {                                                             \
    tDType x;                                                                                  \
    tDType y;                                                                                  \
    tDType z;                                                                                  \
    GET_DATA_STRIDE(p1, s1, tDType, x);                                                        \
    GET_DATA_STRIDE(p2, s2, tDType, y);                                                        \
    z = m_mod(x, y);                                                                           \
    SET_DATA_STRIDE(p3, s3, tDType, z);                                                        \
  }

/* Common macro for add_self function */
#define DEF_MOD_SELF_FUNC(tDType, tNAryClass)                                                  \
  static VALUE tDType##_mod_self(VALUE self, VALUE other) {                                    \
    ndfunc_arg_in_t ain[2] = { { tNAryClass, 0 }, { tNAryClass, 0 } };                         \
    ndfunc_arg_out_t aout[1] = { { tNAryClass, 0 } };                                          \
    ndfunc_t ndf = { iter_##tDType##_mod, STRIDE_LOOP, 2, 1, ain, aout };                      \
    return na_ndloop(&ndf, 2, self, other);                                                    \
  }

/* Common macro for add function */
#define DEF_MOD_FUNC(tDType, tNAryClass)                                                       \
  static VALUE tDType##_mod(VALUE self, VALUE other) {                                         \
    VALUE klass = na_upcast(rb_obj_class(self), rb_obj_class(other));                          \
    if (klass == tNAryClass) {                                                                 \
      return tDType##_mod_self(self, other);                                                   \
    } else {                                                                                   \
      VALUE v = rb_funcall(klass, id_cast, 1, self);                                           \
      return rb_funcall(v, '/', 1, other);                                                     \
    }                                                                                          \
  }

#define DEF_NARRAY_FLT_MOD_METHOD_FUNC(tDType, tNAryClass)                                     \
  static void iter_##tDType##_mod(na_loop_t* const lp) {                                       \
    ITER_MOD_INIT_VARS()                                                                       \
                                                                                               \
    if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                    \
        is_aligned(p3, sizeof(tDType))) {                                                      \
      if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {              \
        if (p1 == p3) {                                                                        \
          for (size_t i = 0; i < n; i++) {                                                     \
            ((tDType*)p1)[i] = m_mod(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
          }                                                                                    \
        } else {                                                                               \
          for (size_t i = 0; i < n; i++) {                                                     \
            ((tDType*)p3)[i] = m_mod(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&        \
          is_aligned_step(s3, sizeof(tDType))) {                                               \
        if (s2 == 0) {                                                                         \
          if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                  \
            ITER_MOD_INPLACE_OR_NEW_P2_SCL(tDType)                                             \
          } else {                                                                             \
            ITER_MOD_NEW_PTR_P2_SCL(tDType)                                                    \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (size_t i = 0; i < n; i++) {                                                   \
              *(tDType*)p1 = m_mod(*(tDType*)p1, *(tDType*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (size_t i = 0; i < n; i++) {                                                   \
              *(tDType*)p3 = m_mod(*(tDType*)p1, *(tDType*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_MOD_FALLBACK_LOOP(tDType)                                                             \
  }                                                                                            \
                                                                                               \
  DEF_MOD_SELF_FUNC(tDType, tNAryClass)                                                        \
  DEF_MOD_FUNC(tDType, tNAryClass)

#define DEF_NARRAY_INT_MOD_METHOD_FUNC(tDType, tNAryClass)                                     \
  static void iter_##tDType##_mod(na_loop_t* const lp) {                                       \
    ITER_MOD_INIT_VARS()                                                                       \
                                                                                               \
    if (is_aligned(p1, sizeof(tDType)) && is_aligned(p2, sizeof(tDType)) &&                    \
        is_aligned(p3, sizeof(tDType))) {                                                      \
      if (s1 == sizeof(tDType) && s2 == sizeof(tDType) && s3 == sizeof(tDType)) {              \
        if (p1 == p3) {                                                                        \
          for (size_t i = 0; i < n; i++) {                                                     \
            if ((((tDType*)p2)[i]) == 0) {                                                     \
              lp->err_type = rb_eZeroDivError;                                                 \
              return;                                                                          \
            }                                                                                  \
            ((tDType*)p1)[i] = m_mod(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
          }                                                                                    \
        } else {                                                                               \
          for (size_t i = 0; i < n; i++) {                                                     \
            if ((((tDType*)p2)[i]) == 0) {                                                     \
              lp->err_type = rb_eZeroDivError;                                                 \
              return;                                                                          \
            }                                                                                  \
            ((tDType*)p3)[i] = m_mod(((tDType*)p1)[i], ((tDType*)p2)[i]);                      \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
      if (is_aligned_step(s1, sizeof(tDType)) && is_aligned_step(s2, sizeof(tDType)) &&        \
          is_aligned_step(s3, sizeof(tDType))) {                                               \
        if (s2 == 0) {                                                                         \
          if ((*(tDType*)p2) == 0) {                                                           \
            lp->err_type = rb_eZeroDivError;                                                   \
            return;                                                                            \
          }                                                                                    \
          if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                  \
            ITER_MOD_INPLACE_OR_NEW_P2_SCL(tDType)                                             \
          } else {                                                                             \
            ITER_MOD_NEW_PTR_P2_SCL(tDType)                                                    \
          }                                                                                    \
        } else {                                                                               \
          if (p1 == p3) {                                                                      \
            for (size_t i = 0; i < n; i++) {                                                   \
              if ((*(tDType*)p2) == 0) {                                                       \
                lp->err_type = rb_eZeroDivError;                                               \
                return;                                                                        \
              }                                                                                \
              *(tDType*)p1 = m_mod(*(tDType*)p1, *(tDType*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
            }                                                                                  \
          } else {                                                                             \
            for (size_t i = 0; i < n; i++) {                                                   \
              if ((*(tDType*)p2) == 0) {                                                       \
                lp->err_type = rb_eZeroDivError;                                               \
                return;                                                                        \
              }                                                                                \
              *(tDType*)p3 = m_mod(*(tDType*)p1, *(tDType*)p2);                                \
              p1 += s1;                                                                        \
              p2 += s2;                                                                        \
              p3 += s3;                                                                        \
            }                                                                                  \
          }                                                                                    \
        }                                                                                      \
        return;                                                                                \
      }                                                                                        \
    }                                                                                          \
    ITER_MOD_FALLBACK_LOOP(tDType)                                                             \
  }                                                                                            \
                                                                                               \
  DEF_MOD_SELF_FUNC(tDType, tNAryClass)                                                        \
  DEF_MOD_FUNC(tDType, tNAryClass)

#define DEF_NARRAY_INT8_MOD_METHOD_FUNC(tDType, tNAryClass)                                    \
  static void iter_##tDType##_mod(na_loop_t* const lp) {                                       \
    ITER_MOD_INIT_VARS()                                                                       \
                                                                                               \
    if (s2 == 0) {                                                                             \
      if ((*(tDType*)p2) == 0) {                                                               \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      if (s1 == sizeof(tDType) && s3 == sizeof(tDType)) {                                      \
        ITER_MOD_INPLACE_OR_NEW_P2_SCL(tDType)                                                 \
      } else {                                                                                 \
        ITER_MOD_NEW_PTR_P2_SCL(tDType)                                                        \
      }                                                                                        \
    } else {                                                                                   \
      if (p1 == p3) {                                                                          \
        for (size_t i = 0; i < n; i++) {                                                       \
          if ((*(tDType*)p2) == 0) {                                                           \
            lp->err_type = rb_eZeroDivError;                                                   \
            return;                                                                            \
          }                                                                                    \
          *(tDType*)p1 = m_mod(*(tDType*)p1, *(tDType*)p2);                                    \
          p1 += s1;                                                                            \
          p2 += s2;                                                                            \
        }                                                                                      \
      } else {                                                                                 \
        for (size_t i = 0; i < n; i++) {                                                       \
          if ((*(tDType*)p2) == 0) {                                                           \
            lp->err_type = rb_eZeroDivError;                                                   \
            return;                                                                            \
          }                                                                                    \
          *(tDType*)p3 = m_mod(*(tDType*)p1, *(tDType*)p2);                                    \
          p1 += s1;                                                                            \
          p2 += s2;                                                                            \
          p3 += s3;                                                                            \
        }                                                                                      \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_MOD_SELF_FUNC(tDType, tNAryClass)                                                        \
  DEF_MOD_FUNC(tDType, tNAryClass)

#define DEF_NARRAY_ROBJ_MOD_METHOD_FUNC()                                                      \
  static void iter_robject_mod(na_loop_t* const lp) {                                          \
    ITER_MOD_INIT_VARS()                                                                       \
                                                                                               \
    if (s2 == 0) {                                                                             \
      if ((*(robject*)p2) == 0) {                                                              \
        lp->err_type = rb_eZeroDivError;                                                       \
        return;                                                                                \
      }                                                                                        \
      if (s1 == sizeof(robject) && s3 == sizeof(robject)) {                                    \
        ITER_MOD_INPLACE_OR_NEW_P2_SCL(robject)                                                \
      } else {                                                                                 \
        ITER_MOD_NEW_PTR_P2_SCL(robject)                                                       \
      }                                                                                        \
    } else {                                                                                   \
      if (p1 == p3) {                                                                          \
        for (size_t i = 0; i < n; i++) {                                                       \
          if ((*(robject*)p2) == 0) {                                                          \
            lp->err_type = rb_eZeroDivError;                                                   \
            return;                                                                            \
          }                                                                                    \
          *(robject*)p1 = m_mod(*(robject*)p1, *(robject*)p2);                                 \
          p1 += s1;                                                                            \
          p2 += s2;                                                                            \
        }                                                                                      \
      } else {                                                                                 \
        for (size_t i = 0; i < n; i++) {                                                       \
          if ((*(robject*)p2) == 0) {                                                          \
            lp->err_type = rb_eZeroDivError;                                                   \
            return;                                                                            \
          }                                                                                    \
          *(robject*)p3 = m_mod(*(robject*)p1, *(robject*)p2);                                 \
          p1 += s1;                                                                            \
          p2 += s2;                                                                            \
          p3 += s3;                                                                            \
        }                                                                                      \
      }                                                                                        \
    }                                                                                          \
  }                                                                                            \
                                                                                               \
  DEF_MOD_SELF_FUNC(robject, numo_cRObject)                                                    \
  static VALUE robject_mod(VALUE self, VALUE other) {                                          \
    return robject_mod_self(self, other);                                                      \
  }

#endif /* NUMO_NARRAY_MH_OP_MOD_H */
