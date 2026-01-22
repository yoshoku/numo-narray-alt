/*
  t_robject.c
  Ruby/Numo::NArray - Numerical Array class for Ruby

  created on: 2017-03-11
  Copyright (C) 2017-2020 Masahiro Tanaka
*/
#include <assert.h>
#include <ruby.h>

#include "SFMT.h"
#include "numo/narray.h"
#include "numo/template.h"

#define m_map(x) m_num_to_data(rb_yield(m_data_to_num(x)))

#ifdef __SSE2__
#include <emmintrin.h>
#define SIMD_ALIGNMENT_SIZE 16
#endif

static ID id_ne;
static ID id_pow;
static ID id_minus;
static ID id_lt;
static ID id_left_shift;
static ID id_le;
static ID id_ufo;
static ID id_eq;
static ID id_gt;
static ID id_ge;
static ID id_right_shift;
static ID id_abs;
static ID id_bit_and;
static ID id_bit_not;
static ID id_bit_or;
static ID id_bit_xor;
static ID id_cast;
static ID id_ceil;
static ID id_copysign;
static ID id_divmod;
static ID id_finite_p;
static ID id_floor;
static ID id_infinite_p;
static ID id_mulsum;
static ID id_nan;
static ID id_nan_p;
static ID id_nearly_eq;
static ID id_reciprocal;
static ID id_round;
static ID id_square;
static ID id_to_a;
static ID id_truncate;

#include <numo/types/robject.h>

/*
  class definition: Numo::RObject
*/
VALUE cT;
extern VALUE cRT;

#include "mh/coerce_cast.h"
#include "mh/to_a.h"
#include "mh/fill.h"
#include "mh/format.h"
#include "mh/format_to_a.h"
#include "mh/inspect.h"
#include "mh/op/add.h"
#include "mh/op/sub.h"
#include "mh/op/mul.h"
#include "mh/op/div.h"
#include "mh/op/mod.h"
#include "mh/divmod.h"
#include "mh/pow.h"
#include "mh/round/floor.h"
#include "mh/round/round.h"
#include "mh/round/ceil.h"
#include "mh/round/trunc.h"
#include "mh/comp/eq.h"
#include "mh/comp/ne.h"
#include "mh/comp/nearly_eq.h"
#include "mh/comp/gt.h"
#include "mh/comp/ge.h"
#include "mh/comp/lt.h"
#include "mh/comp/le.h"
#include "mh/bit/and.h"
#include "mh/bit/or.h"
#include "mh/bit/xor.h"
#include "mh/bit/not.h"
#include "mh/bit/left_shift.h"
#include "mh/bit/right_shift.h"
#include "mh/clip.h"
#include "mh/isnan.h"
#include "mh/isinf.h"
#include "mh/isposinf.h"
#include "mh/isneginf.h"
#include "mh/isfinite.h"
#include "mh/sum.h"
#include "mh/prod.h"
#include "mh/mean.h"
#include "mh/var.h"
#include "mh/stddev.h"
#include "mh/rms.h"
#include "mh/min.h"
#include "mh/max.h"
#include "mh/ptp.h"
#include "mh/max_index.h"
#include "mh/min_index.h"
#include "mh/argmax.h"
#include "mh/argmin.h"
#include "mh/maximum.h"
#include "mh/minimum.h"
#include "mh/minmax.h"
#include "mh/cumsum.h"
#include "mh/cumprod.h"
#include "mh/mulsum.h"
#include "mh/seq.h"
#include "mh/logseq.h"
#include "mh/eye.h"
#include "mh/rand.h"

typedef VALUE robject; // Type aliases for shorter notation
                       // following the codebase naming convention.
DEF_NARRAY_COERCE_CAST_METHOD_FUNC(robject)
DEF_NARRAY_TO_A_METHOD_FUNC(robject)
DEF_NARRAY_FILL_METHOD_FUNC(robject)
DEF_NARRAY_FORMAT_METHOD_FUNC(robject)
DEF_NARRAY_FORMAT_TO_A_METHOD_FUNC(robject)
DEF_NARRAY_ROBJ_INSPECT_METHOD_FUNC()
DEF_NARRAY_ROBJ_ADD_METHOD_FUNC()
DEF_NARRAY_ROBJ_SUB_METHOD_FUNC()
DEF_NARRAY_ROBJ_MUL_METHOD_FUNC()
DEF_NARRAY_ROBJ_DIV_METHOD_FUNC()
DEF_NARRAY_ROBJ_MOD_METHOD_FUNC()
DEF_NARRAY_ROBJ_DIVMOD_METHOD_FUNC()
DEF_NARRAY_ROBJ_POW_METHOD_FUNC()
DEF_NARRAY_ROBJ_FLOOR_METHOD_FUNC()
DEF_NARRAY_ROBJ_ROUND_METHOD_FUNC()
DEF_NARRAY_ROBJ_CEIL_METHOD_FUNC()
DEF_NARRAY_ROBJ_TRUNC_METHOD_FUNC()
DEF_NARRAY_ROBJ_EQ_METHOD_FUNC()
DEF_NARRAY_ROBJ_NE_METHOD_FUNC()
DEF_NARRAY_ROBJ_NEARLY_EQ_METHOD_FUNC()
DEF_NARRAY_ROBJ_GT_METHOD_FUNC()
DEF_NARRAY_ROBJ_GE_METHOD_FUNC()
DEF_NARRAY_ROBJ_LT_METHOD_FUNC()
DEF_NARRAY_ROBJ_LE_METHOD_FUNC()
DEF_NARRAY_ROBJ_BIT_AND_METHOD_FUNC()
DEF_NARRAY_ROBJ_BIT_OR_METHOD_FUNC()
DEF_NARRAY_ROBJ_BIT_XOR_METHOD_FUNC()
DEF_NARRAY_ROBJ_BIT_NOT_METHOD_FUNC()
DEF_NARRAY_ROBJ_LEFT_SHIFT_METHOD_FUNC()
DEF_NARRAY_ROBJ_RIGHT_SHIFT_METHOD_FUNC()
DEF_NARRAY_CLIP_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_ISNAN_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_ISINF_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_ISPOSINF_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_ISNEGINF_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_ISFINITE_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_SUM_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_PROD_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MEAN_METHOD_FUNC(robject, numo_cRObject, VALUE, numo_cRObject)
DEF_NARRAY_FLT_VAR_METHOD_FUNC(robject, numo_cRObject, VALUE, numo_cRObject)
DEF_NARRAY_FLT_STDDEV_METHOD_FUNC(robject, numo_cRObject, VALUE, numo_cRObject)
DEF_NARRAY_FLT_RMS_METHOD_FUNC(robject, numo_cRObject, VALUE, numo_cRObject)
DEF_NARRAY_FLT_MIN_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MAX_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_PTP_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MAX_INDEX_METHOD_FUNC(robject)
DEF_NARRAY_FLT_MIN_INDEX_METHOD_FUNC(robject)
DEF_NARRAY_FLT_ARGMAX_METHOD_FUNC(robject)
DEF_NARRAY_FLT_ARGMIN_METHOD_FUNC(robject)
DEF_NARRAY_FLT_MAXIMUM_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MINIMUM_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MINMAX_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_CUMSUM_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_CUMPROD_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_MULSUM_METHOD_FUNC(robject, numo_cRObject)
DEF_NARRAY_FLT_SEQ_METHOD_FUNC(robject)
DEF_NARRAY_FLT_LOGSEQ_METHOD_FUNC(robject)
DEF_NARRAY_EYE_METHOD_FUNC(robject)
DEF_NARRAY_FLT_RAND_METHOD_FUNC(robject)

static VALUE robject_store(VALUE, VALUE);

static size_t robject_memsize(const void* ptr) {
  size_t size = sizeof(narray_data_t);
  const narray_data_t* na = (const narray_data_t*)ptr;

  assert(na->base.type == NARRAY_DATA_T);

  if (na->ptr != NULL) {

    size += na->base.size * sizeof(dtype);
  }
  if (na->base.size > 0) {
    if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
      size += sizeof(size_t) * na->base.ndim;
    }
  }
  return size;
}

static void robject_free(void* ptr) {
  narray_data_t* na = (narray_data_t*)ptr;

  assert(na->base.type == NARRAY_DATA_T);

  if (na->ptr != NULL) {
    if (na->owned) {
      xfree(na->ptr);
    }
    na->ptr = NULL;
  }
  if (na->base.size > 0) {
    if (na->base.shape != NULL && na->base.shape != &(na->base.size)) {
      xfree(na->base.shape);
      na->base.shape = NULL;
    }
  }
  xfree(na);
}

static narray_type_info_t robject_info = {

  0,             // element_bits
  sizeof(dtype), // element_bytes
  sizeof(dtype), // element_stride (in bytes)

};

static void robject_gc_mark(void* ptr) {
  size_t n, i;
  VALUE* a;
  narray_data_t* na = ptr;

  if (na->ptr) {
    a = (VALUE*)(na->ptr);
    n = na->base.size;
    for (i = 0; i < n; i++) {
      rb_gc_mark(a[i]);
    }
  }
}

static const rb_data_type_t robject_data_type = {
  "Numo::RObject",
  {
    robject_gc_mark,
    robject_free,
    robject_memsize,
  },
  &na_data_type,
  &robject_info,
  0, // flags
};

static VALUE robject_s_alloc_func(VALUE klass) {
  narray_data_t* na = ALLOC(narray_data_t);

  na->base.ndim = 0;
  na->base.type = NARRAY_DATA_T;
  na->base.flag[0] = NA_FL0_INIT;
  na->base.flag[1] = NA_FL1_INIT;
  na->base.size = 0;
  na->base.shape = NULL;
  na->base.reduce = INT2FIX(0);
  na->ptr = NULL;
  na->owned = FALSE;
  return TypedData_Wrap_Struct(klass, &robject_data_type, (void*)na);
}

static VALUE robject_allocate(VALUE self) {
  narray_t* na;
  char* ptr;

  GetNArray(self, na);

  switch (NA_TYPE(na)) {
  case NARRAY_DATA_T:
    ptr = NA_DATA_PTR(na);
    if (na->size > 0 && ptr == NULL) {
      ptr = xmalloc(sizeof(dtype) * na->size);

      {
        size_t i;
        VALUE* a = (VALUE*)ptr;
        for (i = na->size; i--;) {
          *a++ = Qnil;
        }
      }

      NA_DATA_PTR(na) = ptr;
      NA_DATA_OWNED(na) = TRUE;
    }
    break;
  case NARRAY_VIEW_T:
    rb_funcall(NA_VIEW_DATA(na), rb_intern("allocate"), 0);
    break;
  case NARRAY_FILEMAP_T:
    // ptr = ((narray_filemap_t*)na)->ptr;
    //  to be implemented
  default:
    rb_bug("invalid narray type : %d", NA_TYPE(na));
  }
  return self;
}

/*
  Extract an element only if self is a dimensionless NArray.
  @overload extract
    @return [Numeric,Numo::NArray]
    --- Extract element value as Ruby Object if self is a dimensionless NArray,
    otherwise returns self.
*/
static VALUE robject_extract(VALUE self) {
  volatile VALUE v;
  char* ptr;
  narray_t* na;
  GetNArray(self, na);

  if (na->ndim == 0) {
    ptr = na_get_pointer_for_read(self) + na_get_offset(self);
    v = m_extract(ptr);
    na_release_lock(self);
    return v;
  }
  return self;
}

static VALUE robject_new_dim0(dtype x) {
  VALUE v;
  dtype* ptr;

  v = nary_new(cT, 0, NULL);
  ptr = (dtype*)(char*)na_get_pointer_for_write(v);
  *ptr = x;
  na_release_lock(v);
  return v;
}

static VALUE robject_store_numeric(VALUE self, VALUE obj) {
  dtype x;
  x = m_num_to_data(obj);
  obj = robject_new_dim0(x);
  robject_store(self, obj);
  return self;
}

static void iter_robject_store_bit(na_loop_t* const lp) {
  size_t i;
  char* p1;
  size_t p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  BIT_DIGIT *a2, x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_BIT_IDX(lp, 1, a2, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        LOAD_BIT(a2, p2 + *idx2, x);
        idx2++;
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        LOAD_BIT(a2, p2 + *idx2, x);
        idx2++;
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        LOAD_BIT(a2, p2, x);
        p2 += s2;
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        LOAD_BIT(a2, p2, x);
        p2 += s2;
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_bit(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_bit, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_dfloat(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  double x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, double, x);
        y = m_from_real(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, double, x);
        y = m_from_real(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, double, x);
        y = m_from_real(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, double, x);
        y = m_from_real(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_dfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_dfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_sfloat(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  float x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, float, x);
        y = m_from_real(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, float, x);
        y = m_from_real(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, float, x);
        y = m_from_real(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, float, x);
        y = m_from_real(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_sfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_sfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_int64(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  int64_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int64_t, x);
        y = m_from_int64(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int64_t, x);
        y = m_from_int64(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int64_t, x);
        y = m_from_int64(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int64_t, x);
        y = m_from_int64(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_int64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_int64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_int32(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  int32_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int32_t, x);
        y = m_from_int32(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int32_t, x);
        y = m_from_int32(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int32_t, x);
        y = m_from_int32(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int32_t, x);
        y = m_from_int32(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_int32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_int32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_int16(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  int16_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int16_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int16_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int16_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int16_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_int16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_int16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_int8(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  int8_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int8_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, int8_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int8_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, int8_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_int8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_int8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_uint64(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  u_int64_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int64_t, x);
        y = m_from_uint64(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int64_t, x);
        y = m_from_uint64(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int64_t, x);
        y = m_from_uint64(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int64_t, x);
        y = m_from_uint64(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_uint64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_uint64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_uint32(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  u_int32_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int32_t, x);
        y = m_from_uint32(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int32_t, x);
        y = m_from_uint32(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int32_t, x);
        y = m_from_uint32(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int32_t, x);
        y = m_from_uint32(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_uint32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_uint32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_uint16(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  u_int16_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int16_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int16_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int16_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int16_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_uint16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_uint16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_uint8(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  u_int8_t x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int8_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, u_int8_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int8_t, x);
        y = m_from_sint(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, u_int8_t, x);
        y = m_from_sint(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_uint8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_uint8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_robject(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  VALUE x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, VALUE, x);
        y = m_num_to_data(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, VALUE, x);
        y = m_num_to_data(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, VALUE, x);
        y = m_num_to_data(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, VALUE, x);
        y = m_num_to_data(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE robject_store_robject(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_store_robject, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_robject_store_array(na_loop_t* const lp) {
  size_t i, n;
  size_t i1, n1;
  VALUE v1, *ptr;
  char* p1;
  size_t s1, *idx1;
  VALUE x;
  double y;
  dtype z;
  size_t len, c;
  double beg, step;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  v1 = lp->args[1].value;
  i = 0;

  if (lp->args[1].ptr) {
    if (v1 == Qtrue) {
      iter_robject_store_robject(lp);
      i = lp->args[1].shape[0];
      if (idx1) {
        idx1 += i;
      } else {
        p1 += s1 * i;
      }
    }
    goto loop_end;
  }

  ptr = &v1;

  switch (TYPE(v1)) {
  case T_ARRAY:
    n1 = RARRAY_LEN(v1);
    ptr = RARRAY_PTR(v1);
    break;
  case T_NIL:
    n1 = 0;
    break;
  default:
    n1 = 1;
  }

  if (idx1) {
    for (i = i1 = 0; i1 < n1 && i < n; i++, i1++) {
      x = ptr[i1];
      if (rb_obj_is_kind_of(x, rb_cRange)
#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
          || rb_obj_is_kind_of(x, rb_cArithSeq)
#else
          || rb_obj_is_kind_of(x, rb_cEnumerator)
#endif
      ) {
        nary_step_sequence(x, &len, &beg, &step);
        for (c = 0; c < len && i < n; c++, i++) {
          y = beg + step * c;
          z = m_from_double(y);
          SET_DATA_INDEX(p1, idx1, dtype, z);
        }
      } else if (TYPE(x) != T_ARRAY) {
        z = m_num_to_data(x);
        SET_DATA_INDEX(p1, idx1, dtype, z);
      }
    }
  } else {
    for (i = i1 = 0; i1 < n1 && i < n; i++, i1++) {
      x = ptr[i1];
      if (rb_obj_is_kind_of(x, rb_cRange)
#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
          || rb_obj_is_kind_of(x, rb_cArithSeq)
#else
          || rb_obj_is_kind_of(x, rb_cEnumerator)
#endif
      ) {
        nary_step_sequence(x, &len, &beg, &step);
        for (c = 0; c < len && i < n; c++, i++) {
          y = beg + step * c;
          z = m_from_double(y);
          SET_DATA_STRIDE(p1, s1, dtype, z);
        }
      } else if (TYPE(x) != T_ARRAY) {
        z = m_num_to_data(x);
        SET_DATA_STRIDE(p1, s1, dtype, z);
      }
    }
  }

loop_end:
  z = m_zero;
  if (idx1) {
    for (; i < n; i++) {
      SET_DATA_INDEX(p1, idx1, dtype, z);
    }
  } else {
    for (; i < n; i++) {
      SET_DATA_STRIDE(p1, s1, dtype, z);
    }
  }
}

static VALUE robject_store_array(VALUE self, VALUE rary) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { rb_cArray, 0 } };
  ndfunc_t ndf = { iter_robject_store_array, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop_store_rarray(&ndf, self, rary);
  return self;
}

/*
  Store elements to Numo::RObject from other.
  @overload store(other)
    @param [Object] other
    @return [Numo::RObject] self
*/
static VALUE robject_store(VALUE self, VALUE obj) {
  VALUE r, klass;

  klass = rb_obj_class(obj);

  if (klass == numo_cRObject) {
    robject_store_robject(self, obj);
    return self;
  }

  if (IS_INTEGER_CLASS(klass) || klass == rb_cFloat || klass == rb_cComplex) {
    robject_store_numeric(self, obj);
    return self;
  }

  if (klass == numo_cBit) {
    robject_store_bit(self, obj);
    return self;
  }

  if (klass == numo_cDFloat) {
    robject_store_dfloat(self, obj);
    return self;
  }

  if (klass == numo_cSFloat) {
    robject_store_sfloat(self, obj);
    return self;
  }

  if (klass == numo_cInt64) {
    robject_store_int64(self, obj);
    return self;
  }

  if (klass == numo_cInt32) {
    robject_store_int32(self, obj);
    return self;
  }

  if (klass == numo_cInt16) {
    robject_store_int16(self, obj);
    return self;
  }

  if (klass == numo_cInt8) {
    robject_store_int8(self, obj);
    return self;
  }

  if (klass == numo_cUInt64) {
    robject_store_uint64(self, obj);
    return self;
  }

  if (klass == numo_cUInt32) {
    robject_store_uint32(self, obj);
    return self;
  }

  if (klass == numo_cUInt16) {
    robject_store_uint16(self, obj);
    return self;
  }

  if (klass == numo_cUInt8) {
    robject_store_uint8(self, obj);
    return self;
  }

  if (klass == rb_cArray) {
    robject_store_array(self, obj);
    return self;
  }

  if (IsNArray(obj)) {
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      robject_store(self, r);
      return self;
    }
  }

  robject_store_numeric(self, obj);

  return self;
}

/*
  Convert a data value of obj (with a single element) to dtype.
*/
static dtype robject_extract_data(VALUE obj) {
  narray_t* na;
  dtype x;
  char* ptr;
  size_t pos;
  VALUE r, klass;

  if (IsNArray(obj)) {
    GetNArray(obj, na);
    if (na->size != 1) {
      rb_raise(nary_eShapeError, "narray size should be 1");
    }
    klass = rb_obj_class(obj);
    ptr = na_get_pointer_for_read(obj);
    pos = na_get_offset(obj);

    if (klass == numo_cRObject) {
      x = m_num_to_data(*(VALUE*)(ptr + pos));
      return x;
    }

    if (klass == numo_cBit) {
      {
        BIT_DIGIT b;
        LOAD_BIT(ptr, pos, b);
        x = m_from_sint(b);
      };
      return x;
    }

    if (klass == numo_cDFloat) {
      x = m_from_real(*(double*)(ptr + pos));
      return x;
    }

    if (klass == numo_cSFloat) {
      x = m_from_real(*(float*)(ptr + pos));
      return x;
    }

    if (klass == numo_cInt64) {
      x = m_from_int64(*(int64_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cInt32) {
      x = m_from_int32(*(int32_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cInt16) {
      x = m_from_sint(*(int16_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cInt8) {
      x = m_from_sint(*(int8_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cUInt64) {
      x = m_from_uint64(*(u_int64_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cUInt32) {
      x = m_from_uint32(*(u_int32_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cUInt16) {
      x = m_from_sint(*(u_int16_t*)(ptr + pos));
      return x;
    }

    if (klass == numo_cUInt8) {
      x = m_from_sint(*(u_int8_t*)(ptr + pos));
      return x;
    }

    // coerce
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      return robject_extract_data(r);
    }

    return obj;
  }
  if (TYPE(obj) == T_ARRAY) {
    if (RARRAY_LEN(obj) != 1) {
      rb_raise(nary_eShapeError, "array size should be 1");
    }
    return m_num_to_data(RARRAY_AREF(obj, 0));
  }
  return m_num_to_data(obj);
}

static VALUE robject_cast_array(VALUE rary) {
  VALUE nary;
  narray_t* na;

  nary = na_s_new_like(cT, rary);
  GetNArray(nary, na);
  if (na->size > 0) {
    robject_store_array(nary, rary);
  }
  return nary;
}

/*
  Cast object to Numo::RObject.
  @overload [](elements)
  @overload cast(array)
    @param [Numeric,Array] elements
    @param [Array] array
    @return [Numo::RObject]
*/
static VALUE robject_s_cast(VALUE type, VALUE obj) {
  VALUE v;
  narray_t* na;
  dtype x;

  if (rb_obj_class(obj) == cT) {
    return obj;
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cNumeric))) {
    x = m_num_to_data(obj);
    return robject_new_dim0(x);
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cArray))) {
    return robject_cast_array(obj);
  }
  if (IsNArray(obj)) {
    GetNArray(obj, na);
    v = nary_new(cT, NA_NDIM(na), NA_SHAPE(na));
    if (NA_SIZE(na) > 0) {
      robject_store(v, obj);
    }
    return v;
  }
  if (rb_respond_to(obj, id_to_a)) {
    obj = rb_funcall(obj, id_to_a, 0);
    if (TYPE(obj) != T_ARRAY) {
      rb_raise(rb_eTypeError, "`to_a' did not return Array");
    }
    return robject_cast_array(obj);
  }

  return robject_new_dim0(obj);
}

/*
  Multi-dimensional element reference.
  @overload [](dim0,...,dimL)
    @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol]
    dim0,...,dimL  multi-dimensional indices.
    @return [Numeric,Numo::RObject] an element or NArray view.
  @see Numo::NArray#[]
  @see #[]=
 */
static VALUE robject_aref(int argc, VALUE* argv, VALUE self) {
  int nd;
  size_t pos;
  char* ptr;

  nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
  if (nd) {
    return na_aref_main(argc, argv, self, 0, nd);
  } else {
    ptr = na_get_pointer_for_read(self) + pos;
    return m_extract(ptr);
  }
}

/*
  Multi-dimensional element assignment.
  @overload []=(dim0,...,dimL,val)
    @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol]
    dim0,...,dimL  multi-dimensional indices.
    @param [Numeric,Numo::NArray,Array] val  Value(s) to be set to self.
    @return [Numeric,Numo::NArray,Array] returns `val` (last argument).
  @see Numo::NArray#[]=
  @see #[]
*/
static VALUE robject_aset(int argc, VALUE* argv, VALUE self) {
  int nd;
  size_t pos;
  char* ptr;
  VALUE a;
  dtype x;

  argc--;
  if (argc == 0) {
    robject_store(self, argv[argc]);
  } else {
    nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
    if (nd) {
      a = na_aref_main(argc, argv, self, 0, nd);
      robject_store(a, argv[argc]);
    } else {
      x = robject_extract_data(argv[argc]);
      ptr = na_get_pointer_for_read_write(self) + pos;
      *(dtype*)ptr = x;
    }
  }
  return argv[argc];
}

static void iter_robject_each(na_loop_t* const lp) {
  size_t i, s1;
  char* p1;
  size_t* idx1;
  dtype x;
  VALUE y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  if (idx1) {
    for (; i--;) {
      GET_DATA_INDEX(p1, idx1, dtype, x);
      y = m_data_to_num(x);
      rb_yield(y);
    }
  } else {
    for (; i--;) {
      GET_DATA_STRIDE(p1, s1, dtype, x);
      y = m_data_to_num(x);
      rb_yield(y);
    }
  }
}

/*
  Calls the given block once for each element in self,
  passing that element as a parameter.
  @overload each
    @return [Numo::NArray] self
    For a block `{|x| ... }`,
    @yieldparam [Numeric] x  an element of NArray.
  @see #each_with_index
  @see #map
*/
static VALUE robject_each(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_each, FULL_LOOP_NIP, 1, 0, ain, 0 };

  na_ndloop(&ndf, 1, self);
  return self;
}

static void iter_robject_map(na_loop_t* const lp) {
  size_t i, n;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  if (idx1) {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_map(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_map(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_map(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      for (i = 0; i < n; i++) {
        *(dtype*)p2 = m_map(*(dtype*)p1);
        p1 += s1;
        p2 += s2;
      }
      return;
      //
    }
  }
}

/*
  Unary map.
  @overload map
    @return [Numo::RObject] map of self.
*/
static VALUE robject_map(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_map, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static inline void yield_each_with_index(dtype x, size_t* c, VALUE* a, int nd, int md) {
  int j;

  a[0] = m_data_to_num(x);
  for (j = 0; j <= nd; j++) {
    a[j + 1] = SIZET2NUM(c[j]);
  }
  rb_yield(rb_ary_new4(md, a));
}

static void iter_robject_each_with_index(na_loop_t* const lp) {
  size_t i, s1;
  char* p1;
  size_t* idx1;
  dtype x;
  VALUE* a;
  size_t* c;
  int nd, md;

  c = (size_t*)(lp->opt_ptr);
  nd = lp->ndim;
  if (nd > 0) {
    nd--;
  }
  md = nd + 2;
  a = ALLOCA_N(VALUE, md);

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  c[nd] = 0;
  if (idx1) {
    for (; i--;) {
      GET_DATA_INDEX(p1, idx1, dtype, x);
      yield_each_with_index(x, c, a, nd, md);
      c[nd]++;
    }
  } else {
    for (; i--;) {
      GET_DATA_STRIDE(p1, s1, dtype, x);
      yield_each_with_index(x, c, a, nd, md);
      c[nd]++;
    }
  }
}

/*
  Invokes the given block once for each element of self,
  passing that element and indices along each axis as parameters.
  @overload each_with_index
    For a block `{|x,i,j,...| ... }`,
    @yieldparam [Numeric] x  an element
    @yieldparam [Integer] i,j,...  multitimensional indices
    @return [Numo::NArray] self
  @see #each
  @see #map_with_index
*/
static VALUE robject_each_with_index(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_t ndf = { iter_robject_each_with_index, FULL_LOOP_NIP, 1, 0, ain, 0 };

  na_ndloop_with_index(&ndf, 1, self);
  return self;
}

static inline dtype yield_map_with_index(dtype x, size_t* c, VALUE* a, int nd, int md) {
  int j;
  VALUE y;

  a[0] = m_data_to_num(x);
  for (j = 0; j <= nd; j++) {
    a[j + 1] = SIZET2NUM(c[j]);
  }
  y = rb_yield(rb_ary_new4(md, a));
  return m_num_to_data(y);
}

static void iter_robject_map_with_index(na_loop_t* const lp) {
  size_t i;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;
  VALUE* a;
  size_t* c;
  int nd, md;

  c = (size_t*)(lp->opt_ptr);
  nd = lp->ndim;
  if (nd > 0) {
    nd--;
  }
  md = nd + 2;
  a = ALLOCA_N(VALUE, md);

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  c[nd] = 0;
  if (idx1) {
    if (idx2) {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = yield_map_with_index(x, c, a, nd, md);
        SET_DATA_INDEX(p2, idx2, dtype, x);
        c[nd]++;
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = yield_map_with_index(x, c, a, nd, md);
        SET_DATA_STRIDE(p2, s2, dtype, x);
        c[nd]++;
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = yield_map_with_index(x, c, a, nd, md);
        SET_DATA_INDEX(p2, idx2, dtype, x);
        c[nd]++;
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = yield_map_with_index(x, c, a, nd, md);
        SET_DATA_STRIDE(p2, s2, dtype, x);
        c[nd]++;
      }
    }
  }
}

/*
  Invokes the given block once for each element of self,
  passing that element and indices along each axis as parameters.
  Creates a new NArray containing the values returned by the block.
  Inplace option is allowed, i.e., `nary.inplace.map` overwrites `nary`.
  @overload map_with_index
    For a block `{|x,i,j,...| ... }`,
    @yieldparam [Numeric] x  an element
    @yieldparam [Integer] i,j,...  multitimensional indices
    @return [Numo::NArray] mapped array
  @see #map
  @see #each_with_index
*/
static VALUE robject_map_with_index(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_map_with_index, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop_with_index(&ndf, 1, self);
}

static void iter_robject_abs(na_loop_t* const lp) {
  size_t i;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;
  rtype y;
  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx1) {
    if (idx2) {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        y = m_abs(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        y = m_abs(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_abs(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_abs(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  }
}

/*
  abs of self.
  @overload abs
    @return [Numo::RObject] abs of self.
*/
static VALUE robject_abs(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cRT, 0 } };
  ndfunc_t ndf = { iter_robject_abs, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_robject_minus(na_loop_t* const lp) {
  size_t i, n;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  if (idx1) {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_minus(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_minus(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_minus(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      for (i = 0; i < n; i++) {
        *(dtype*)p2 = m_minus(*(dtype*)p1);
        p1 += s1;
        p2 += s2;
      }
      return;
      //
    }
  }
}

/*
  Unary minus.
  @overload -@
    @return [Numo::RObject] minus of self.
*/
static VALUE robject_minus(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_minus, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_robject_reciprocal(na_loop_t* const lp) {
  size_t i, n;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  if (idx1) {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_reciprocal(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_reciprocal(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_reciprocal(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      for (i = 0; i < n; i++) {
        *(dtype*)p2 = m_reciprocal(*(dtype*)p1);
        p1 += s1;
        p2 += s2;
      }
      return;
      //
    }
  }
}

/*
  Unary reciprocal.
  @overload reciprocal
    @return [Numo::RObject] reciprocal of self.
*/
static VALUE robject_reciprocal(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_reciprocal, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_robject_sign(na_loop_t* const lp) {
  size_t i, n;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  if (idx1) {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_sign(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_sign(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_sign(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      for (i = 0; i < n; i++) {
        *(dtype*)p2 = m_sign(*(dtype*)p1);
        p1 += s1;
        p2 += s2;
      }
      return;
      //
    }
  }
}

/*
  Unary sign.
  @overload sign
    @return [Numo::RObject] sign of self.
*/
static VALUE robject_sign(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_sign, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_robject_square(na_loop_t* const lp) {
  size_t i, n;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t *idx1, *idx2;
  dtype x;

  INIT_COUNTER(lp, n);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);

  if (idx1) {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_square(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_square(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_square(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      for (i = 0; i < n; i++) {
        *(dtype*)p2 = m_square(*(dtype*)p1);
        p1 += s1;
        p2 += s2;
      }
      return;
      //
    }
  }
}

/*
  Unary square.
  @overload square
    @return [Numo::RObject] square of self.
*/
static VALUE robject_square(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_square, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_robject_poly(na_loop_t* const lp) {
  size_t i;
  dtype x, y, a;

  x = *(dtype*)(lp->args[0].ptr + lp->args[0].iter[0].pos);
  i = lp->narg - 2;
  y = *(dtype*)(lp->args[i].ptr + lp->args[i].iter[0].pos);
  for (; --i;) {
    y = m_mul(x, y);
    a = *(dtype*)(lp->args[i].ptr + lp->args[i].iter[0].pos);
    y = m_add(y, a);
  }
  i = lp->narg - 1;
  *(dtype*)(lp->args[i].ptr + lp->args[i].iter[0].pos) = y;
}

/*
  Calculate polynomial.
    `x.poly(a0,a1,a2,...,an) = a0 + a1*x + a2*x**2 + ... + an*x**n`
  @overload poly a0, a1, ..., an
    @param [Numo::NArray,Numeric] a0,a1,...,an
    @return [Numo::RObject]
*/
static VALUE robject_poly(VALUE self, VALUE args) {
  int argc, i;
  VALUE* argv;
  volatile VALUE v, a;
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_robject_poly, NO_LOOP, 0, 1, 0, aout };

  argc = (int)RARRAY_LEN(args);
  ndf.nin = argc + 1;
  ndf.ain = ALLOCA_N(ndfunc_arg_in_t, argc + 1);
  for (i = 0; i < argc + 1; i++) {
    ndf.ain[i].type = cT;
  }
  argv = ALLOCA_N(VALUE, argc + 1);
  argv[0] = self;
  for (i = 0; i < argc; i++) {
    argv[i + 1] = RARRAY_PTR(args)[i];
  }
  a = rb_ary_new4(argc + 1, argv);
  v = na_ndloop2(&ndf, a);
  return robject_extract(v);
}

void Init_numo_robject(void) {
  VALUE hCast, mNumo;

  mNumo = rb_define_module("Numo");

  id_ne = rb_intern("!=");
  id_pow = rb_intern("**");
  id_minus = rb_intern("-@");
  id_lt = rb_intern("<");
  id_left_shift = rb_intern("<<");
  id_le = rb_intern("<=");
  id_ufo = rb_intern("<=>");
  id_eq = rb_intern("==");
  id_gt = rb_intern(">");
  id_ge = rb_intern(">=");
  id_right_shift = rb_intern(">>");
  id_abs = rb_intern("abs");
  id_bit_and = rb_intern("bit_and");
  id_bit_not = rb_intern("bit_not");
  id_bit_or = rb_intern("bit_or");
  id_bit_xor = rb_intern("bit_xor");
  id_cast = rb_intern("cast");
  id_ceil = rb_intern("ceil");
  id_copysign = rb_intern("copysign");
  id_divmod = rb_intern("divmod");
  id_finite_p = rb_intern("finite?");
  id_floor = rb_intern("floor");
  id_infinite_p = rb_intern("infinite?");
  id_mulsum = rb_intern("mulsum");
  id_nan = rb_intern("nan");
  id_nan_p = rb_intern("nan?");
  id_nearly_eq = rb_intern("nearly_eq");
  id_reciprocal = rb_intern("reciprocal");
  id_round = rb_intern("round");
  id_square = rb_intern("square");
  id_to_a = rb_intern("to_a");
  id_truncate = rb_intern("truncate");

  /**
   * Document-class: Numo::RObject
   *
   * Ruby object N-dimensional array class.
   */
  cT = rb_define_class_under(mNumo, "RObject", cNArray);

  hCast = rb_hash_new();
  /* Upcasting rules of RObject. */
  rb_define_const(cT, "UPCAST", hCast);
  rb_hash_aset(hCast, rb_cArray, cT);

#ifdef RUBY_INTEGER_UNIFICATION
  rb_hash_aset(hCast, rb_cInteger, cT);
#else
  rb_hash_aset(hCast, rb_cFixnum, cT);
  rb_hash_aset(hCast, rb_cBignum, cT);
#endif
  rb_hash_aset(hCast, rb_cFloat, cT);
  rb_hash_aset(hCast, rb_cComplex, cT);
  rb_hash_aset(hCast, numo_cDComplex, numo_cRObject);
  rb_hash_aset(hCast, numo_cSComplex, numo_cRObject);
  rb_hash_aset(hCast, numo_cDFloat, numo_cRObject);
  rb_hash_aset(hCast, numo_cSFloat, numo_cRObject);
  rb_hash_aset(hCast, numo_cInt64, numo_cRObject);
  rb_hash_aset(hCast, numo_cInt32, numo_cRObject);
  rb_hash_aset(hCast, numo_cInt16, numo_cRObject);
  rb_hash_aset(hCast, numo_cInt8, numo_cRObject);
  rb_hash_aset(hCast, numo_cUInt64, numo_cRObject);
  rb_hash_aset(hCast, numo_cUInt32, numo_cRObject);
  rb_hash_aset(hCast, numo_cUInt16, numo_cRObject);
  rb_hash_aset(hCast, numo_cUInt8, numo_cRObject);
  rb_obj_freeze(hCast);

  /* Element size of RObject in bits. */
  rb_define_const(cT, "ELEMENT_BIT_SIZE", INT2FIX(sizeof(dtype) * 8));
  /* Element size of RObject in bytes. */
  rb_define_const(cT, "ELEMENT_BYTE_SIZE", INT2FIX(sizeof(dtype)));
  /* Stride size of contiguous RObject array. */
  rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(sizeof(dtype)));
  rb_undef_method(rb_singleton_class(cT), "from_binary");
  rb_undef_method(cT, "to_binary");
  rb_undef_method(cT, "swap_byte");
  rb_undef_method(cT, "to_network");
  rb_undef_method(cT, "to_vacs");
  rb_undef_method(cT, "to_host");
  rb_undef_method(cT, "to_swapped");
  rb_define_alloc_func(cT, robject_s_alloc_func);
  rb_define_method(cT, "allocate", robject_allocate, 0);
  rb_define_method(cT, "extract", robject_extract, 0);

  rb_define_method(cT, "store", robject_store, 1);

  rb_define_singleton_method(cT, "cast", robject_s_cast, 1);
  rb_define_method(cT, "[]", robject_aref, -1);
  rb_define_method(cT, "[]=", robject_aset, -1);
  /**
   * return NArray with cast to the type of self.
   * @overload coerce_cast(type)
   *   @return [nil]
   */
  rb_define_method(cT, "coerce_cast", robject_coerce_cast, 1);
  /**
   * Convert self to Array.
   * @overload to_a
   *   @return [Array]
   */
  rb_define_method(cT, "to_a", robject_to_a, 0);
  /**
   * Fill elements with other.
   * @overload fill other
   *   @param [Numeric] other
   *   @return [Numo::RObject] self.
   */
  rb_define_method(cT, "fill", robject_fill, 1);
  /**
   * Format elements into strings.
   * @overload format format
   *   @param [String] format
   *   @return [Numo::RObject] array of formatted strings.
   */
  rb_define_method(cT, "format", robject_format, -1);
  /**
   * Format elements into strings.
   * @overload format_to_a format
   *   @param [String] format
   *   @return [Array] array of formatted strings.
   */
  rb_define_method(cT, "format_to_a", robject_format_to_a, -1);
  /**
   * Returns a string containing a human-readable representation of NArray.
   * @overload inspect
   *   @return [String]
   */
  rb_define_method(cT, "inspect", robject_inspect, 0);
  rb_define_method(cT, "each", robject_each, 0);
  rb_define_method(cT, "map", robject_map, 0);
  rb_define_method(cT, "each_with_index", robject_each_with_index, 0);
  rb_define_method(cT, "map_with_index", robject_map_with_index, 0);
  rb_define_method(cT, "abs", robject_abs, 0);
  /**
   * Binary add.
   * @overload + other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self + other
   */
  rb_define_method(cT, "+", robject_add, 1);
  /**
   * Binary sub.
   * @overload - other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self - other
   */
  rb_define_method(cT, "-", robject_sub, 1);
  /**
   * Binary mul.
   * @overload * other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self * other
   */
  rb_define_method(cT, "*", robject_mul, 1);
  /**
   * Binary div.
   * @overload / other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self / other
   */
  rb_define_method(cT, "/", robject_div, 1);
  /**
   * Binary mod.
   * @overload % other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self % other
   */
  rb_define_method(cT, "%", robject_mod, 1);
  /**
   * Binary divmod.
   * @overload divmod other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] divmod of self and other.
   */
  rb_define_method(cT, "divmod", robject_divmod, 1);
  /**
   * Binary power.
   * @overload ** other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self to the other-th power.
   */
  rb_define_method(cT, "**", robject_pow, 1);
  rb_define_alias(cT, "pow", "**");
  rb_define_method(cT, "-@", robject_minus, 0);
  rb_define_method(cT, "reciprocal", robject_reciprocal, 0);
  rb_define_method(cT, "sign", robject_sign, 0);
  rb_define_method(cT, "square", robject_square, 0);
  rb_define_alias(cT, "conj", "view");
  rb_define_alias(cT, "im", "view");
  rb_define_alias(cT, "conjugate", "conj");
  /**
   * Comparison eq other.
   * @overload eq other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self eq other.
   */
  rb_define_method(cT, "eq", robject_eq, 1);
  /**
   * Comparison ne other.
   * @overload ne other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self ne other.
   */
  rb_define_method(cT, "ne", robject_ne, 1);
  /**
   * Comparison nearly_eq other.
   * @overload nearly_eq other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self nearly_eq other.
   */
  rb_define_method(cT, "nearly_eq", robject_nearly_eq, 1);
  rb_define_alias(cT, "close_to", "nearly_eq");
  /**
   * Binary bit_and.
   * @overload & other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self & other
   */
  rb_define_method(cT, "&", robject_bit_and, 1);
  /**
   * Binary bit_or.
   * @overload | other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self | other
   */
  rb_define_method(cT, "|", robject_bit_or, 1);
  /**
   * Binary bit_xor.
   * @overload ^ other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self ^ other
   */
  rb_define_method(cT, "^", robject_bit_xor, 1);
  /**
   * Unary bit_not.
   * @overload ~
   *   @return [Numo::RObject] bit_not of self.
   */
  rb_define_method(cT, "~", robject_bit_not, 0);
  /**
   * Binary left_shift.
   * @overload << other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self << other
   */
  rb_define_method(cT, "<<", robject_left_shift, 1);
  /**
   * Binary right_shift.
   * @overload >> other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self >> other
   */
  rb_define_method(cT, ">>", robject_right_shift, 1);
  /**
   * Unary floor.
   * @overload floor
   *   @return [Numo::RObject] floor of self.
   */
  rb_define_method(cT, "floor", robject_floor, 0);
  /**
   * Unary round.
   * @overload round
   *   @return [Numo::RObject] round of self.
   */
  rb_define_method(cT, "round", robject_round, 0);
  /**
   * Unary ceil.
   * @overload ceil
   *   @return [Numo::RObject] ceil of self.
   */
  rb_define_method(cT, "ceil", robject_ceil, 0);
  /**
   * Unary trunc.
   * @overload trunc
   *   @return [Numo::RObject] trunc of self.
   */
  rb_define_method(cT, "trunc", robject_trunc, 0);
  /**
   * Comparison gt other.
   * @overload gt other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self gt other.
   */
  rb_define_method(cT, "gt", robject_gt, 1);
  /**
   * Comparison ge other.
   * @overload ge other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self ge other.
   */
  rb_define_method(cT, "ge", robject_ge, 1);
  /**
   * Comparison lt other.
   * @overload lt other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self lt other.
   */
  rb_define_method(cT, "lt", robject_lt, 1);
  /**
   * Comparison le other.
   * @overload le other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self le other.
   */
  rb_define_method(cT, "le", robject_le, 1);
  rb_define_alias(cT, ">", "gt");
  rb_define_alias(cT, ">=", "ge");
  rb_define_alias(cT, "<", "lt");
  rb_define_alias(cT, "<=", "le");
  /**
   * Clip array elements by [min,max].
   * If either of min or max is nil, one side is clipped.
   * @overload clip(min,max)
   *   @param [Numo::NArray,Numeric] min
   *   @param [Numo::NArray,Numeric] max
   *   @return [Numo::NArray] result of clip.
   *
   * @example
   *     a = Numo::Int32.new(10).seq
   *     # => Numo::Int32#shape=[10]
   *     # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
   *
   *     a.clip(1,8)
   *     # => Numo::Int32#shape=[10]
   *     # [1, 1, 2, 3, 4, 5, 6, 7, 8, 8]
   *
   *     a.inplace.clip(3,6)
   *     a
   *     # => Numo::Int32#shape=[10]
   *     # [3, 3, 3, 3, 4, 5, 6, 6, 6, 6]
   *
   *     b = Numo::Int32.new(10).seq
   *     # => Numo::Int32#shape=[10]
   *     # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
   *
   *     b.clip([3,4,1,1,1,4,4,4,4,4], 8)
   *     # => Numo::Int32#shape=[10]
   *     # [3, 4, 2, 3, 4, 5, 6, 7, 8, 8]
   */
  rb_define_method(cT, "clip", robject_clip, 2);
  /**
   * Condition of isnan.
   * @overload isnan
   *   @return [Numo::Bit] Condition of isnan.
   */
  rb_define_method(cT, "isnan", robject_isnan, 0);
  /**
   * Condition of isinf.
   * @overload isinf
   *   @return [Numo::Bit] Condition of isinf.
   */
  rb_define_method(cT, "isinf", robject_isinf, 0);
  /**
   * Condition of isposinf.
   * @overload isposinf
   *   @return [Numo::Bit] Condition of isposinf.
   */
  rb_define_method(cT, "isposinf", robject_isposinf, 0);
  /**
   * Condition of isneginf.
   * @overload isneginf
   *   @return [Numo::Bit] Condition of isneginf.
   */
  rb_define_method(cT, "isneginf", robject_isneginf, 0);
  /**
   * Condition of isfinite.
   * @overload isfinite
   *   @return [Numo::Bit] Condition of isfinite.
   */
  rb_define_method(cT, "isfinite", robject_isfinite, 0);
  /**
   * sum of self.
   * @overload sum(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs sum along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::RObject] returns result of sum.
   */
  rb_define_method(cT, "sum", robject_sum, -1);
  /**
   * prod of self.
   * @overload prod(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs prod along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::RObject] returns result of prod.
   */
  rb_define_method(cT, "prod", robject_prod, -1);
  /**
   * mean of self.
   * @overload mean(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs mean along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::RObject] returns result of mean.
   */
  rb_define_method(cT, "mean", robject_mean, -1);
  /**
   * var of self.
   * @overload var(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs var along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::RObject] returns result of var.
   */
  rb_define_method(cT, "var", robject_var, -1);
  /**
   * stddev of self.
   * @overload stddev(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs stddev along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::RObject] returns result of stddev.
   */
  rb_define_method(cT, "stddev", robject_stddev, -1);
  /**
   * rms of self.
   * @overload rms(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs rms along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::RObject] returns result of rms.
   */
  rb_define_method(cT, "rms", robject_rms, -1);
  /**
   * min of self.
   * @overload min(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs min along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::RObject] returns result of min.
   */
  rb_define_method(cT, "min", robject_min, -1);
  /**
   * max of self.
   * @overload max(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs max along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::RObject] returns result of max.
   */
  rb_define_method(cT, "max", robject_max, -1);
  /**
   * ptp of self.
   * @overload ptp(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs ptp along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::RObject] returns result of ptp.
   */
  rb_define_method(cT, "ptp", robject_ptp, -1);
  /**
   * Index of the maximum value.
   * @overload max_index(axis:nil, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (return
   *     NaN posision if exist).
   *   @param [Numeric,Array,Range] axis  Finds maximum values along the axis and
   *     returns **flat 1-d indices**.
   *   @return [Integer,Numo::Int] returns result indices.
   * @see #argmax
   * @see #max
   *
   * @example
   *     a = Numo::NArray[3,4,1,2]
   *     a.max_index  #=> 1
   *
   *     b = Numo::NArray[[3,4,1],[2,0,5]]
   *     b.max_index             #=> 5
   *     b.max_index(axis:1)     #=> [1, 5]
   *     b.max_index(axis:0)     #=> [0, 1, 5]
   *     b[b.max_index(axis:0)]  #=> [3, 4, 5]
   */
  rb_define_method(cT, "max_index", robject_max_index, -1);
  /**
   * Index of the minimum value.
   * @overload min_index(axis:nil, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (returnNaN posision if exist).
   *   @param [Numeric,Array,Range] axis  Finds minimum values along the axis and
   *     returns **flat 1-d indices**.
   *   @return [Integer,Numo::Int] returns result indices.
   * @see #argmin
   * @see #min
   *
   * @example
   *     a = Numo::NArray[3,4,1,2]
   *     a.min_index  #=> 2
   *
   *     b = Numo::NArray[[3,4,1],[2,0,5]]
   *     b.min_index             #=> 4
   *     b.min_index(axis:1)     #=> [2, 4]
   *     b.min_index(axis:0)     #=> [3, 4, 2]
   *     b[b.min_index(axis:0)]  #=> [2, 0, 1]
   */
  rb_define_method(cT, "min_index", robject_min_index, -1);
  /**
   * Index of the maximum value.
   * @overload argmax(axis:nil, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (return NaN posision
   *     if exist).
   *   @param [Numeric,Array,Range] axis  Finds maximum values along the axis and
   *     returns **indices along the axis**.
   *   @return [Integer,Numo::Int] returns the result indices.
   * @see #max_index
   * @see #max
   *
   * @example
   *     a = Numo::NArray[3,4,1,2]
   *     a.argmax  #=> 1
   *
   *     b = Numo::NArray[[3,4,1],[2,0,5]]
   *     b.argmax                       #=> 5
   *     b.argmax(axis:1)               #=> [1, 2]
   *     b.argmax(axis:0)               #=> [0, 0, 1]
   *     b.at(b.argmax(axis:0), 0..-1)  #=> [3, 4, 5]
   */
  rb_define_method(cT, "argmax", robject_argmax, -1);
  /**
   * Index of the minimum value.
   * @overload argmin(axis:nil, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (return
   *     NaN posision if exist).
   *   @param [Numeric,Array,Range] axis  Finds minimum values along the axis and
   *     returns **indices along the axis**.
   *   @return [Integer,Numo::Int] returns the result indices.
   * @see #min_index
   * @see #min
   *
   * @example
   *     a = Numo::NArray[3,4,1,2]
   *     a.argmin  #=> 2
   *
   *     b = Numo::NArray[[3,4,1],[2,0,5]]
   *     b.argmin                       #=> 4
   *     b.argmin(axis:1)               #=> [2, 1]
   *     b.argmin(axis:0)               #=> [1, 1, 0]
   *     b.at(b.argmin(axis:0), 0..-1)  #=> [2, 0, 1]
   */
  rb_define_method(cT, "argmin", robject_argmin, -1);
  /**
   * minmax of self.
   * @overload minmax(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (return NaN if exist).
   *   @param [Numeric,Array,Range] axis  Finds min-max along the axis.
   *   @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in
   *     the result array as dimensions with size one.
   *   @return [Numo::RObject,Numo::RObject] min and max of self.
   */
  rb_define_method(cT, "minmax", robject_minmax, -1);
  /**
   * Element-wise maximum of two arrays.
   * @overload maximum(a1, a2, nan:false)
   *   @param [Numo::NArray,Numeric] a1  The array to be compared.
   *   @param [Numo::NArray,Numeric] a2  The array to be compared.
   *   @param [Boolean] nan  If true, apply NaN-aware algorithm (return NaN if exist).
   *   @return [Numo::RObject]
   */
  rb_define_module_function(cT, "maximum", robject_s_maximum, -1);
  /**
   * Element-wise minimum of two arrays.
   * @overload minimum(a1, a2, nan:false)
   *   @param [Numo::NArray,Numeric] a1  The array to be compared.
   *   @param [Numo::NArray,Numeric] a2  The array to be compared.
   *   @param [Boolean] nan  If true, apply NaN-aware algorithm (return NaN if exist).
   *   @return [Numo::RObject]
   */
  rb_define_module_function(cT, "minimum", robject_s_minimum, -1);
  /**
   * cumsum of self.
   * @overload cumsum(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumsum along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::RObject] cumsum of self.
   */
  rb_define_method(cT, "cumsum", robject_cumsum, -1);
  /**
   * cumprod of self.
   * @overload cumprod(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumprod along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::RObject] cumprod of self.
   */
  rb_define_method(cT, "cumprod", robject_cumprod, -1);
  /**
   * Binary mulsum.
   *
   * @overload mulsum(other, axis:nil, keepdims:false, nan:false)
   *   @param [Numo::NArray,Numeric] other
   *   @param [Numeric,Array,Range] axis  Performs mulsum along the axis.
   *   @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in
   *     the result array as dimensions with size one.
   *   @param [TrueClass] nan (keyword) If true, apply NaN-aware algorithm
   *     (avoid NaN if exists).
   *   @return [Numo::NArray] mulsum of self and other.
   */
  rb_define_method(cT, "mulsum", robject_mulsum, -1);
  /**
   * Set linear sequence of numbers to self. The sequence is obtained from
   *    beg+i*step
   * where i is 1-dimensional index.
   * @overload seq([beg,[step]])
   *   @param [Numeric] beg  beginning of sequence. (default=0)
   *   @param [Numeric] step  step of sequence. (default=1)
   *   @return [Numo::RObject] self.
   * @example
   *   Numo::DFloat.new(6).seq(1,-0.2)
   *   # => Numo::DFloat#shape=[6]
   *   # [1, 0.8, 0.6, 0.4, 0.2, 0]
   *
   *   Numo::DComplex.new(6).seq(1,-0.2+0.2i)
   *   # => Numo::DComplex#shape=[6]
   *   # [1+0i, 0.8+0.2i, 0.6+0.4i, 0.4+0.6i, 0.2+0.8i, 0+1i]
   */
  rb_define_method(cT, "seq", robject_seq, -1);
  /**
   * Set logarithmic sequence of numbers to self. The sequence is obtained from
   *    `base**(beg+i*step)`
   * where i is 1-dimensional index.
   * Applicable classes: DFloat, SFloat, DComplex, SCopmplex.
   *
   * @overload logseq(beg,step,[base])
   *   @param [Numeric] beg  The beginning of sequence.
   *   @param [Numeric] step  The step of sequence.
   *   @param [Numeric] base  The base of log space. (default=10)
   *   @return [Numo::RObject] self.
   *
   * @example
   *   Numo::DFloat.new(5).logseq(4,-1,2)
   *   # => Numo::DFloat#shape=[5]
   *   # [16, 8, 4, 2, 1]
   *
   *   Numo::DComplex.new(5).logseq(0,1i*Math::PI/3,Math::E)
   *   # => Numo::DComplex#shape=[5]
   *   # [1+7.26156e-310i, 0.5+0.866025i, -0.5+0.866025i, -1+1.22465e-16i, ...]
   */
  rb_define_method(cT, "logseq", robject_logseq, -1);
  /**
   * Eye: Set a value to diagonal components, set 0 to non-diagonal components.
   * @overload eye([element,offset])
   *   @param [Numeric] element  Diagonal element to be stored. Default is 1.
   *   @param [Integer] offset Diagonal offset from the main diagonal.  The
   *       default is 0. k>0 for diagonals above the main diagonal, and k<0
   *       for diagonals below the main diagonal.
   *   @return [Numo::RObject] eye of self.
   */
  rb_define_method(cT, "eye", robject_eye, -1);
  rb_define_alias(cT, "indgen", "seq");
  /**
   * Generate uniformly distributed random numbers on self narray.
   * @overload rand([[low],high])
   *   @param [Numeric] low  lower inclusive boundary of random numbers. (default=0)
   *   @param [Numeric] high  upper exclusive boundary of random numbers.
   *     (default=1 or 1+1i for complex types)
   *   @return [Numo::RObject] self.
   * @example
   *   Numo::DFloat.new(6).rand
   *   # => Numo::DFloat#shape=[6]
   *   # [0.0617545, 0.373067, 0.794815, 0.201042, 0.116041, 0.344032]
   *
   *   Numo::DComplex.new(6).rand(5+5i)
   *   # => Numo::DComplex#shape=[6]
   *   # [2.69974+3.68908i, 0.825443+0.254414i, 0.540323+0.34354i, 4.52061+2.39322i, ...]
   *
   *   Numo::Int32.new(6).rand(2,5)
   *   # => Numo::Int32#shape=[6]
   *   # [4, 3, 3, 2, 4, 2]
   */
  rb_define_method(cT, "rand", robject_rand, -1);
  rb_define_method(cT, "poly", robject_poly, -2);
  rb_define_singleton_method(cT, "[]", robject_s_cast, -2);
}
