/*
  t_uint64.c
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

static ID id_pow;
static ID id_left_shift;
static ID id_right_shift;
static ID id_cast;
static ID id_divmod;
static ID id_eq;
static ID id_ge;
static ID id_gt;
static ID id_le;
static ID id_lt;
static ID id_minlength;
static ID id_mulsum;
static ID id_nan;
static ID id_ne;
static ID id_to_a;

#include <numo/types/uint64.h>

/*
  class definition: Numo::UInt64
*/
VALUE cT;
extern VALUE cRT;

#include "mh/extract.h"
#include "mh/coerce_cast.h"
#include "mh/to_a.h"
#include "mh/fill.h"
#include "mh/format.h"
#include "mh/format_to_a.h"
#include "mh/inspect.h"
#include "mh/each.h"
#include "mh/map.h"
#include "mh/each_with_index.h"
#include "mh/map_with_index.h"
#include "mh/abs.h"
#include "mh/op/add.h"
#include "mh/op/sub.h"
#include "mh/op/mul.h"
#include "mh/op/div.h"
#include "mh/op/mod.h"
#include "mh/divmod.h"
#include "mh/pow.h"
#include "mh/minus.h"
#include "mh/reciprocal.h"
#include "mh/sign.h"
#include "mh/square.h"
#include "mh/comp/eq.h"
#include "mh/comp/ne.h"
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
#include "mh/sum.h"
#include "mh/prod.h"
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
#include "mh/bincount.h"
#include "mh/cumsum.h"
#include "mh/cumprod.h"
#include "mh/mulsum.h"
#include "mh/seq.h"
#include "mh/eye.h"
#include "mh/rand.h"
#include "mh/mean.h"
#include "mh/var.h"
#include "mh/stddev.h"
#include "mh/rms.h"

typedef u_int64_t uint64; // Type aliases for shorter notation
                          // following the codebase naming convention.
DEF_NARRAY_EXTRACT_METHOD_FUNC(uint64)
DEF_NARRAY_COERCE_CAST_METHOD_FUNC(uint64)
DEF_NARRAY_TO_A_METHOD_FUNC(uint64)
DEF_NARRAY_FILL_METHOD_FUNC(uint64)
DEF_NARRAY_FORMAT_METHOD_FUNC(uint64)
DEF_NARRAY_FORMAT_TO_A_METHOD_FUNC(uint64)
DEF_NARRAY_INSPECT_METHOD_FUNC(uint64)
DEF_NARRAY_EACH_METHOD_FUNC(uint64)
DEF_NARRAY_MAP_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_EACH_WITH_INDEX_METHOD_FUNC(uint64)
DEF_NARRAY_MAP_WITH_INDEX_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_ABS_METHOD_FUNC(uint64, numo_cUInt64, uint64, numo_cUInt64)
DEF_NARRAY_ADD_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_SUB_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_MUL_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_DIV_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MOD_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_DIVMOD_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT64_POW_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_MINUS_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_RECIPROCAL_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_SIGN_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_SQUARE_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_EQ_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_NE_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_GT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_GE_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_LT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_LE_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_BIT_AND_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_BIT_OR_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_BIT_XOR_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_BIT_NOT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_LEFT_SHIFT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_RIGHT_SHIFT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_CLIP_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_SUM_METHOD_FUNC(uint64, numo_cUInt64, u_int64_t, numo_cUInt64)
DEF_NARRAY_INT_PROD_METHOD_FUNC(uint64, numo_cUInt64, u_int64_t, numo_cUInt64)
DEF_NARRAY_INT_MIN_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MAX_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_PTP_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MAX_INDEX_METHOD_FUNC(uint64)
DEF_NARRAY_INT_MIN_INDEX_METHOD_FUNC(uint64)
DEF_NARRAY_INT_ARGMAX_METHOD_FUNC(uint64)
DEF_NARRAY_INT_ARGMIN_METHOD_FUNC(uint64)
DEF_NARRAY_INT_MAXIMUM_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MINIMUM_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MINMAX_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_UINT_BINCOUNT_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_CUMSUM_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_CUMPROD_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_MULSUM_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_SEQ_METHOD_FUNC(uint64)
DEF_NARRAY_EYE_METHOD_FUNC(uint64)
DEF_NARRAY_INT64_RAND_METHOD_FUNC(uint64)
DEF_NARRAY_INT_MEAN_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_VAR_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_STDDEV_METHOD_FUNC(uint64, numo_cUInt64)
DEF_NARRAY_INT_RMS_METHOD_FUNC(uint64, numo_cUInt64)

static VALUE uint64_store(VALUE, VALUE);

static size_t uint64_memsize(const void* ptr) {
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

static void uint64_free(void* ptr) {
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

static narray_type_info_t uint64_info = {

  0,             // element_bits
  sizeof(dtype), // element_bytes
  sizeof(dtype), // element_stride (in bytes)

};

static const rb_data_type_t uint64_data_type = {
  "Numo::UInt64",
  {
    0,
    uint64_free,
    uint64_memsize,
  },
  &na_data_type,
  &uint64_info,
  RUBY_TYPED_FROZEN_SHAREABLE, // flags
};

static VALUE uint64_s_alloc_func(VALUE klass) {
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
  return TypedData_Wrap_Struct(klass, &uint64_data_type, (void*)na);
}

static VALUE uint64_allocate(VALUE self) {
  narray_t* na;
  char* ptr;

  GetNArray(self, na);

  switch (NA_TYPE(na)) {
  case NARRAY_DATA_T:
    ptr = NA_DATA_PTR(na);
    if (na->size > 0 && ptr == NULL) {
      ptr = xmalloc(sizeof(dtype) * na->size);

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

static VALUE uint64_new_dim0(dtype x) {
  VALUE v;
  dtype* ptr;

  v = nary_new(cT, 0, NULL);
  ptr = (dtype*)(char*)na_get_pointer_for_write(v);
  *ptr = x;
  na_release_lock(v);
  return v;
}

static VALUE uint64_store_numeric(VALUE self, VALUE obj) {
  dtype x;
  x = m_num_to_data(obj);
  obj = uint64_new_dim0(x);
  uint64_store(self, obj);
  return self;
}

static void iter_uint64_store_bit(na_loop_t* const lp) {
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

static VALUE uint64_store_bit(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_bit, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_dfloat(na_loop_t* const lp) {
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

static VALUE uint64_store_dfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_dfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_sfloat(na_loop_t* const lp) {
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

static VALUE uint64_store_sfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_sfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_int64(na_loop_t* const lp) {
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

static VALUE uint64_store_int64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_int64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_int32(na_loop_t* const lp) {
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

static VALUE uint64_store_int32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_int32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_int16(na_loop_t* const lp) {
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

static VALUE uint64_store_int16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_int16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_int8(na_loop_t* const lp) {
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

static VALUE uint64_store_int8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_int8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_uint64(na_loop_t* const lp) {
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

static VALUE uint64_store_uint64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_uint64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_uint32(na_loop_t* const lp) {
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

static VALUE uint64_store_uint32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_uint32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_uint16(na_loop_t* const lp) {
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

static VALUE uint64_store_uint16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_uint16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_uint8(na_loop_t* const lp) {
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

static VALUE uint64_store_uint8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_uint8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_robject(na_loop_t* const lp) {
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

static VALUE uint64_store_robject(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_uint64_store_robject, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_uint64_store_array(na_loop_t* const lp) {
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
      iter_uint64_store_uint64(lp);
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

static VALUE uint64_store_array(VALUE self, VALUE rary) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { rb_cArray, 0 } };
  ndfunc_t ndf = { iter_uint64_store_array, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop_store_rarray(&ndf, self, rary);
  return self;
}

/*
  Store elements to Numo::UInt64 from other.
  @overload store(other)
    @param [Object] other
    @return [Numo::UInt64] self
*/
static VALUE uint64_store(VALUE self, VALUE obj) {
  VALUE r, klass;

  klass = rb_obj_class(obj);

  if (klass == numo_cUInt64) {
    uint64_store_uint64(self, obj);
    return self;
  }

  if (IS_INTEGER_CLASS(klass) || klass == rb_cFloat || klass == rb_cComplex) {
    uint64_store_numeric(self, obj);
    return self;
  }

  if (klass == numo_cBit) {
    uint64_store_bit(self, obj);
    return self;
  }

  if (klass == numo_cDFloat) {
    uint64_store_dfloat(self, obj);
    return self;
  }

  if (klass == numo_cSFloat) {
    uint64_store_sfloat(self, obj);
    return self;
  }

  if (klass == numo_cInt64) {
    uint64_store_int64(self, obj);
    return self;
  }

  if (klass == numo_cInt32) {
    uint64_store_int32(self, obj);
    return self;
  }

  if (klass == numo_cInt16) {
    uint64_store_int16(self, obj);
    return self;
  }

  if (klass == numo_cInt8) {
    uint64_store_int8(self, obj);
    return self;
  }

  if (klass == numo_cUInt32) {
    uint64_store_uint32(self, obj);
    return self;
  }

  if (klass == numo_cUInt16) {
    uint64_store_uint16(self, obj);
    return self;
  }

  if (klass == numo_cUInt8) {
    uint64_store_uint8(self, obj);
    return self;
  }

  if (klass == numo_cRObject) {
    uint64_store_robject(self, obj);
    return self;
  }

  if (klass == rb_cArray) {
    uint64_store_array(self, obj);
    return self;
  }

  if (IsNArray(obj)) {
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      uint64_store(self, r);
      return self;
    }
  }

  rb_raise(
    nary_eCastError, "unknown conversion from %s to %s", rb_class2name(rb_obj_class(obj)),
    rb_class2name(rb_obj_class(self))
  );

  return self;
}

/*
  Convert a data value of obj (with a single element) to dtype.
*/
static dtype uint64_extract_data(VALUE obj) {
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

    if (klass == numo_cUInt64) {
      x = m_from_uint64(*(u_int64_t*)(ptr + pos));
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

    if (klass == numo_cRObject) {
      x = m_num_to_data(*(VALUE*)(ptr + pos));
      return x;
    }

    // coerce
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      return uint64_extract_data(r);
    }

    rb_raise(
      nary_eCastError, "unknown conversion from %s to %s", rb_class2name(rb_obj_class(obj)),
      rb_class2name(cT)
    );
  }
  if (TYPE(obj) == T_ARRAY) {
    if (RARRAY_LEN(obj) != 1) {
      rb_raise(nary_eShapeError, "array size should be 1");
    }
    return m_num_to_data(RARRAY_AREF(obj, 0));
  }
  return m_num_to_data(obj);
}

static VALUE uint64_cast_array(VALUE rary) {
  VALUE nary;
  narray_t* na;

  nary = na_s_new_like(cT, rary);
  GetNArray(nary, na);
  if (na->size > 0) {
    uint64_store_array(nary, rary);
  }
  return nary;
}

/*
  Cast object to Numo::UInt64.
  @overload [](elements)
  @overload cast(array)
    @param [Numeric,Array] elements
    @param [Array] array
    @return [Numo::UInt64]
*/
static VALUE uint64_s_cast(VALUE type, VALUE obj) {
  VALUE v;
  narray_t* na;
  dtype x;

  if (rb_obj_class(obj) == cT) {
    return obj;
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cNumeric))) {
    x = m_num_to_data(obj);
    return uint64_new_dim0(x);
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cArray))) {
    return uint64_cast_array(obj);
  }
  if (IsNArray(obj)) {
    GetNArray(obj, na);
    v = nary_new(cT, NA_NDIM(na), NA_SHAPE(na));
    if (NA_SIZE(na) > 0) {
      uint64_store(v, obj);
    }
    return v;
  }
  if (rb_respond_to(obj, id_to_a)) {
    obj = rb_funcall(obj, id_to_a, 0);
    if (TYPE(obj) != T_ARRAY) {
      rb_raise(rb_eTypeError, "`to_a' did not return Array");
    }
    return uint64_cast_array(obj);
  }

  rb_raise(nary_eCastError, "cannot cast to %s", rb_class2name(type));
  return Qnil;
}

/*
  Multi-dimensional element reference.
  @overload [](dim0,...,dimL)
    @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol]
    dim0,...,dimL  multi-dimensional indices.
    @return [Numeric,Numo::UInt64] an element or NArray view.
  @see Numo::NArray#[]
  @see #[]=
 */
static VALUE uint64_aref(int argc, VALUE* argv, VALUE self) {
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
static VALUE uint64_aset(int argc, VALUE* argv, VALUE self) {
  int nd;
  size_t pos;
  char* ptr;
  VALUE a;
  dtype x;

  argc--;
  if (argc == 0) {
    uint64_store(self, argv[argc]);
  } else {
    nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
    if (nd) {
      a = na_aref_main(argc, argv, self, 0, nd);
      uint64_store(a, argv[argc]);
    } else {
      x = uint64_extract_data(argv[argc]);
      ptr = na_get_pointer_for_read_write(self) + pos;
      *(dtype*)ptr = x;
    }
  }
  return argv[argc];
}

static void iter_uint64_poly(na_loop_t* const lp) {
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
    @return [Numo::UInt64]
*/
static VALUE uint64_poly(VALUE self, VALUE args) {
  int argc, i;
  VALUE* argv;
  volatile VALUE v, a;
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_uint64_poly, NO_LOOP, 0, 1, 0, aout };

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
  return uint64_extract(v);
}

/*
  qsort.c
  Ruby/Numo::NArray - Numerical Array class for Ruby
    modified by Masahiro TANAKA
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef QSORT_INCL
#define QSORT_INCL
#define Min(x, y) ((x) < (y) ? (x) : (y))

/*
 * Qsort routine based on J. L. Bentley and M. D. McIlroy,
 * "Engineering a sort function",
 * Software--Practice and Experience 23 (1993) 1249-1265.
 * We have modified their original by adding a check for already-sorted input,
 * which seems to be a win per discussions on pgsql-hackers around 2006-03-21.
 */
#define swapcode(TYPE, parmi, parmj, n)                                                        \
  do {                                                                                         \
    size_t i = (n) / sizeof(TYPE);                                                             \
    TYPE* pi = (TYPE*)(void*)(parmi);                                                          \
    TYPE* pj = (TYPE*)(void*)(parmj);                                                          \
    do {                                                                                       \
      TYPE t = *pi;                                                                            \
      *pi++ = *pj;                                                                             \
      *pj++ = t;                                                                               \
    } while (--i > 0);                                                                         \
  } while (0)

#ifdef HAVE_STDINT_H
#define SWAPINIT(a, es)                                                                        \
  swaptype = (uintptr_t)(a) % sizeof(long) || (es) % sizeof(long) ? 2                          \
             : (es) == sizeof(long)                               ? 0                          \
                                                                  : 1;
#else
#define SWAPINIT(a, es)                                                                        \
  swaptype = ((char*)(a) - (char*)0) % sizeof(long) || (es) % sizeof(long) ? 2                 \
             : (es) == sizeof(long)                                        ? 0                 \
                                                                           : 1;
#endif

static inline void swapfunc(char* a, char* b, size_t n, int swaptype) {
  if (swaptype <= 1)
    swapcode(long, a, b, n);
  else
    swapcode(char, a, b, n);
}

#define swap(a, b)                                                                             \
  if (swaptype == 0) {                                                                         \
    long t = *(long*)(void*)(a);                                                               \
    *(long*)(void*)(a) = *(long*)(void*)(b);                                                   \
    *(long*)(void*)(b) = t;                                                                    \
  } else                                                                                       \
    swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n)                                                                       \
  if ((n) > 0) swapfunc((a), (b), (size_t)(n), swaptype)

#define med3(a, b, c, _cmp)                                                                    \
  (cmpgt(b, a) ? (cmpgt(c, b) ? b : (cmpgt(c, a) ? c : a))                                     \
               : (cmpgt(b, c) ? b : (cmpgt(c, a) ? a : c)))
#endif

#undef qsort_dtype
#define qsort_dtype dtype
#undef qsort_cast
#define qsort_cast *(dtype*)

static void uint64_qsort(void* a, size_t n, ssize_t es) {
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  int d, r, swaptype, presorted;

loop:
  SWAPINIT(a, es);
  if (n < 7) {
    for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es)
      for (pl = pm; pl > (char*)a && cmpgt(pl - es, pl); pl -= es) swap(pl, pl - es);
    return;
  }
  presorted = 1;
  for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es) {
    if (cmpgt(pm - es, pm)) {
      presorted = 0;
      break;
    }
  }
  if (presorted) return;
  pm = (char*)a + (n / 2) * es;
  if (n > 7) {
    pl = (char*)a;
    pn = (char*)a + (n - 1) * es;
    if (n > 40) {
      d = (int)((n / 8) * es);
      pl = med3(pl, pl + d, pl + 2 * d, cmp);
      pm = med3(pm - d, pm, pm + d, cmp);
      pn = med3(pn - 2 * d, pn - d, pn, cmp);
    }
    pm = med3(pl, pm, pn, cmp);
  }
  swap(a, pm);
  pa = pb = (char*)a + es;
  pc = pd = (char*)a + (n - 1) * es;
  for (;;) {
    while (pb <= pc && (r = cmp(pb, a)) <= 0) {
      if (r == 0) {
        swap(pa, pb);
        pa += es;
      }
      pb += es;
    }
    while (pb <= pc && (r = cmp(pc, a)) >= 0) {
      if (r == 0) {
        swap(pc, pd);
        pd -= es;
      }
      pc -= es;
    }
    if (pb > pc) break;
    swap(pb, pc);
    pb += es;
    pc -= es;
  }
  pn = (char*)a + n * es;
  r = (int)Min(pa - (char*)a, pb - pa);
  vecswap(a, pb - r, r);
  r = (int)Min(pd - pc, pn - pd - es);
  vecswap(pb, pn - r, r);
  if ((r = (int)(pb - pa)) > es) uint64_qsort(a, r / es, es);
  if ((r = (int)(pd - pc)) > es) {
    /* Iterate rather than recurse to save stack space */
    a = pn - r;
    n = r / es;
    goto loop;
  }
  /*              qsort(pn - r, r / es, es, cmp);*/
}

static void iter_uint64_sort(na_loop_t* const lp) {
  size_t n;
  char* ptr;
  ssize_t step;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, ptr, step);
  uint64_qsort(ptr, n, step);
}

/*
  sort of self.
  @overload sort(axis:nil)
    @param [Numeric,Array,Range] axis  Performs sort along the axis.
    @return [Numo::UInt64] returns result of sort.
  @example
      Numo::DFloat[3,4,1,2].sort #=> Numo::DFloat[1,2,3,4]
*/
static VALUE uint64_sort(int argc, VALUE* argv, VALUE self) {
  VALUE reduce;
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { sym_reduce, 0 } };
  ndfunc_t ndf = { 0, NDF_HAS_LOOP | NDF_FLAT_REDUCE, 2, 0, ain, 0 };

  if (!TEST_INPLACE(self)) {
    self = na_copy(self);
  }

  ndf.func = iter_uint64_sort;
  reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);

  na_ndloop(&ndf, 2, self, reduce);
  return self;
}

/*
  qsort.c
  Ruby/Numo::NArray - Numerical Array class for Ruby
    modified by Masahiro TANAKA
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#undef qsort_dtype
#define qsort_dtype dtype*
#undef qsort_cast
#define qsort_cast **(dtype**)

static void uint64_index_qsort(void* a, size_t n, ssize_t es) {
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  int d, r, swaptype, presorted;

loop:
  SWAPINIT(a, es);
  if (n < 7) {
    for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es)
      for (pl = pm; pl > (char*)a && cmpgt(pl - es, pl); pl -= es) swap(pl, pl - es);
    return;
  }
  presorted = 1;
  for (pm = (char*)a + es; pm < (char*)a + n * es; pm += es) {
    if (cmpgt(pm - es, pm)) {
      presorted = 0;
      break;
    }
  }
  if (presorted) return;
  pm = (char*)a + (n / 2) * es;
  if (n > 7) {
    pl = (char*)a;
    pn = (char*)a + (n - 1) * es;
    if (n > 40) {
      d = (int)((n / 8) * es);
      pl = med3(pl, pl + d, pl + 2 * d, cmp);
      pm = med3(pm - d, pm, pm + d, cmp);
      pn = med3(pn - 2 * d, pn - d, pn, cmp);
    }
    pm = med3(pl, pm, pn, cmp);
  }
  swap(a, pm);
  for (pa = pb = (char*)a + es, pc = pd = (char*)a + (n - 1) * es; pb <= pc;
       pb += es, pc -= es) {
    while (pb <= pc && (r = cmp(pb, a)) <= 0) {
      if (r == 0) {
        swap(pa, pb);
        pa += es;
      }
      pb += es;
    }
    while (pb <= pc && (r = cmp(pc, a)) >= 0) {
      if (r == 0) {
        swap(pc, pd);
        pd -= es;
      }
      pc -= es;
    }
    if (pb > pc) break;
    swap(pb, pc);
  }
  pn = (char*)a + n * es;
  r = (int)Min(pa - (char*)a, pb - pa);
  vecswap(a, pb - r, r);
  r = (int)Min(pd - pc, pn - pd - es);
  vecswap(pb, pn - r, r);
  if ((r = (int)(pb - pa)) > es) uint64_index_qsort(a, r / es, es);
  if ((r = (int)(pd - pc)) > es) {
    /* Iterate rather than recurse to save stack space */
    a = pn - r;
    n = r / es;
    goto loop;
  }
  /*              qsort(pn - r, r / es, es, cmp);*/
}

#define idx_t int64_t
static void uint64_index64_qsort(na_loop_t* const lp) {
  size_t i, n, idx;
  char *d_ptr, *i_ptr, *o_ptr;
  ssize_t d_step, i_step, o_step;
  char** ptr;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, d_ptr, d_step);
  INIT_PTR(lp, 1, i_ptr, i_step);
  INIT_PTR(lp, 2, o_ptr, o_step);

  ptr = (char**)(lp->opt_ptr);

  // o_ptr=%lx,o_step=%ld)\n",(size_t)ptr,(size_t)d_ptr,(ssize_t)d_step,(size_t)i_ptr,(ssize_t)i_step,(size_t)o_ptr,(ssize_t)o_step);

  if (n == 1) {
    *(idx_t*)o_ptr = *(idx_t*)(i_ptr);
    return;
  }

  for (i = 0; i < n; i++) {
    ptr[i] = d_ptr + d_step * i;
  }

  uint64_index_qsort(ptr, n, sizeof(dtype*));

  // d_ptr = lp->args[0].ptr;

  for (i = 0; i < n; i++) {
    idx = (ptr[i] - d_ptr) / d_step;
    *(idx_t*)o_ptr = *(idx_t*)(i_ptr + i_step * idx);
    o_ptr += o_step;
  }
}
#undef idx_t

#define idx_t int32_t
static void uint64_index32_qsort(na_loop_t* const lp) {
  size_t i, n, idx;
  char *d_ptr, *i_ptr, *o_ptr;
  ssize_t d_step, i_step, o_step;
  char** ptr;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, d_ptr, d_step);
  INIT_PTR(lp, 1, i_ptr, i_step);
  INIT_PTR(lp, 2, o_ptr, o_step);

  ptr = (char**)(lp->opt_ptr);

  // o_ptr=%lx,o_step=%ld)\n",(size_t)ptr,(size_t)d_ptr,(ssize_t)d_step,(size_t)i_ptr,(ssize_t)i_step,(size_t)o_ptr,(ssize_t)o_step);

  if (n == 1) {
    *(idx_t*)o_ptr = *(idx_t*)(i_ptr);
    return;
  }

  for (i = 0; i < n; i++) {
    ptr[i] = d_ptr + d_step * i;
  }

  uint64_index_qsort(ptr, n, sizeof(dtype*));

  // d_ptr = lp->args[0].ptr;

  for (i = 0; i < n; i++) {
    idx = (ptr[i] - d_ptr) / d_step;
    *(idx_t*)o_ptr = *(idx_t*)(i_ptr + i_step * idx);
    o_ptr += o_step;
  }
}
#undef idx_t

/*
  sort_index. Returns an index array of sort result.
  @overload sort_index(axis:nil)
    @param [Numeric,Array,Range] axis  Performs sort_index along the axis.
    @return [Integer,Numo::Int] returns result index of sort_index.
  @example
      Numo::NArray[3,4,1,2].sort_index #=> Numo::Int32[2,3,0,1]
*/
static VALUE uint64_sort_index(int argc, VALUE* argv, VALUE self) {
  size_t size;
  narray_t* na;
  VALUE idx, tmp, reduce, res;
  char* buf;
  ndfunc_arg_in_t ain[3] = { { cT, 0 }, { 0, 0 }, { sym_reduce, 0 } };
  ndfunc_arg_out_t aout[1] = { { 0, 0, 0 } };
  ndfunc_t ndf = { 0, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE | NDF_CUM, 3, 1, ain, aout };

  GetNArray(self, na);
  if (na->ndim == 0) {
    return INT2FIX(0);
  }
  if (na->size > (~(u_int32_t)0)) {
    ain[1].type = aout[0].type = numo_cInt64;
    idx = nary_new(numo_cInt64, na->ndim, na->shape);

    ndf.func = uint64_index64_qsort;
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);

  } else {
    ain[1].type = aout[0].type = numo_cInt32;
    idx = nary_new(numo_cInt32, na->ndim, na->shape);

    ndf.func = uint64_index32_qsort;
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
  }
  rb_funcall(idx, rb_intern("seq"), 0);

  size = na->size * sizeof(void*); // max capa
  buf = rb_alloc_tmp_buffer(&tmp, size);
  res = na_ndloop3(&ndf, buf, 3, self, idx, reduce);
  rb_free_tmp_buffer(&tmp);
  return res;
}

static void iter_uint64_median(na_loop_t* const lp) {
  size_t n;
  char *p1, *p2;
  dtype* buf;

  INIT_COUNTER(lp, n);
  p1 = (lp->args[0]).ptr + (lp->args[0].iter[0]).pos;
  p2 = (lp->args[1]).ptr + (lp->args[1].iter[0]).pos;
  buf = (dtype*)p1;

  uint64_qsort(buf, n, sizeof(dtype));

  if (n == 0) {
    *(dtype*)p2 = buf[0];
  } else if (n % 2 == 0) {
    *(dtype*)p2 = (buf[n / 2 - 1] + buf[n / 2]) / 2;
  } else {
    *(dtype*)p2 = buf[(n - 1) / 2];
  }
}

/*
  median of self.
  @overload median(axis:nil, keepdims:false)
    @param [Numeric,Array,Range] axis  Finds median along the axis.
    @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
    dimensions with size one.
    @return [Numo::UInt64] returns median of self.
*/

static VALUE uint64_median(int argc, VALUE* argv, VALUE self) {
  VALUE v, reduce;
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { sym_reduce, 0 } };
  ndfunc_arg_out_t aout[1] = { { INT2FIX(0), 0 } };
  ndfunc_t ndf = { 0, NDF_HAS_LOOP | NDF_FLAT_REDUCE, 2, 1, ain, aout };

  self = na_copy(self); // as temporary buffer

  ndf.func = iter_uint64_median;
  reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);

  v = na_ndloop(&ndf, 2, self, reduce);
  return uint64_extract(v);
}

void Init_numo_uint64(void) {
  VALUE hCast, mNumo;

  mNumo = rb_define_module("Numo");

  id_pow = rb_intern("**");
  id_left_shift = rb_intern("<<");
  id_right_shift = rb_intern(">>");
  id_cast = rb_intern("cast");
  id_divmod = rb_intern("divmod");
  id_eq = rb_intern("eq");
  id_ge = rb_intern("ge");
  id_gt = rb_intern("gt");
  id_le = rb_intern("le");
  id_lt = rb_intern("lt");
  id_minlength = rb_intern("minlength");
  id_mulsum = rb_intern("mulsum");
  id_nan = rb_intern("nan");
  id_ne = rb_intern("ne");
  id_to_a = rb_intern("to_a");

  /**
   * Document-class: Numo::UInt64
   *
   * 64-bit unsigned integer N-dimensional array class.
   */
  cT = rb_define_class_under(mNumo, "UInt64", cNArray);

  hCast = rb_hash_new();
  /* Upcasting rules of UInt64. */
  rb_define_const(cT, "UPCAST", hCast);
  rb_hash_aset(hCast, rb_cArray, cT);

#ifdef RUBY_INTEGER_UNIFICATION
  rb_hash_aset(hCast, rb_cInteger, cT);
#else
  rb_hash_aset(hCast, rb_cFixnum, cT);
  rb_hash_aset(hCast, rb_cBignum, cT);
#endif
  rb_hash_aset(hCast, rb_cFloat, numo_cDFloat);
  rb_hash_aset(hCast, rb_cComplex, numo_cDComplex);
  rb_hash_aset(hCast, numo_cRObject, numo_cRObject);
  rb_hash_aset(hCast, numo_cDComplex, numo_cDComplex);
  rb_hash_aset(hCast, numo_cSComplex, numo_cSComplex);
  rb_hash_aset(hCast, numo_cDFloat, numo_cDFloat);
  rb_hash_aset(hCast, numo_cSFloat, numo_cSFloat);
  rb_hash_aset(hCast, numo_cInt64, numo_cInt64);
  rb_hash_aset(hCast, numo_cInt32, numo_cInt64);
  rb_hash_aset(hCast, numo_cInt16, numo_cInt64);
  rb_hash_aset(hCast, numo_cInt8, numo_cInt64);
  rb_hash_aset(hCast, numo_cUInt64, cT);
  rb_hash_aset(hCast, numo_cUInt32, cT);
  rb_hash_aset(hCast, numo_cUInt16, cT);
  rb_hash_aset(hCast, numo_cUInt8, cT);
  rb_obj_freeze(hCast);

  /* Element size of UInt64 in bits. */
  rb_define_const(cT, "ELEMENT_BIT_SIZE", INT2FIX(sizeof(dtype) * 8));
  /* Element size of UInt64 in bytes. */
  rb_define_const(cT, "ELEMENT_BYTE_SIZE", INT2FIX(sizeof(dtype)));
  /* Stride size of contiguous UInt64 array. */
  rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(sizeof(dtype)));
  /* The largest representable value of UInt64. */
  rb_define_const(cT, "MAX", M_MAX);
  /* The smallest representable value of UInt64. */
  rb_define_const(cT, "MIN", M_MIN);
  rb_define_alloc_func(cT, uint64_s_alloc_func);
  rb_define_method(cT, "allocate", uint64_allocate, 0);
  /**
   * Extract an element only if self is a dimensionless NArray.
   * @overload extract
   *   @return [Numeric,Numo::NArray]
   *   --- Extract element value as Ruby Object if self is a dimensionless NArray,
   *   otherwise returns self.
   */
  rb_define_method(cT, "extract", uint64_extract, 0);

  rb_define_method(cT, "store", uint64_store, 1);

  rb_define_singleton_method(cT, "cast", uint64_s_cast, 1);
  rb_define_method(cT, "[]", uint64_aref, -1);
  rb_define_method(cT, "[]=", uint64_aset, -1);
  /**
   * return NArray with cast to the type of self.
   * @overload coerce_cast(type)
   *   @return [nil]
   */
  rb_define_method(cT, "coerce_cast", uint64_coerce_cast, 1);
  /**
   * Convert self to Array.
   * @overload to_a
   *   @return [Array]
   */
  rb_define_method(cT, "to_a", uint64_to_a, 0);
  /**
   * Fill elements with other.
   * @overload fill other
   *   @param [Numeric] other
   *   @return [Numo::UInt64] self.
   */
  rb_define_method(cT, "fill", uint64_fill, 1);
  /**
   * Format elements into strings.
   * @overload format format
   *   @param [String] format
   *   @return [Numo::RObject] array of formatted strings.
   */
  rb_define_method(cT, "format", uint64_format, -1);
  /**
   * Format elements into strings.
   * @overload format_to_a format
   *   @param [String] format
   *   @return [Array] array of formatted strings.
   */
  rb_define_method(cT, "format_to_a", uint64_format_to_a, -1);
  /**
   * Returns a string containing a human-readable representation of NArray.
   * @overload inspect
   *   @return [String]
   */
  rb_define_method(cT, "inspect", uint64_inspect, 0);
  /**
   * Calls the given block once for each element in self,
   * passing that element as a parameter.
   * @overload each
   *   @return [Numo::NArray] self
   *   For a block `{|x| ... }`,
   *   @yieldparam [Numeric] x  an element of NArray.
   * @see #each_with_index
   * @see #map
   */
  rb_define_method(cT, "each", uint64_each, 0);
  /**
   * Unary map.
   * @overload map
   *   @return [Numo::UInt64] map of self.
   */
  rb_define_method(cT, "map", uint64_map, 0);
  /**
   * Invokes the given block once for each element of self,
   * passing that element and indices along each axis as parameters.
   * @overload each_with_index
   *   For a block `{|x,i,j,...| ... }`,
   *   @yieldparam [Numeric] x  an element
   *   @yieldparam [Integer] i,j,...  multitimensional indices
   *   @return [Numo::NArray] self
   * @see #each
   * @see #map_with_index
   */
  rb_define_method(cT, "each_with_index", uint64_each_with_index, 0);
  /**
   * Invokes the given block once for each element of self,
   * passing that element and indices along each axis as parameters.
   * Creates a new NArray containing the values returned by the block.
   * Inplace option is allowed, i.e., `nary.inplace.map` overwrites `nary`.
   * @overload map_with_index
   *   For a block `{|x,i,j,...| ... }`,
   *   @yieldparam [Numeric] x  an element
   *   @yieldparam [Integer] i,j,...  multitimensional indices
   *   @return [Numo::NArray] mapped array
   * @see #map
   * @see #each_with_index
   */
  rb_define_method(cT, "map_with_index", uint64_map_with_index, 0);
  /**
   * abs of self.
   * @overload abs
   *   @return [Numo::UInt64] abs of self.
   */
  rb_define_method(cT, "abs", uint64_abs, 0);
  /**
   * Binary add.
   * @overload + other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self + other
   */
  rb_define_method(cT, "+", uint64_add, 1);
  /**
   * Binary sub.
   * @overload - other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self - other
   */
  rb_define_method(cT, "-", uint64_sub, 1);
  /**
   * Binary mul.
   * @overload * other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self * other
   */
  rb_define_method(cT, "*", uint64_mul, 1);
  /**
   * Binary div.
   * @overload / other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self / other
   */
  rb_define_method(cT, "/", uint64_div, 1);
  /**
   * Binary mod.
   * @overload % other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self % other
   */
  rb_define_method(cT, "%", uint64_mod, 1);
  /**
   * Binary divmod.
   * @overload divmod other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] divmod of self and other.
   */
  rb_define_method(cT, "divmod", uint64_divmod, 1);
  /**
   * Binary power.
   * @overload ** other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self to the other-th power.
   */
  rb_define_method(cT, "**", uint64_pow, 1);
  rb_define_alias(cT, "pow", "**");
  /**
   * Unary minus.
   * @overload -@
   *   @return [Numo::UInt64] minus of self.
   */
  rb_define_method(cT, "-@", uint64_minus, 0);
  /**
   * Unary reciprocal.
   * @overload reciprocal
   *   @return [Numo::UInt64] reciprocal of self.
   */
  rb_define_method(cT, "reciprocal", uint64_reciprocal, 0);
  /**
   * Unary sign.
   * @overload sign
   *   @return [Numo::UInt64] sign of self.
   */
  rb_define_method(cT, "sign", uint64_sign, 0);
  /**
   * Unary square.
   * @overload square
   *   @return [Numo::UInt64] square of self.
   */
  rb_define_method(cT, "square", uint64_square, 0);
  rb_define_alias(cT, "conj", "view");
  rb_define_alias(cT, "im", "view");
  rb_define_alias(cT, "conjugate", "conj");
  /**
   * Comparison eq other.
   * @overload eq other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self eq other.
   */
  rb_define_method(cT, "eq", uint64_eq, 1);
  /**
   * Comparison ne other.
   * @overload ne other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self ne other.
   */
  rb_define_method(cT, "ne", uint64_ne, 1);
  rb_define_alias(cT, "nearly_eq", "eq");
  rb_define_alias(cT, "close_to", "nearly_eq");
  /**
   * Binary bit_and.
   * @overload & other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self & other
   */
  rb_define_method(cT, "&", uint64_bit_and, 1);
  /**
   * Binary bit_or.
   * @overload | other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self | other
   */
  rb_define_method(cT, "|", uint64_bit_or, 1);
  /**
   * Binary bit_xor.
   * @overload ^ other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self ^ other
   */
  rb_define_method(cT, "^", uint64_bit_xor, 1);
  /**
   * Unary bit_not.
   * @overload ~
   *   @return [Numo::UInt64] bit_not of self.
   */
  rb_define_method(cT, "~", uint64_bit_not, 0);
  /**
   * Binary left_shift.
   * @overload << other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self << other
   */
  rb_define_method(cT, "<<", uint64_left_shift, 1);
  /**
   * Binary right_shift.
   * @overload >> other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::NArray] self >> other
   */
  rb_define_method(cT, ">>", uint64_right_shift, 1);
  rb_define_alias(cT, "floor", "view");
  rb_define_alias(cT, "round", "view");
  rb_define_alias(cT, "ceil", "view");
  rb_define_alias(cT, "trunc", "view");
  rb_define_alias(cT, "rint", "view");
  /**
   * Comparison gt other.
   * @overload gt other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self gt other.
   */
  rb_define_method(cT, "gt", uint64_gt, 1);
  /**
   * Comparison ge other.
   * @overload ge other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self ge other.
   */
  rb_define_method(cT, "ge", uint64_ge, 1);
  /**
   * Comparison lt other.
   * @overload lt other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self lt other.
   */
  rb_define_method(cT, "lt", uint64_lt, 1);
  /**
   * Comparison le other.
   * @overload le other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self le other.
   */
  rb_define_method(cT, "le", uint64_le, 1);
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
  rb_define_method(cT, "clip", uint64_clip, 2);
  /**
   * sum of self.
   * @overload sum(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Performs sum along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::UInt64] returns result of sum.
   */
  rb_define_method(cT, "sum", uint64_sum, -1);
  /**
   * prod of self.
   * @overload prod(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Performs prod along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::UInt64] returns result of prod.
   */
  rb_define_method(cT, "prod", uint64_prod, -1);
  /**
   * min of self.
   * @overload min(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Performs min along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::UInt64] returns result of min.
   */
  rb_define_method(cT, "min", uint64_min, -1);
  /**
   * max of self.
   * @overload max(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Performs max along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::UInt64] returns result of max.
   */
  rb_define_method(cT, "max", uint64_max, -1);
  /**
   * ptp of self.
   * @overload ptp(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Performs ptp along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::UInt64] returns result of ptp.
   */
  rb_define_method(cT, "ptp", uint64_ptp, -1);
  /**
   * Index of the maximum value.
   * @overload max_index(axis:nil)
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
  rb_define_method(cT, "max_index", uint64_max_index, -1);
  /**
   * Index of the minimum value.
   * @overload min_index(axis:nil)
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
  rb_define_method(cT, "min_index", uint64_min_index, -1);
  /**
   * Index of the maximum value.
   * @overload argmax(axis:nil)
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
  rb_define_method(cT, "argmax", uint64_argmax, -1);
  /**
   * Index of the minimum value.
   * @overload argmin(axis:nil)
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
  rb_define_method(cT, "argmin", uint64_argmin, -1);
  /**
   * minmax of self.
   * @overload minmax(axis:nil, keepdims:false)
   *   @param [Numeric,Array,Range] axis  Finds min-max along the axis.
   *   @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in
   *     the result array as dimensions with size one.
   *   @return [Numo::UInt64,Numo::UInt64] min and max of self.
   */
  rb_define_method(cT, "minmax", uint64_minmax, -1);
  /**
   * Element-wise maximum of two arrays.
   * @overload maximum(a1, a2)
   *   @param [Numo::NArray,Numeric] a1,a2  The arrays holding the elements to be compared.
   *   @return [Numo::UInt64]
   */
  rb_define_module_function(cT, "maximum", uint64_s_maximum, -1);
  /**
   * Element-wise minimum of two arrays.
   * @overload minimum(a1, a2)
   *   @param [Numo::NArray,Numeric] a1,a2  The arrays holding the elements to be compared.
   *   @return [Numo::UInt64]
   */
  rb_define_module_function(cT, "minimum", uint64_s_minimum, -1);
  /**
   * Count the number of occurrences of each non-negative integer value.
   * Only Integer-types has this method.
   *
   * @overload bincount([weight], minlength:nil)
   *   @param [SFloat or DFloat or Array] weight (optional) Array of
   *     float values. Its size along last axis should be same as that of self.
   *   @param [Integer] minlength (keyword, optional) Minimum size along
   *     last axis for the output array.
   *   @return [UInt32 or UInt64 or SFloat or DFloat]
   *     Returns Float NArray if weight array is supplied,
   *     otherwise returns UInt32 or UInt64 depending on the size along last axis.
   * @example
   *   Numo::Int32[0..4].bincount
   *   # => Numo::UInt32#shape=[5]
   *   # [1, 1, 1, 1, 1]
   *
   *   Numo::Int32[0, 1, 1, 3, 2, 1, 7].bincount
   *   # => Numo::UInt32#shape=[8]
   *   # [1, 3, 1, 1, 0, 0, 0, 1]
   *
   *   x = Numo::Int32[0, 1, 1, 3, 2, 1, 7, 23]
   *   x.bincount.size == x.max+1
   *   # => true
   *
   *   w = Numo::DFloat[0.3, 0.5, 0.2, 0.7, 1.0, -0.6]
   *   x = Numo::Int32[0, 1, 1, 2, 2, 2]
   *   x.bincount(w)
   *   # => Numo::DFloat#shape=[3]
   *   # [0.3, 0.7, 1.1]
   */
  rb_define_method(cT, "bincount", uint64_bincount, -1);
  /**
   * cumsum of self.
   * @overload cumsum(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumsum along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::UInt64] cumsum of self.
   */
  rb_define_method(cT, "cumsum", uint64_cumsum, -1);
  /**
   * cumprod of self.
   * @overload cumprod(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumprod along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::UInt64] cumprod of self.
   */
  rb_define_method(cT, "cumprod", uint64_cumprod, -1);
  /**
   * Binary mulsum.
   *
   * @overload mulsum(other, axis:nil, keepdims:false)
   *   @param [Numo::NArray,Numeric] other
   *   @param [Numeric,Array,Range] axis  Performs mulsum along the axis.
   *   @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in
   *     the result array as dimensions with size one.
   *   @return [Numo::NArray] mulsum of self and other.
   */
  rb_define_method(cT, "mulsum", uint64_mulsum, -1);
  /**
   * Set linear sequence of numbers to self. The sequence is obtained from
   *    beg+i*step
   * where i is 1-dimensional index.
   * @overload seq([beg,[step]])
   *   @param [Numeric] beg  beginning of sequence. (default=0)
   *   @param [Numeric] step  step of sequence. (default=1)
   *   @return [Numo::UInt64] self.
   * @example
   *   Numo::DFloat.new(6).seq(1,-0.2)
   *   # => Numo::DFloat#shape=[6]
   *   # [1, 0.8, 0.6, 0.4, 0.2, 0]
   *
   *   Numo::DComplex.new(6).seq(1,-0.2+0.2i)
   *   # => Numo::DComplex#shape=[6]
   *   # [1+0i, 0.8+0.2i, 0.6+0.4i, 0.4+0.6i, 0.2+0.8i, 0+1i]
   */
  rb_define_method(cT, "seq", uint64_seq, -1);
  /**
   * Eye: Set a value to diagonal components, set 0 to non-diagonal components.
   * @overload eye([element,offset])
   *   @param [Numeric] element  Diagonal element to be stored. Default is 1.
   *   @param [Integer] offset Diagonal offset from the main diagonal.  The
   *       default is 0. k>0 for diagonals above the main diagonal, and k<0
   *       for diagonals below the main diagonal.
   *   @return [Numo::UInt64] eye of self.
   */
  rb_define_method(cT, "eye", uint64_eye, -1);
  rb_define_alias(cT, "indgen", "seq");
  /**
   * Generate uniformly distributed random numbers on self narray.
   * @overload rand([[low],high])
   *   @param [Numeric] low  lower inclusive boundary of random numbers. (default=0)
   *   @param [Numeric] high  upper exclusive boundary of random numbers.
   *     (default=1 or 1+1i for complex types)
   *   @return [Numo::UInt64] self.
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
  rb_define_method(cT, "rand", uint64_rand, -1);
  rb_define_method(cT, "poly", uint64_poly, -2);

  rb_define_method(cT, "sort", uint64_sort, -1);

  rb_define_method(cT, "sort_index", uint64_sort_index, -1);
  rb_define_method(cT, "median", uint64_median, -1);
  rb_define_singleton_method(cT, "[]", uint64_s_cast, -2);
  /**
   * mean of self.
   * @overload mean(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs mean along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or return NaN for min/max etc).
   *   @return [Numo::DFloat] returns result of mean.
   */
  rb_define_method(cT, "mean", uint64_mean, -1);
  /**
   * var of self.
   * @overload var(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs var along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::DFloat] returns result of var.
   */
  rb_define_method(cT, "var", uint64_var, -1);
  /**
   * stddev of self.
   * @overload stddev(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs stddev along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::DFloat] returns result of stddev.
   */
  rb_define_method(cT, "stddev", uint64_stddev, -1);
  /**
   * rms of self.
   * @overload rms(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs rms along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @return [Numo::DFloat] returns result of rms.
   */
  rb_define_method(cT, "rms", uint64_rms, -1);
}
