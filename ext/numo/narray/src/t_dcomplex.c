/*
  t_dcomplex.c
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
static ID id_cast;
static ID id_copysign;
static ID id_eq;
static ID id_imag;
static ID id_mulsum;
static ID id_ne;
static ID id_nearly_eq;
static ID id_real;
static ID id_to_a;

#include <numo/types/dcomplex.h>

/*
  class definition: Numo::DComplex
*/
VALUE cT;
extern VALUE cRT;

#include "mh/to_a.h"
#include "mh/comp/eq.h"
#include "mh/comp/ne.h"
#include "mh/comp/nearly_eq.h"
#include "mh/round/floor.h"
#include "mh/round/round.h"
#include "mh/round/ceil.h"
#include "mh/round/trunc.h"
#include "mh/round/rint.h"
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
#include "mh/cumsum.h"
#include "mh/cumprod.h"
#include "mh/mulsum.h"
#include "mh/seq.h"
#include "mh/logseq.h"
#include "mh/eye.h"
#include "mh/rand.h"
#include "mh/math/sqrt.h"
#include "mh/math/cbrt.h"
#include "mh/math/log.h"
#include "mh/math/log2.h"
#include "mh/math/log10.h"
#include "mh/math/exp.h"
#include "mh/math/exp2.h"
#include "mh/math/exp10.h"
#include "mh/math/sin.h"
#include "mh/math/cos.h"
#include "mh/math/tan.h"
#include "mh/math/asin.h"
#include "mh/math/acos.h"
#include "mh/math/atan.h"
#include "mh/math/sinh.h"
#include "mh/math/cosh.h"
#include "mh/math/tanh.h"
#include "mh/math/asinh.h"
#include "mh/math/acosh.h"
#include "mh/math/atanh.h"
#include "mh/math/sinc.h"

DEF_NARRAY_TO_A_METHOD_FUNC(dcomplex)
DEF_NARRAY_EQ_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_NE_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_NEARLY_EQ_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_FLOOR_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ROUND_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_CEIL_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_TRUNC_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_RINT_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ISNAN_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ISINF_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ISPOSINF_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ISNEGINF_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ISFINITE_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_SUM_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_PROD_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_MEAN_METHOD_FUNC(dcomplex, numo_cDComplex, dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_VAR_METHOD_FUNC(dcomplex, numo_cDComplex, double, numo_cDFloat)
DEF_NARRAY_FLT_STDDEV_METHOD_FUNC(dcomplex, numo_cDComplex, double, numo_cDFloat)
DEF_NARRAY_FLT_RMS_METHOD_FUNC(dcomplex, numo_cDComplex, double, numo_cDFloat)
DEF_NARRAY_FLT_CUMSUM_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_CUMPROD_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_MULSUM_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_SEQ_METHOD_FUNC(dcomplex)
DEF_NARRAY_FLT_LOGSEQ_METHOD_FUNC(dcomplex)
DEF_NARRAY_EYE_METHOD_FUNC(dcomplex)
DEF_NARRAY_FLT_RAND_METHOD_FUNC(dcomplex)
DEF_NARRAY_FLT_SQRT_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_CBRT_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_LOG_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_LOG2_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_LOG10_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_EXP_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_EXP2_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_EXP10_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_SIN_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_COS_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_TAN_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ASIN_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ACOS_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ATAN_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_SINH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_COSH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_TANH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ASINH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ACOSH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_ATANH_METHOD_FUNC(dcomplex, numo_cDComplex)
DEF_NARRAY_FLT_SINC_METHOD_FUNC(dcomplex, numo_cDComplex)

static VALUE dcomplex_store(VALUE, VALUE);

static size_t dcomplex_memsize(const void* ptr) {
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

static void dcomplex_free(void* ptr) {
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

static narray_type_info_t dcomplex_info = {

  0,             // element_bits
  sizeof(dtype), // element_bytes
  sizeof(dtype), // element_stride (in bytes)

};

static const rb_data_type_t dcomplex_data_type = {
  "Numo::DComplex",
  {
    0,
    dcomplex_free,
    dcomplex_memsize,
  },
  &na_data_type,
  &dcomplex_info,
  RUBY_TYPED_FROZEN_SHAREABLE, // flags
};

static VALUE dcomplex_s_alloc_func(VALUE klass) {
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
  return TypedData_Wrap_Struct(klass, &dcomplex_data_type, (void*)na);
}

static VALUE dcomplex_allocate(VALUE self) {
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

/*
  Extract an element only if self is a dimensionless NArray.
  @overload extract
    @return [Numeric,Numo::NArray]
    --- Extract element value as Ruby Object if self is a dimensionless NArray,
  otherwise returns self.
*/
static VALUE dcomplex_extract(VALUE self) {
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

static VALUE dcomplex_new_dim0(dtype x) {
  VALUE v;
  dtype* ptr;

  v = nary_new(cT, 0, NULL);
  ptr = (dtype*)(char*)na_get_pointer_for_write(v);
  *ptr = x;
  na_release_lock(v);
  return v;
}

static VALUE dcomplex_store_numeric(VALUE self, VALUE obj) {
  dtype x;
  x = m_num_to_data(obj);
  obj = dcomplex_new_dim0(x);
  dcomplex_store(self, obj);
  return self;
}

static void iter_dcomplex_store_bit(na_loop_t* const lp) {
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

static VALUE dcomplex_store_bit(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_bit, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_dcomplex(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  dcomplex x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, dcomplex, x);
        y = m_from_dcomplex(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, dcomplex, x);
        y = m_from_dcomplex(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, dcomplex, x);
        y = m_from_dcomplex(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, dcomplex, x);
        y = m_from_dcomplex(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE dcomplex_store_dcomplex(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_dcomplex, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_scomplex(na_loop_t* const lp) {
  size_t i, s1, s2;
  char *p1, *p2;
  size_t *idx1, *idx2;
  scomplex x;
  dtype y;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR_IDX(lp, 1, p2, s2, idx2);
  if (idx2) {
    if (idx1) {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, scomplex, x);
        y = m_from_scomplex(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p2, idx2, scomplex, x);
        y = m_from_scomplex(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  } else {
    if (idx1) {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, scomplex, x);
        y = m_from_scomplex(x);
        SET_DATA_INDEX(p1, idx1, dtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p2, s2, scomplex, x);
        y = m_from_scomplex(x);
        SET_DATA_STRIDE(p1, s1, dtype, y);
      }
    }
  }
}

static VALUE dcomplex_store_scomplex(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_scomplex, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_dfloat(na_loop_t* const lp) {
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

static VALUE dcomplex_store_dfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_dfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_sfloat(na_loop_t* const lp) {
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

static VALUE dcomplex_store_sfloat(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_sfloat, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_int64(na_loop_t* const lp) {
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

static VALUE dcomplex_store_int64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_int64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_int32(na_loop_t* const lp) {
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

static VALUE dcomplex_store_int32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_int32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_int16(na_loop_t* const lp) {
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

static VALUE dcomplex_store_int16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_int16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_int8(na_loop_t* const lp) {
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

static VALUE dcomplex_store_int8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_int8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_uint64(na_loop_t* const lp) {
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

static VALUE dcomplex_store_uint64(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_uint64, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_uint32(na_loop_t* const lp) {
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

static VALUE dcomplex_store_uint32(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_uint32, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_uint16(na_loop_t* const lp) {
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

static VALUE dcomplex_store_uint16(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_uint16, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_uint8(na_loop_t* const lp) {
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

static VALUE dcomplex_store_uint8(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_uint8, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_robject(na_loop_t* const lp) {
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

static VALUE dcomplex_store_robject(VALUE self, VALUE obj) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_robject, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, obj);
  return self;
}

static void iter_dcomplex_store_array(na_loop_t* const lp) {
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
      iter_dcomplex_store_dcomplex(lp);
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

static VALUE dcomplex_store_array(VALUE self, VALUE rary) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { rb_cArray, 0 } };
  ndfunc_t ndf = { iter_dcomplex_store_array, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop_store_rarray(&ndf, self, rary);
  return self;
}

/*
  Store elements to Numo::DComplex from other.
  @overload store(other)
    @param [Object] other
    @return [Numo::DComplex] self
*/
static VALUE dcomplex_store(VALUE self, VALUE obj) {
  VALUE r, klass;

  klass = rb_obj_class(obj);

  if (klass == numo_cDComplex) {
    dcomplex_store_dcomplex(self, obj);
    return self;
  }

  if (IS_INTEGER_CLASS(klass) || klass == rb_cFloat || klass == rb_cComplex) {
    dcomplex_store_numeric(self, obj);
    return self;
  }

  if (klass == numo_cBit) {
    dcomplex_store_bit(self, obj);
    return self;
  }

  if (klass == numo_cSComplex) {
    dcomplex_store_scomplex(self, obj);
    return self;
  }

  if (klass == numo_cDFloat) {
    dcomplex_store_dfloat(self, obj);
    return self;
  }

  if (klass == numo_cSFloat) {
    dcomplex_store_sfloat(self, obj);
    return self;
  }

  if (klass == numo_cInt64) {
    dcomplex_store_int64(self, obj);
    return self;
  }

  if (klass == numo_cInt32) {
    dcomplex_store_int32(self, obj);
    return self;
  }

  if (klass == numo_cInt16) {
    dcomplex_store_int16(self, obj);
    return self;
  }

  if (klass == numo_cInt8) {
    dcomplex_store_int8(self, obj);
    return self;
  }

  if (klass == numo_cUInt64) {
    dcomplex_store_uint64(self, obj);
    return self;
  }

  if (klass == numo_cUInt32) {
    dcomplex_store_uint32(self, obj);
    return self;
  }

  if (klass == numo_cUInt16) {
    dcomplex_store_uint16(self, obj);
    return self;
  }

  if (klass == numo_cUInt8) {
    dcomplex_store_uint8(self, obj);
    return self;
  }

  if (klass == numo_cRObject) {
    dcomplex_store_robject(self, obj);
    return self;
  }

  if (klass == rb_cArray) {
    dcomplex_store_array(self, obj);
    return self;
  }

  if (IsNArray(obj)) {
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      dcomplex_store(self, r);
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
static dtype dcomplex_extract_data(VALUE obj) {
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

    if (klass == numo_cDComplex) {
      {
        dcomplex* p = (dcomplex*)(ptr + pos);
        x = c_new(REAL(*p), IMAG(*p));
      };
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

    if (klass == numo_cSComplex) {
      {
        scomplex* p = (scomplex*)(ptr + pos);
        x = c_new(REAL(*p), IMAG(*p));
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

    if (klass == numo_cRObject) {
      x = m_num_to_data(*(VALUE*)(ptr + pos));
      return x;
    }

    // coerce
    r = rb_funcall(obj, rb_intern("coerce_cast"), 1, cT);
    if (rb_obj_class(r) == cT) {
      return dcomplex_extract_data(r);
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

static VALUE dcomplex_cast_array(VALUE rary) {
  VALUE nary;
  narray_t* na;

  nary = na_s_new_like(cT, rary);
  GetNArray(nary, na);
  if (na->size > 0) {
    dcomplex_store_array(nary, rary);
  }
  return nary;
}

/*
  Cast object to Numo::DComplex.
  @overload [](elements)
  @overload cast(array)
    @param [Numeric,Array] elements
    @param [Array] array
    @return [Numo::DComplex]
*/
static VALUE dcomplex_s_cast(VALUE type, VALUE obj) {
  VALUE v;
  narray_t* na;
  dtype x;

  if (rb_obj_class(obj) == cT) {
    return obj;
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cNumeric))) {
    x = m_num_to_data(obj);
    return dcomplex_new_dim0(x);
  }
  if (RTEST(rb_obj_is_kind_of(obj, rb_cArray))) {
    return dcomplex_cast_array(obj);
  }
  if (IsNArray(obj)) {
    GetNArray(obj, na);
    v = nary_new(cT, NA_NDIM(na), NA_SHAPE(na));
    if (NA_SIZE(na) > 0) {
      dcomplex_store(v, obj);
    }
    return v;
  }
  if (rb_respond_to(obj, id_to_a)) {
    obj = rb_funcall(obj, id_to_a, 0);
    if (TYPE(obj) != T_ARRAY) {
      rb_raise(rb_eTypeError, "`to_a' did not return Array");
    }
    return dcomplex_cast_array(obj);
  }

  rb_raise(nary_eCastError, "cannot cast to %s", rb_class2name(type));
  return Qnil;
}

/*
  Multi-dimensional element reference.
  @overload [](dim0,...,dimL)
    @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol]
    dim0,...,dimL  multi-dimensional indices.
    @return [Numeric,Numo::DComplex] an element or NArray view.
  @see Numo::NArray#[]
  @see #[]=
 */
static VALUE dcomplex_aref(int argc, VALUE* argv, VALUE self) {
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
static VALUE dcomplex_aset(int argc, VALUE* argv, VALUE self) {
  int nd;
  size_t pos;
  char* ptr;
  VALUE a;
  dtype x;

  argc--;
  if (argc == 0) {
    dcomplex_store(self, argv[argc]);
  } else {
    nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
    if (nd) {
      a = na_aref_main(argc, argv, self, 0, nd);
      dcomplex_store(a, argv[argc]);
    } else {
      x = dcomplex_extract_data(argv[argc]);
      ptr = na_get_pointer_for_read_write(self) + pos;
      *(dtype*)ptr = x;
    }
  }
  return argv[argc];
}

/*
  return NArray with cast to the type of self.
  @overload coerce_cast(type)
    @return [nil]
*/
static VALUE dcomplex_coerce_cast(VALUE self, VALUE type) {
  return Qnil;
}

static void iter_dcomplex_fill(na_loop_t* const lp) {
  size_t i;
  char* p1;
  ssize_t s1;
  size_t* idx1;
  VALUE x = lp->option;
  dtype y;
  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  y = m_num_to_data(x);
  if (idx1) {
    for (; i--;) {
      SET_DATA_INDEX(p1, idx1, dtype, y);
    }
  } else {
    for (; i--;) {
      SET_DATA_STRIDE(p1, s1, dtype, y);
    }
  }
}

/*
  Fill elements with other.
  @overload fill other
    @param [Numeric] other
    @return [Numo::DComplex] self.
*/
static VALUE dcomplex_fill(VALUE self, VALUE val) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { sym_option } };
  ndfunc_t ndf = { iter_dcomplex_fill, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, val);
  return self;
}

static VALUE format_dcomplex(VALUE fmt, dtype* x) {
  // fix-me
  char s[48];
  int n;

  if (NIL_P(fmt)) {
    n = m_sprintf(s, *x);
    return rb_str_new(s, n);
  }
  return rb_funcall(fmt, '%', 1, m_data_to_num(*x));
}

static void iter_dcomplex_format(na_loop_t* const lp) {
  size_t i;
  char *p1, *p2;
  ssize_t s1, s2;
  size_t* idx1;
  dtype* x;
  VALUE y;
  VALUE fmt = lp->option;
  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  INIT_PTR(lp, 1, p2, s2);
  if (idx1) {
    for (; i--;) {
      x = (dtype*)(p1 + *idx1);
      idx1++;
      y = format_dcomplex(fmt, x);
      SET_DATA_STRIDE(p2, s2, VALUE, y);
    }
  } else {
    for (; i--;) {
      x = (dtype*)p1;
      p1 += s1;
      y = format_dcomplex(fmt, x);
      SET_DATA_STRIDE(p2, s2, VALUE, y);
    }
  }
}

/*
  Format elements into strings.
  @overload format format
    @param [String] format
    @return [Numo::RObject] array of formatted strings.
*/
static VALUE dcomplex_format(int argc, VALUE* argv, VALUE self) {
  VALUE fmt = Qnil;

  ndfunc_arg_in_t ain[2] = { { Qnil, 0 }, { sym_option } };
  ndfunc_arg_out_t aout[1] = { { numo_cRObject, 0 } };
  ndfunc_t ndf = { iter_dcomplex_format, FULL_LOOP_NIP, 2, 1, ain, aout };

  rb_scan_args(argc, argv, "01", &fmt);
  return na_ndloop(&ndf, 2, self, fmt);
}

static void iter_dcomplex_format_to_a(na_loop_t* const lp) {
  size_t i;
  char* p1;
  ssize_t s1;
  size_t* idx1;
  dtype* x;
  VALUE y;
  volatile VALUE a;
  VALUE fmt = lp->option;
  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  a = rb_ary_new2(i);
  rb_ary_push(lp->args[1].value, a);
  if (idx1) {
    for (; i--;) {
      x = (dtype*)(p1 + *idx1);
      idx1++;
      y = format_dcomplex(fmt, x);
      rb_ary_push(a, y);
    }
  } else {
    for (; i--;) {
      x = (dtype*)p1;
      p1 += s1;
      y = format_dcomplex(fmt, x);
      rb_ary_push(a, y);
    }
  }
}

/*
  Format elements into strings.
  @overload format_to_a format
    @param [String] format
    @return [Array] array of formatted strings.
*/
static VALUE dcomplex_format_to_a(int argc, VALUE* argv, VALUE self) {
  VALUE fmt = Qnil;
  ndfunc_arg_in_t ain[3] = { { Qnil, 0 }, { sym_loop_opt }, { sym_option } };
  ndfunc_arg_out_t aout[1] = { { rb_cArray, 0 } }; // dummy?
  ndfunc_t ndf = { iter_dcomplex_format_to_a, FULL_LOOP_NIP, 3, 1, ain, aout };

  rb_scan_args(argc, argv, "01", &fmt);
  return na_ndloop_cast_narray_to_rarray(&ndf, self, fmt);
}

static VALUE iter_dcomplex_inspect(char* ptr, size_t pos, VALUE fmt) {
  return format_dcomplex(fmt, (dtype*)(ptr + pos));
}

/*
  Returns a string containing a human-readable representation of NArray.
  @overload inspect
    @return [String]
*/
static VALUE dcomplex_inspect(VALUE ary) {
  return na_ndloop_inspect(ary, iter_dcomplex_inspect, Qnil);
}

static void iter_dcomplex_each(na_loop_t* const lp) {
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
static VALUE dcomplex_each(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_each, FULL_LOOP_NIP, 1, 0, ain, 0 };

  na_ndloop(&ndf, 1, self);
  return self;
}

static void iter_dcomplex_map(na_loop_t* const lp) {
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
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_map(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
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
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_map(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary map.
  @overload map
    @return [Numo::DComplex] map of self.
*/
static VALUE dcomplex_map(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_map, FULL_LOOP, 1, 1, ain, aout };

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

static void iter_dcomplex_each_with_index(na_loop_t* const lp) {
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
static VALUE dcomplex_each_with_index(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_t ndf = { iter_dcomplex_each_with_index, FULL_LOOP_NIP, 1, 0, ain, 0 };

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

static void iter_dcomplex_map_with_index(na_loop_t* const lp) {
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
static VALUE dcomplex_map_with_index(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { Qnil, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_map_with_index, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop_with_index(&ndf, 1, self);
}

static void iter_dcomplex_abs(na_loop_t* const lp) {
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
    @return [Numo::DFloat] abs of self.
*/
static VALUE dcomplex_abs(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_abs, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

#define check_intdivzero(y)                                                                    \
  {}

static void iter_dcomplex_add(na_loop_t* const lp) {
  size_t i = 0;
  size_t n;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);

  //
  if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&
      is_aligned(p3, sizeof(dtype))) {

    if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {
      if (p1 == p3) { // inplace case
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p1)[i] = m_add(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      } else {
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p3)[i] = m_add(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      }
      return;
    }

    if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&
        is_aligned_step(s3, sizeof(dtype))) {
      //

      if (s2 == 0) { // Broadcasting from scalar value.
        check_intdivzero(*(dtype*)p2);
        if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {
          if (p1 == p3) { // inplace case
            for (; i < n; i++) {
              ((dtype*)p1)[i] = m_add(((dtype*)p1)[i], *(dtype*)p2);
            }
          } else {
            for (; i < n; i++) {
              ((dtype*)p3)[i] = m_add(((dtype*)p1)[i], *(dtype*)p2);
            }
          }
        } else {
          for (i = 0; i < n; i++) {
            *(dtype*)p3 = m_add(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p3 += s3;
          }
        }
      } else {
        if (p1 == p3) { // inplace case
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p1 = m_add(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
          }
        } else {
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p3 = m_add(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
            p3 += s3;
          }
        }
      }

      return;
      //
    }
  }
  for (i = 0; i < n; i++) {
    dtype x, y, z;
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    check_intdivzero(y);
    z = m_add(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, z);
  }
  //
}
#undef check_intdivzero

static VALUE dcomplex_add_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_add, STRIDE_LOOP, 2, 1, ain, aout };

  return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary add.
  @overload + other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self + other
*/
static VALUE dcomplex_add(VALUE self, VALUE other) {

  VALUE klass, v;

  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_add_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, '+', 1, other);
  }
}

#define check_intdivzero(y)                                                                    \
  {}

static void iter_dcomplex_sub(na_loop_t* const lp) {
  size_t i = 0;
  size_t n;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);

  //
  if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&
      is_aligned(p3, sizeof(dtype))) {

    if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {
      if (p1 == p3) { // inplace case
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      } else {
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      }
      return;
    }

    if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&
        is_aligned_step(s3, sizeof(dtype))) {
      //

      if (s2 == 0) { // Broadcasting from scalar value.
        check_intdivzero(*(dtype*)p2);
        if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {
          if (p1 == p3) { // inplace case
            for (; i < n; i++) {
              ((dtype*)p1)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);
            }
          } else {
            for (; i < n; i++) {
              ((dtype*)p3)[i] = m_sub(((dtype*)p1)[i], *(dtype*)p2);
            }
          }
        } else {
          for (i = 0; i < n; i++) {
            *(dtype*)p3 = m_sub(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p3 += s3;
          }
        }
      } else {
        if (p1 == p3) { // inplace case
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p1 = m_sub(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
          }
        } else {
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p3 = m_sub(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
            p3 += s3;
          }
        }
      }

      return;
      //
    }
  }
  for (i = 0; i < n; i++) {
    dtype x, y, z;
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    check_intdivzero(y);
    z = m_sub(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, z);
  }
  //
}
#undef check_intdivzero

static VALUE dcomplex_sub_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_sub, STRIDE_LOOP, 2, 1, ain, aout };

  return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary sub.
  @overload - other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self - other
*/
static VALUE dcomplex_sub(VALUE self, VALUE other) {

  VALUE klass, v;

  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_sub_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, '-', 1, other);
  }
}

#define check_intdivzero(y)                                                                    \
  {}

static void iter_dcomplex_mul(na_loop_t* const lp) {
  size_t i = 0;
  size_t n;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);

  //
  if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&
      is_aligned(p3, sizeof(dtype))) {

    if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {
      if (p1 == p3) { // inplace case
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p1)[i] = m_mul(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      } else {
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p3)[i] = m_mul(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      }
      return;
    }

    if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&
        is_aligned_step(s3, sizeof(dtype))) {
      //

      if (s2 == 0) { // Broadcasting from scalar value.
        check_intdivzero(*(dtype*)p2);
        if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {
          if (p1 == p3) { // inplace case
            for (; i < n; i++) {
              ((dtype*)p1)[i] = m_mul(((dtype*)p1)[i], *(dtype*)p2);
            }
          } else {
            for (; i < n; i++) {
              ((dtype*)p3)[i] = m_mul(((dtype*)p1)[i], *(dtype*)p2);
            }
          }
        } else {
          for (i = 0; i < n; i++) {
            *(dtype*)p3 = m_mul(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p3 += s3;
          }
        }
      } else {
        if (p1 == p3) { // inplace case
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p1 = m_mul(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
          }
        } else {
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p3 = m_mul(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
            p3 += s3;
          }
        }
      }

      return;
      //
    }
  }
  for (i = 0; i < n; i++) {
    dtype x, y, z;
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    check_intdivzero(y);
    z = m_mul(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, z);
  }
  //
}
#undef check_intdivzero

static VALUE dcomplex_mul_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_mul, STRIDE_LOOP, 2, 1, ain, aout };

  return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary mul.
  @overload * other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self * other
*/
static VALUE dcomplex_mul(VALUE self, VALUE other) {

  VALUE klass, v;

  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_mul_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, '*', 1, other);
  }
}

#define check_intdivzero(y)                                                                    \
  {}

static void iter_dcomplex_div(na_loop_t* const lp) {
  size_t i = 0;
  size_t n;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);

  //
  if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&
      is_aligned(p3, sizeof(dtype))) {

    if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {
      if (p1 == p3) { // inplace case
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p1)[i] = m_div(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      } else {
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p3)[i] = m_div(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      }
      return;
    }

    if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&
        is_aligned_step(s3, sizeof(dtype))) {
      //

      if (s2 == 0) { // Broadcasting from scalar value.
        check_intdivzero(*(dtype*)p2);
        if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {
          if (p1 == p3) { // inplace case
            for (; i < n; i++) {
              ((dtype*)p1)[i] = m_div(((dtype*)p1)[i], *(dtype*)p2);
            }
          } else {
            for (; i < n; i++) {
              ((dtype*)p3)[i] = m_div(((dtype*)p1)[i], *(dtype*)p2);
            }
          }
        } else {
          for (i = 0; i < n; i++) {
            *(dtype*)p3 = m_div(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p3 += s3;
          }
        }
      } else {
        if (p1 == p3) { // inplace case
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p1 = m_div(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
          }
        } else {
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p3 = m_div(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
            p3 += s3;
          }
        }
      }

      return;
      //
    }
  }
  for (i = 0; i < n; i++) {
    dtype x, y, z;
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    check_intdivzero(y);
    z = m_div(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, z);
  }
  //
}
#undef check_intdivzero

static VALUE dcomplex_div_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_div, STRIDE_LOOP, 2, 1, ain, aout };

  return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary div.
  @overload / other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self / other
*/
static VALUE dcomplex_div(VALUE self, VALUE other) {

  VALUE klass, v;

  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_div_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, '/', 1, other);
  }
}

static void iter_dcomplex_pow(na_loop_t* const lp) {
  size_t i;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;
  dtype x, y;
  INIT_COUNTER(lp, i);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);
  for (; i--;) {
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    x = m_pow(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, x);
  }
}

static void iter_dcomplex_pow_int32(na_loop_t* const lp) {
  size_t i;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;
  dtype x;
  int32_t y;
  INIT_COUNTER(lp, i);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);
  for (; i--;) {
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, int32_t, y);
    x = m_pow_int(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, x);
  }
}

static VALUE dcomplex_pow_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_in_t ain_i[2] = { { cT, 0 }, { numo_cInt32, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_pow, STRIDE_LOOP, 2, 1, ain, aout };
  ndfunc_t ndf_i = { iter_dcomplex_pow_int32, STRIDE_LOOP, 2, 1, ain_i, aout };

  // fixme : use na.integer?
  if (FIXNUM_P(other) || rb_obj_is_kind_of(other, numo_cInt32)) {
    return na_ndloop(&ndf_i, 2, self, other);
  } else {
    return na_ndloop(&ndf, 2, self, other);
  }
}

/*
  Binary power.
  @overload ** other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self to the other-th power.
*/
static VALUE dcomplex_pow(VALUE self, VALUE other) {

  VALUE klass, v;
  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_pow_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, id_pow, 1, other);
  }
}

static void iter_dcomplex_minus(na_loop_t* const lp) {
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
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_minus(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
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
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_minus(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary minus.
  @overload -@
    @return [Numo::DComplex] minus of self.
*/
static VALUE dcomplex_minus(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_minus, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_reciprocal(na_loop_t* const lp) {
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
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_reciprocal(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
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
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_reciprocal(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary reciprocal.
  @overload reciprocal
    @return [Numo::DComplex] reciprocal of self.
*/
static VALUE dcomplex_reciprocal(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_reciprocal, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_sign(na_loop_t* const lp) {
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
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_sign(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
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
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_sign(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary sign.
  @overload sign
    @return [Numo::DComplex] sign of self.
*/
static VALUE dcomplex_sign(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_sign, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_square(na_loop_t* const lp) {
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
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_square(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
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
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_square(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary square.
  @overload square
    @return [Numo::DComplex] square of self.
*/
static VALUE dcomplex_square(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_square, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_conj(na_loop_t* const lp) {
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
        x = m_conj(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_conj(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_conj(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_conj(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
          //
          for (i = 0; i < n; i++) {
            *(dtype*)p2 = m_conj(*(dtype*)p1);
            p1 += s1;
            p2 += s2;
          }
          return;
          //
        }
      }
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_conj(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary conj.
  @overload conj
    @return [Numo::DComplex] conj of self.
*/
static VALUE dcomplex_conj(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_conj, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_im(na_loop_t* const lp) {
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
        x = m_im(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      for (i = 0; i < n; i++) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        x = m_im(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_im(x);
        SET_DATA_INDEX(p2, idx2, dtype, x);
      }
    } else {
      //
      if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype))) {
        if (s1 == sizeof(dtype) && s2 == sizeof(dtype)) {
          for (i = 0; i < n; i++) {
            ((dtype*)p2)[i] = m_im(((dtype*)p1)[i]);
          }
          return;
        }
        if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype))) {
          //
          for (i = 0; i < n; i++) {
            *(dtype*)p2 = m_im(*(dtype*)p1);
            p1 += s1;
            p2 += s2;
          }
          return;
          //
        }
      }
      for (i = 0; i < n; i++) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        x = m_im(x);
        SET_DATA_STRIDE(p2, s2, dtype, x);
      }
      //
    }
  }
}

/*
  Unary im.
  @overload im
    @return [Numo::DComplex] im of self.
*/
static VALUE dcomplex_im(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_im, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_real(na_loop_t* const lp) {
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
        y = m_real(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        y = m_real(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_real(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_real(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  }
}

/*
  real of self.
  @overload real
    @return [Numo::DFloat] real of self.
*/
static VALUE dcomplex_real(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_real, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_imag(na_loop_t* const lp) {
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
        y = m_imag(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        y = m_imag(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_imag(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_imag(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  }
}

/*
  imag of self.
  @overload imag
    @return [Numo::DFloat] imag of self.
*/
static VALUE dcomplex_imag(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_imag, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_arg(na_loop_t* const lp) {
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
        y = m_arg(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_INDEX(p1, idx1, dtype, x);
        y = m_arg(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_arg(x);
        SET_DATA_INDEX(p2, idx2, rtype, y);
      }
    } else {
      for (; i--;) {
        GET_DATA_STRIDE(p1, s1, dtype, x);
        y = m_arg(x);
        SET_DATA_STRIDE(p2, s2, rtype, y);
      }
    }
  }
}

/*
  arg of self.
  @overload arg
    @return [Numo::DFloat] arg of self.
*/
static VALUE dcomplex_arg(VALUE self) {
  ndfunc_arg_in_t ain[1] = { { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_arg, FULL_LOOP, 1, 1, ain, aout };

  return na_ndloop(&ndf, 1, self);
}

static void iter_dcomplex_set_imag(na_loop_t* const lp) {
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
        GET_DATA(p1 + *idx1, dtype, x);
        GET_DATA_INDEX(p2, idx2, rtype, y);
        x = m_set_imag(x, y);
        SET_DATA_INDEX(p1, idx1, dtype, x);
      }
    } else {
      for (; i--;) {
        GET_DATA(p1 + *idx1, dtype, x);
        GET_DATA_STRIDE(p2, s2, rtype, y);
        x = m_set_imag(x, y);
        SET_DATA_INDEX(p1, idx1, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA(p1, dtype, x);
        GET_DATA_INDEX(p2, idx2, rtype, y);
        x = m_set_imag(x, y);
        SET_DATA_STRIDE(p1, s1, dtype, x);
      }
    } else {
      for (; i--;) {
        GET_DATA(p1, dtype, x);
        GET_DATA_STRIDE(p2, s2, rtype, y);
        x = m_set_imag(x, y);
        SET_DATA_STRIDE(p1, s1, dtype, x);
      }
    }
  }
}

static VALUE dcomplex_set_imag(VALUE self, VALUE a1) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_set_imag, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, a1);
  return a1;
}

static void iter_dcomplex_set_real(na_loop_t* const lp) {
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
        GET_DATA(p1 + *idx1, dtype, x);
        GET_DATA_INDEX(p2, idx2, rtype, y);
        x = m_set_real(x, y);
        SET_DATA_INDEX(p1, idx1, dtype, x);
      }
    } else {
      for (; i--;) {
        GET_DATA(p1 + *idx1, dtype, x);
        GET_DATA_STRIDE(p2, s2, rtype, y);
        x = m_set_real(x, y);
        SET_DATA_INDEX(p1, idx1, dtype, x);
      }
    }
  } else {
    if (idx2) {
      for (; i--;) {
        GET_DATA(p1, dtype, x);
        GET_DATA_INDEX(p2, idx2, rtype, y);
        x = m_set_real(x, y);
        SET_DATA_STRIDE(p1, s1, dtype, x);
      }
    } else {
      for (; i--;) {
        GET_DATA(p1, dtype, x);
        GET_DATA_STRIDE(p2, s2, rtype, y);
        x = m_set_real(x, y);
        SET_DATA_STRIDE(p1, s1, dtype, x);
      }
    }
  }
}

static VALUE dcomplex_set_real(VALUE self, VALUE a1) {
  ndfunc_arg_in_t ain[2] = { { OVERWRITE, 0 }, { cRT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_set_real, FULL_LOOP, 2, 0, ain, 0 };

  na_ndloop(&ndf, 2, self, a1);
  return a1;
}

#define check_intdivzero(y)                                                                    \
  {}

static void iter_dcomplex_copysign(na_loop_t* const lp) {
  size_t i = 0;
  size_t n;
  char *p1, *p2, *p3;
  ssize_t s1, s2, s3;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  INIT_PTR(lp, 1, p2, s2);
  INIT_PTR(lp, 2, p3, s3);

  //
  if (is_aligned(p1, sizeof(dtype)) && is_aligned(p2, sizeof(dtype)) &&
      is_aligned(p3, sizeof(dtype))) {

    if (s1 == sizeof(dtype) && s2 == sizeof(dtype) && s3 == sizeof(dtype)) {
      if (p1 == p3) { // inplace case
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p1)[i] = m_copysign(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      } else {
        for (; i < n; i++) {
          check_intdivzero(((dtype*)p2)[i]);
          ((dtype*)p3)[i] = m_copysign(((dtype*)p1)[i], ((dtype*)p2)[i]);
        }
      }
      return;
    }

    if (is_aligned_step(s1, sizeof(dtype)) && is_aligned_step(s2, sizeof(dtype)) &&
        is_aligned_step(s3, sizeof(dtype))) {
      //

      if (s2 == 0) { // Broadcasting from scalar value.
        check_intdivzero(*(dtype*)p2);
        if (s1 == sizeof(dtype) && s3 == sizeof(dtype)) {
          if (p1 == p3) { // inplace case
            for (; i < n; i++) {
              ((dtype*)p1)[i] = m_copysign(((dtype*)p1)[i], *(dtype*)p2);
            }
          } else {
            for (; i < n; i++) {
              ((dtype*)p3)[i] = m_copysign(((dtype*)p1)[i], *(dtype*)p2);
            }
          }
        } else {
          for (i = 0; i < n; i++) {
            *(dtype*)p3 = m_copysign(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p3 += s3;
          }
        }
      } else {
        if (p1 == p3) { // inplace case
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p1 = m_copysign(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
          }
        } else {
          for (i = 0; i < n; i++) {
            check_intdivzero(*(dtype*)p2);
            *(dtype*)p3 = m_copysign(*(dtype*)p1, *(dtype*)p2);
            p1 += s1;
            p2 += s2;
            p3 += s3;
          }
        }
      }

      return;
      //
    }
  }
  for (i = 0; i < n; i++) {
    dtype x, y, z;
    GET_DATA_STRIDE(p1, s1, dtype, x);
    GET_DATA_STRIDE(p2, s2, dtype, y);
    check_intdivzero(y);
    z = m_copysign(x, y);
    SET_DATA_STRIDE(p3, s3, dtype, z);
  }
  //
}
#undef check_intdivzero

static VALUE dcomplex_copysign_self(VALUE self, VALUE other) {
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { cT, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_copysign, STRIDE_LOOP, 2, 1, ain, aout };

  return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary copysign.
  @overload copysign other
    @param [Numo::NArray,Numeric] other
    @return [Numo::NArray] self copysign other
*/
static VALUE dcomplex_copysign(VALUE self, VALUE other) {

  VALUE klass, v;

  klass = na_upcast(rb_obj_class(self), rb_obj_class(other));
  if (klass == cT) {
    return dcomplex_copysign_self(self, other);
  } else {
    v = rb_funcall(klass, id_cast, 1, self);
    return rb_funcall(v, id_copysign, 1, other);
  }
}

static void iter_dcomplex_kahan_sum(na_loop_t* const lp) {
  size_t n;
  char *p1, *p2;
  ssize_t s1;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  p2 = lp->args[1].ptr + lp->args[1].iter[0].pos;

  *(dtype*)p2 = f_kahan_sum(n, p1, s1);
}
static void iter_dcomplex_kahan_sum_nan(na_loop_t* const lp) {
  size_t n;
  char *p1, *p2;
  ssize_t s1;

  INIT_COUNTER(lp, n);
  INIT_PTR(lp, 0, p1, s1);
  p2 = lp->args[1].ptr + lp->args[1].iter[0].pos;

  *(dtype*)p2 = f_kahan_sum_nan(n, p1, s1);
}

/*
  kahan_sum of self.
  @overload kahan_sum(axis:nil, keepdims:false, nan:false)
    @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN for sum/mean etc, or,
    return NaN for min/max etc).
    @param [Numeric,Array,Range] axis  Performs kahan_sum along the axis.
    @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
    dimensions with size one.
    @return [Numo::DComplex] returns result of kahan_sum.
*/
static VALUE dcomplex_kahan_sum(int argc, VALUE* argv, VALUE self) {
  VALUE v, reduce;
  ndfunc_arg_in_t ain[2] = { { cT, 0 }, { sym_reduce, 0 } };
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = {
    iter_dcomplex_kahan_sum, STRIDE_LOOP_NIP | NDF_FLAT_REDUCE, 2, 1, ain, aout
  };

  reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, iter_dcomplex_kahan_sum_nan);

  v = na_ndloop(&ndf, 2, self, reduce);

  return dcomplex_extract(v);
}

typedef struct {
  dtype mu;
  rtype sigma;
} randn_opt_t;

static void iter_dcomplex_rand_norm(na_loop_t* const lp) {
  size_t i;
  char* p1;
  ssize_t s1;
  size_t* idx1;

  dtype* a0;

  dtype mu;
  rtype sigma;
  randn_opt_t* g;

  INIT_COUNTER(lp, i);
  INIT_PTR_IDX(lp, 0, p1, s1, idx1);
  g = (randn_opt_t*)(lp->opt_ptr);
  mu = g->mu;
  sigma = g->sigma;

  if (idx1) {

    for (; i--;) {
      a0 = (dtype*)(p1 + *idx1);
      m_rand_norm(mu, sigma, a0);
      idx1 += 1;
    }

  } else {

    for (; i--;) {
      a0 = (dtype*)(p1);
      m_rand_norm(mu, sigma, a0);
      p1 += s1;
    }
  }
}

/*
  Generates random numbers from the normal distribution on self narray
  using Box-Muller Transformation.
  @overload rand_norm([mu,[sigma]])
    @param [Numeric] mu  mean of normal distribution. (default=0)
    @param [Numeric] sigma  standard deviation of normal distribution. (default=1)
    @return [Numo::DComplex] self.
  @example
    Numo::DFloat.new(5,5).rand_norm
    # => Numo::DFloat#shape=[5,5]
    # [[-0.581255, -0.168354, 0.586895, -0.595142, -0.802802],
    #  [-0.326106, 0.282922, 1.68427, 0.918499, -0.0485384],
    #  [-0.464453, -0.992194, 0.413794, -0.60717, -0.699695],
    #  [-1.64168, 0.48676, -0.875871, -1.43275, 0.812172],
    #  [-0.209975, -0.103612, -0.878617, -1.42495, 1.0968]]

    Numo::DFloat.new(5,5).rand_norm(10,0.1)
    # => Numo::DFloat#shape=[5,5]
    # [[9.9019, 9.90339, 10.0826, 9.98384, 9.72861],
    #  [9.81507, 10.0272, 9.91445, 10.0568, 9.88923],
    #  [10.0234, 9.97874, 9.96011, 9.9006, 9.99964],
    #  [10.0186, 9.94598, 9.92236, 9.99811, 9.97003],
    #  [9.79266, 9.95044, 9.95212, 9.93692, 10.2027]]

    Numo::DComplex.new(3,3).rand_norm(5+5i)
    # => Numo::DComplex#shape=[3,3]
    # [[5.84303+4.40052i, 4.00984+6.08982i, 5.10979+5.13215i],
    #  [4.26477+3.99655i, 4.90052+5.00763i, 4.46607+2.3444i],
    #  [4.5528+7.11003i, 5.62117+6.69094i, 5.05443+5.35133i]]
*/
static VALUE dcomplex_rand_norm(int argc, VALUE* argv, VALUE self) {
  int n;
  randn_opt_t g;
  VALUE v1 = Qnil, v2 = Qnil;
  ndfunc_arg_in_t ain[1] = { { OVERWRITE, 0 } };
  ndfunc_t ndf = { iter_dcomplex_rand_norm, FULL_LOOP, 1, 0, ain, 0 };

  n = rb_scan_args(argc, argv, "02", &v1, &v2);
  if (n == 0) {
    g.mu = m_zero;
  } else {
    g.mu = m_num_to_data(v1);
  }
  if (n == 2) {
    g.sigma = NUM2DBL(v2);
  } else {
    g.sigma = 1;
  }
  na_ndloop3(&ndf, &g, 1, self);
  return self;
}

static void iter_dcomplex_poly(na_loop_t* const lp) {
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
    @return [Numo::DComplex]
*/
static VALUE dcomplex_poly(VALUE self, VALUE args) {
  int argc, i;
  VALUE* argv;
  volatile VALUE v, a;
  ndfunc_arg_out_t aout[1] = { { cT, 0 } };
  ndfunc_t ndf = { iter_dcomplex_poly, NO_LOOP, 0, 1, 0, aout };

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
  return dcomplex_extract(v);
}

VALUE mTM;

void Init_numo_dcomplex(void) {
  VALUE hCast, mNumo;

  mNumo = rb_define_module("Numo");

  id_pow = rb_intern("**");
  id_cast = rb_intern("cast");
  id_copysign = rb_intern("copysign");
  id_eq = rb_intern("eq");
  id_imag = rb_intern("imag");
  id_mulsum = rb_intern("mulsum");
  id_ne = rb_intern("ne");
  id_nearly_eq = rb_intern("nearly_eq");
  id_real = rb_intern("real");
  id_to_a = rb_intern("to_a");

  /**
   * Document-class: Numo::DComplex
   *
   * Double precision floating point complex number N-dimensional array class.
   */
  cT = rb_define_class_under(mNumo, "DComplex", cNArray);

  // alias of DComplex
  rb_define_const(mNumo, "Complex64", numo_cDComplex);

  hCast = rb_hash_new();
  /* Upcasting rules of DComplex. */
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
  rb_hash_aset(hCast, numo_cRObject, numo_cRObject);
  rb_hash_aset(hCast, numo_cDComplex, numo_cDComplex);
  rb_hash_aset(hCast, numo_cSComplex, numo_cDComplex);
  rb_hash_aset(hCast, numo_cDFloat, numo_cDComplex);
  rb_hash_aset(hCast, numo_cSFloat, numo_cDComplex);
  rb_hash_aset(hCast, numo_cInt64, numo_cDComplex);
  rb_hash_aset(hCast, numo_cInt32, numo_cDComplex);
  rb_hash_aset(hCast, numo_cInt16, numo_cDComplex);
  rb_hash_aset(hCast, numo_cInt8, numo_cDComplex);
  rb_hash_aset(hCast, numo_cUInt64, numo_cDComplex);
  rb_hash_aset(hCast, numo_cUInt32, numo_cDComplex);
  rb_hash_aset(hCast, numo_cUInt16, numo_cDComplex);
  rb_hash_aset(hCast, numo_cUInt8, numo_cDComplex);
  rb_obj_freeze(hCast);

  /* Element size of DComplex in bits. */
  rb_define_const(cT, "ELEMENT_BIT_SIZE", INT2FIX(sizeof(dtype) * 8));
  /* Element size of DComplex in bytes. */
  rb_define_const(cT, "ELEMENT_BYTE_SIZE", INT2FIX(sizeof(dtype)));
  /* Stride size of contiguous DComplex array. */
  rb_define_const(cT, "CONTIGUOUS_STRIDE", INT2FIX(sizeof(dtype)));
  /* Machine epsilon of DComplex. */
  rb_define_const(cT, "EPSILON", M_EPSILON);
  /* The largest representable value of DComplex. */
  rb_define_const(cT, "MAX", M_MAX);
  /* The smallest representable value of DComplex. */
  rb_define_const(cT, "MIN", M_MIN);
  rb_define_alloc_func(cT, dcomplex_s_alloc_func);
  rb_define_method(cT, "allocate", dcomplex_allocate, 0);
  rb_define_method(cT, "extract", dcomplex_extract, 0);

  rb_define_method(cT, "store", dcomplex_store, 1);

  rb_define_singleton_method(cT, "cast", dcomplex_s_cast, 1);
  rb_define_method(cT, "[]", dcomplex_aref, -1);
  rb_define_method(cT, "[]=", dcomplex_aset, -1);
  rb_define_method(cT, "coerce_cast", dcomplex_coerce_cast, 1);
  /**
   * Convert self to Array.
   * @overload to_a
   *   @return [Array]
   */
  rb_define_method(cT, "to_a", dcomplex_to_a, 0);
  rb_define_method(cT, "fill", dcomplex_fill, 1);
  rb_define_method(cT, "format", dcomplex_format, -1);
  rb_define_method(cT, "format_to_a", dcomplex_format_to_a, -1);
  rb_define_method(cT, "inspect", dcomplex_inspect, 0);
  rb_define_method(cT, "each", dcomplex_each, 0);
  rb_define_method(cT, "map", dcomplex_map, 0);
  rb_define_method(cT, "each_with_index", dcomplex_each_with_index, 0);
  rb_define_method(cT, "map_with_index", dcomplex_map_with_index, 0);
  rb_define_method(cT, "abs", dcomplex_abs, 0);
  rb_define_method(cT, "+", dcomplex_add, 1);
  rb_define_method(cT, "-", dcomplex_sub, 1);
  rb_define_method(cT, "*", dcomplex_mul, 1);
  rb_define_method(cT, "/", dcomplex_div, 1);
  rb_define_method(cT, "**", dcomplex_pow, 1);
  rb_define_alias(cT, "pow", "**");
  rb_define_method(cT, "-@", dcomplex_minus, 0);
  rb_define_method(cT, "reciprocal", dcomplex_reciprocal, 0);
  rb_define_method(cT, "sign", dcomplex_sign, 0);
  rb_define_method(cT, "square", dcomplex_square, 0);
  rb_define_method(cT, "conj", dcomplex_conj, 0);
  rb_define_method(cT, "im", dcomplex_im, 0);
  rb_define_method(cT, "real", dcomplex_real, 0);
  rb_define_method(cT, "imag", dcomplex_imag, 0);
  rb_define_method(cT, "arg", dcomplex_arg, 0);
  rb_define_alias(cT, "angle", "arg");
  rb_define_method(cT, "set_imag", dcomplex_set_imag, 1);
  rb_define_method(cT, "set_real", dcomplex_set_real, 1);
  rb_define_alias(cT, "imag=", "set_imag");
  rb_define_alias(cT, "real=", "set_real");
  rb_define_alias(cT, "conjugate", "conj");
  /**
   * Comparison eq other.
   * @overload eq other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self eq other.
   */
  rb_define_method(cT, "eq", dcomplex_eq, 1);
  /**
   * Comparison ne other.
   * @overload ne other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self ne other.
   */
  rb_define_method(cT, "ne", dcomplex_ne, 1);
  /**
   * Comparison nearly_eq other.
   * @overload nearly_eq other
   *   @param [Numo::NArray,Numeric] other
   *   @return [Numo::Bit] result of self nearly_eq other.
   */
  rb_define_method(cT, "nearly_eq", dcomplex_nearly_eq, 1);
  rb_define_alias(cT, "close_to", "nearly_eq");
  /**
   * Unary floor.
   * @overload floor
   *   @return [Numo::DComplex] floor of self.
   */
  rb_define_method(cT, "floor", dcomplex_floor, 0);
  /**
   * Unary round.
   * @overload round
   *   @return [Numo::DComplex] round of self.
   */
  rb_define_method(cT, "round", dcomplex_round, 0);
  /**
   * Unary ceil.
   * @overload ceil
   *   @return [Numo::DComplex] ceil of self.
   */
  rb_define_method(cT, "ceil", dcomplex_ceil, 0);
  /**
   * Unary trunc.
   * @overload trunc
   *   @return [Numo::DComplex] trunc of self.
   */
  rb_define_method(cT, "trunc", dcomplex_trunc, 0);
  /**
   * Unary rint.
   * @overload rint
   *   @return [Numo::DComplex] rint of self.
   */
  rb_define_method(cT, "rint", dcomplex_rint, 0);
  rb_define_method(cT, "copysign", dcomplex_copysign, 1);
  /**
   * Condition of isnan.
   * @overload isnan
   *   @return [Numo::Bit] Condition of isnan.
   */
  rb_define_method(cT, "isnan", dcomplex_isnan, 0);
  /**
   * Condition of isinf.
   * @overload isinf
   *   @return [Numo::Bit] Condition of isinf.
   */
  rb_define_method(cT, "isinf", dcomplex_isinf, 0);
  /**
   * Condition of isposinf.
   * @overload isposinf
   *   @return [Numo::Bit] Condition of isposinf.
   */
  rb_define_method(cT, "isposinf", dcomplex_isposinf, 0);
  /**
   * Condition of isneginf.
   * @overload isneginf
   *   @return [Numo::Bit] Condition of isneginf.
   */
  rb_define_method(cT, "isneginf", dcomplex_isneginf, 0);
  /**
   * Condition of isfinite.
   * @overload isfinite
   *   @return [Numo::Bit] Condition of isfinite.
   */
  rb_define_method(cT, "isfinite", dcomplex_isfinite, 0);
  /**
   * sum of self.
   * @overload sum(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs sum along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::DComplex] returns result of sum.
   */
  rb_define_method(cT, "sum", dcomplex_sum, -1);
  /**
   * prod of self.
   * @overload prod(axis:nil, keepdims:false, nan:false)
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or, return NaN for min/max etc).
   *   @param [Numeric,Array,Range] axis  Performs prod along the axis.
   *   @param [TrueClass] keepdims  If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @return [Numo::DComplex] returns result of prod.
   */
  rb_define_method(cT, "prod", dcomplex_prod, -1);
  rb_define_method(cT, "kahan_sum", dcomplex_kahan_sum, -1);
  /**
   * mean of self.
   * @overload mean(axis: nil, keepdims: false, nan: false)
   *   @param axis [Numeric, Array, Range] Performs mean along the axis.
   *   @param keepdims [Boolean] If true, the reduced axes are left in the result array as
   *     dimensions with size one.
   *   @param nan [Boolean] If true, apply NaN-aware algorithm
   *     (avoid NaN for sum/mean etc, or return NaN for min/max etc).
   * @return [Numo::DComplex] returns result of mean.
   */
  rb_define_method(cT, "mean", dcomplex_mean, -1);
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
  rb_define_method(cT, "var", dcomplex_var, -1);
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
  rb_define_method(cT, "stddev", dcomplex_stddev, -1);
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
  rb_define_method(cT, "rms", dcomplex_rms, -1);
  /**
   * cumsum of self.
   * @overload cumsum(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumsum along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::DComplex] cumsum of self.
   */
  rb_define_method(cT, "cumsum", dcomplex_cumsum, -1);
  /**
   * cumprod of self.
   * @overload cumprod(axis:nil, nan:false)
   *   @param [Numeric,Array,Range] axis  Performs cumprod along the axis.
   *   @param [TrueClass] nan  If true, apply NaN-aware algorithm (avoid NaN if exists).
   *   @return [Numo::DComplex] cumprod of self.
   */
  rb_define_method(cT, "cumprod", dcomplex_cumprod, -1);
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
  rb_define_method(cT, "mulsum", dcomplex_mulsum, -1);
  /**
   * Set linear sequence of numbers to self. The sequence is obtained from
   *    beg+i*step
   * where i is 1-dimensional index.
   * @overload seq([beg,[step]])
   *   @param [Numeric] beg  beginning of sequence. (default=0)
   *   @param [Numeric] step  step of sequence. (default=1)
   *   @return [Numo::DComplex] self.
   * @example
   *   Numo::DFloat.new(6).seq(1,-0.2)
   *   # => Numo::DFloat#shape=[6]
   *   # [1, 0.8, 0.6, 0.4, 0.2, 0]
   *
   *   Numo::DComplex.new(6).seq(1,-0.2+0.2i)
   *   # => Numo::DComplex#shape=[6]
   *   # [1+0i, 0.8+0.2i, 0.6+0.4i, 0.4+0.6i, 0.2+0.8i, 0+1i]
   */
  rb_define_method(cT, "seq", dcomplex_seq, -1);
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
   *   @return [Numo::DComplex] self.
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
  rb_define_method(cT, "logseq", dcomplex_logseq, -1);
  /**
   * Eye: Set a value to diagonal components, set 0 to non-diagonal components.
   * @overload eye([element,offset])
   *   @param [Numeric] element  Diagonal element to be stored. Default is 1.
   *   @param [Integer] offset Diagonal offset from the main diagonal.  The
   *       default is 0. k>0 for diagonals above the main diagonal, and k<0
   *       for diagonals below the main diagonal.
   *   @return [Numo::DComplex] eye of self.
   */
  rb_define_method(cT, "eye", dcomplex_eye, -1);
  rb_define_alias(cT, "indgen", "seq");
  /**
   * Generate uniformly distributed random numbers on self narray.
   * @overload rand([[low],high])
   *   @param [Numeric] low  lower inclusive boundary of random numbers. (default=0)
   *   @param [Numeric] high  upper exclusive boundary of random numbers.
   *     (default=1 or 1+1i for complex types)
   *   @return [Numo::DComplex] self.
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
  rb_define_method(cT, "rand", dcomplex_rand, -1);
  rb_define_method(cT, "rand_norm", dcomplex_rand_norm, -1);
  rb_define_method(cT, "poly", dcomplex_poly, -2);
  rb_define_singleton_method(cT, "[]", dcomplex_s_cast, -2);

  /**
   * Document-module: Numo::DComplex::Math
   *
   * This module contains mathematical functions for Numo::DComplex.
   */
  mTM = rb_define_module_under(cT, "Math");
  /**
   * Calculate sqrt(x).
   * @overload sqrt(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of sqrt(x).
   */
  rb_define_module_function(mTM, "sqrt", dcomplex_math_s_sqrt, 1);
  /**
   * Calculate cbrt(x).
   * @overload cbrt(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of cbrt(x).
   */
  rb_define_module_function(mTM, "cbrt", dcomplex_math_s_cbrt, 1);
  /**
   * Calculate log(x).
   * @overload log(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of log(x).
   */
  rb_define_module_function(mTM, "log", dcomplex_math_s_log, 1);
  /**
   * Calculate log2(x).
   * @overload log2(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of log2(x).
   */
  rb_define_module_function(mTM, "log2", dcomplex_math_s_log2, 1);
  /**
   * Calculate log10(x).
   * @overload log10(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of log10(x).
   */
  rb_define_module_function(mTM, "log10", dcomplex_math_s_log10, 1);
  /**
   * Calculate exp(x).
   * @overload exp(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of exp(x).
   */
  rb_define_module_function(mTM, "exp", dcomplex_math_s_exp, 1);
  /**
   * Calculate exp2(x).
   * @overload exp2(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of exp2(x).
   */
  rb_define_module_function(mTM, "exp2", dcomplex_math_s_exp2, 1);
  /**
   * Calculate exp10(x).
   * @overload exp10(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of exp10(x).
   */
  rb_define_module_function(mTM, "exp10", dcomplex_math_s_exp10, 1);
  /**
   * Calculate sin(x).
   * @overload sin(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of sin(x).
   */
  rb_define_module_function(mTM, "sin", dcomplex_math_s_sin, 1);
  /**
   * Calculate cos(x).
   * @overload cos(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of cos(x).
   */
  rb_define_module_function(mTM, "cos", dcomplex_math_s_cos, 1);
  /**
   * Calculate tan(x).
   * @overload tan(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of tan(x).
   */
  rb_define_module_function(mTM, "tan", dcomplex_math_s_tan, 1);
  /**
   * Calculate asin(x).
   * @overload asin(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of asin(x).
   */
  rb_define_module_function(mTM, "asin", dcomplex_math_s_asin, 1);
  /**
   * Calculate acos(x).
   * @overload acos(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of acos(x).
   */
  rb_define_module_function(mTM, "acos", dcomplex_math_s_acos, 1);
  /**
   * Calculate atan(x).
   * @overload atan(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of atan(x).
   */
  rb_define_module_function(mTM, "atan", dcomplex_math_s_atan, 1);
  /**
   * Calculate sinh(x).
   * @overload sinh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of sinh(x).
   */
  rb_define_module_function(mTM, "sinh", dcomplex_math_s_sinh, 1);
  /**
   * Calculate cosh(x).
   * @overload cosh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of cosh(x).
   */
  rb_define_module_function(mTM, "cosh", dcomplex_math_s_cosh, 1);
  /**
   * Calculate tanh(x).
   * @overload tanh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of tanh(x).
   */
  rb_define_module_function(mTM, "tanh", dcomplex_math_s_tanh, 1);
  /**
   * Calculate asinh(x).
   * @overload asinh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of asinh(x).
   */
  rb_define_module_function(mTM, "asinh", dcomplex_math_s_asinh, 1);
  /**
   * Calculate acosh(x).
   * @overload acosh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of acosh(x).
   */
  rb_define_module_function(mTM, "acosh", dcomplex_math_s_acosh, 1);
  /**
   * Calculate atanh(x).
   * @overload atanh(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of atanh(x).
   */
  rb_define_module_function(mTM, "atanh", dcomplex_math_s_atanh, 1);
  /**
   * Calculate sinc(x).
   * @overload sinc(x)
   *   @param [Numo::NArray,Numeric] x  input value
   *   @return [Numo::DComplex] result of sinc(x).
   */
  rb_define_module_function(mTM, "sinc", dcomplex_math_s_sinc, 1);

  //  how to do this?
  // rb_extend_object(cT, mTM);
}
