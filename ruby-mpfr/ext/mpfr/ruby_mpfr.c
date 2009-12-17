#include "ruby_mpfr.h"

static ID eqq, to_s, new, class, method_defined, object_id;
static VALUE __mpfr_class__, __sym_to_s__, __sym_to_str__;

/* ------------------------------ Precision and Rounding Mode Start ------------------------------ */
#define VALID_RND(rnd) (rnd >= 0 && rnd <= 3)
#define SPECIAL_FUNC_STATE "@@special_func_state"

/* Convert VALUE rnd (rounding mode number) to C integer and */
/* return it if it is valid as rounding mode number. */
mp_rnd_t r_mpfr_rnd_from_value(VALUE rnd)
{
  mp_rnd_t r = (mp_rnd_t)NUM2INT(rnd);
  if(!VALID_RND(r)){
    rb_raise(rb_eArgError, "Argument must be Rounding Mode.");
  }
  return r;
}

/* If argc equals max, convert last argument to rounding mode number. */
/* Otherwise, return defoult rounding mode number. */
mp_rnd_t r_mpfr_rnd_from_optional_argument(int min, int max, int argc, VALUE *argv)
{
  mp_rnd_t rnd;
  if(argc >= min && argc <= max){
    if(argc == max){
      rnd = r_mpfr_rnd_from_value(argv[max-1]);
    }else{
      rnd = mpfr_get_default_rounding_mode();
    }
  }else{
    rb_raise(rb_eArgError, "Invalid number of arguments.");
  }
  return rnd;
}

/* If argc equals max, convert last argument to precision number. */
/* Otherwise, return defoult precision number. */
mp_rnd_t r_mpfr_prec_from_optional_argument(int min, int max, int argc, VALUE *argv)
{
  mp_prec_t prec;
  if(argc >= min && argc <= max){
    if(argc == max){
      prec = NUM2INT(argv[max - 1]);
    }else{
      prec = mpfr_get_default_prec();
    }
  }else{
    rb_raise(rb_eArgError, "Invalid number of arguments.");
  }
  return prec;
}

/* min is a minimum number of arguments. */
/* max is a maximum number of arguments. */
void r_mpfr_get_rnd_prec_from_optional_arguments(mp_rnd_t *rnd, mp_prec_t *prec, int min, int max,
							int argc, VALUE *argv)
{
  if(argc >= min && argc <= max){
    if(argc >= max - 1){
      *rnd = r_mpfr_rnd_from_value(argv[max - 2]);
    }else{
      *rnd = mpfr_get_default_rounding_mode();
    }
    if(argc == max){
      *prec = NUM2INT(argv[max - 1]);
    }else{
      *prec = mpfr_get_default_prec();
    }
  }else{
    rb_raise(rb_eArgError, "Invalid number of arguments.");
  }
}


/* Set the default MPFR precision in bits. */
static VALUE r_mpfr_set_default_prec(VALUE self, VALUE prec)
{
  int set_prec = NUM2INT(prec);
  if(set_prec <= 0){
    rb_raise(rb_eRangeError, "Argument must be positive.");
  }
  mpfr_set_default_prec(set_prec);
  return INT2FIX(mpf_get_default_prec());
}

/* Return the default MPFR precision in bits. */
static VALUE r_mpfr_get_default_prec(VALUE self)
{
  return INT2NUM((int)mpfr_get_default_prec());
}

/* Set the default rounding mode. The default rounding mode is MPFR::RNDN. */
static VALUE r_mpfr_set_default_rounding_mode(VALUE self, VALUE rnd)
{
  mp_rnd_t a = NUM2INT(rnd);
  if(VALID_RND(a)){
    mpfr_set_default_rounding_mode(a);
  }else{
    rb_raise(rb_eArgError, "Argument must be Rounding Mode.");
  }
  return INT2FIX(mpfr_get_default_rounding_mode());
}

/* Get the default rounding mode. */
static VALUE r_mpfr_get_default_rounding_mode(VALUE self)
{
  return INT2NUM(mpfr_get_default_rounding_mode());
}

/* ------------------------------ Precision and Rounding Mode End ------------------------------ */

/* ------------------------------ Exception Related Functions Start ------------------------------ */

/* Return integer which is mpfr_get_emin(). */
static VALUE r_mpfr_get_emin(VALUE self)
{
  return INT2NUM(mpfr_get_emin());
}

/* Return integer which is mpfr_get_emax(). */
static VALUE r_mpfr_get_emax(VALUE self)
{
  return INT2NUM(mpfr_get_emax());
}

/* Return integer which is mpfr_set_emin( p1 ). */
static VALUE r_mpfr_set_emin(VALUE self, VALUE exp)
{
  return INT2NUM(mpfr_set_emin(NUM2INT(exp)));
}

/* Return integer which is mpfr_set_emax( p1 ). */
static VALUE r_mpfr_set_emax(VALUE self, VALUE exp)
{
  return INT2NUM(mpfr_set_emax(NUM2INT(exp)));
}

/* Return integer which is mpfr_get_emin_min(). */
static VALUE r_mpfr_get_emin_min(VALUE self)
{
  return INT2NUM(mpfr_get_emin_min());
}

/* Return integer which is mpfr_get_emin_max(). */
static VALUE r_mpfr_get_emin_max(VALUE self)
{
  return INT2NUM(mpfr_get_emin_max());
}

/* Return integer which is mpfr_get_emax_min(). */
static VALUE r_mpfr_get_emax_min(VALUE self)
{
  return INT2NUM(mpfr_get_emax_min());
}

/* Return integer which is mpfr_get_emax_max(). */
static VALUE r_mpfr_get_emax_max(VALUE self)
{
  return INT2NUM(mpfr_get_emax_max());
}

/* Execute mpfr_clear_underflow() and return nil. */
static VALUE r_mpfr_clear_underflow(VALUE self)
{
  mpfr_clear_underflow();
  return Qnil;
}

/* Execute mpfr_clear_overflow() and return nil. */
static VALUE r_mpfr_clear_overflow(VALUE self)
{
  mpfr_clear_overflow();
  return Qnil;
}

/* Execute mpfr_clear_nanflag() and return nil. */
static VALUE r_mpfr_clear_nanflag(VALUE self)
{
  mpfr_clear_nanflag();
  return Qnil;
}

/* Execute mpfr_clear_inexflag() and return nil. */
static VALUE r_mpfr_clear_inexflag(VALUE self)
{
  mpfr_clear_inexflag();
  return Qnil;
}

/* Execute mpfr_clear_erangeflag() and return nil. */
static VALUE r_mpfr_clear_erangeflag(VALUE self)
{
  mpfr_clear_erangeflag();
  return Qnil;
}

/* Execute mpfr_set_underflow() and return nil. */
static VALUE r_mpfr_set_underflow(VALUE self)
{
  mpfr_set_underflow();
  return Qnil;
}

/* Execute mpfr_set_overflow() and return nil. */
static VALUE r_mpfr_set_overflow(VALUE self)
{
  mpfr_set_overflow();
  return Qnil;
}

/* Execute mpfr_set_nanflag() and return nil. */
static VALUE r_mpfr_set_nanflag(VALUE self)
{
  mpfr_set_nanflag();
  return Qnil;
}

/* Execute mpfr_set_inexflag() and return nil. */
static VALUE r_mpfr_set_inexflag(VALUE self)
{
  mpfr_set_inexflag();
  return Qnil;
}

/* Execute mpfr_set_erangeflag() and return nil. */
static VALUE r_mpfr_set_erangeflag(VALUE self)
{
  mpfr_set_erangeflag();
  return Qnil;
}

/* Execute mpfr_clear_flags() and return nil. */
static VALUE r_mpfr_clear_flags(VALUE self)
{
  mpfr_clear_flags();
  return Qnil;
}

/* If underflow flag is set, this method returns true. Otherwise nil. */
static VALUE r_mpfr_underflow_p(VALUE self)
{
  return (mpfr_underflow_p() != 0 ? Qtrue : Qfalse);
}

/* If owerflow flag is set, this method returns true. Otherwise nil. */
static VALUE r_mpfr_overflow_p(VALUE self)
{
  return (mpfr_overflow_p() != 0 ? Qtrue : Qfalse);
}

/* If invalid flag is set, this method returns true. Otherwise nil. */
static VALUE r_mpfr_nanflag_p(VALUE self)
{
  return (mpfr_nanflag_p() != 0 ? Qtrue : Qfalse);
}

/* If inexact flag is set, this method returns true. Otherwise nil. */
static VALUE r_mpfr_inexflag_p(VALUE self)
{
  return (mpfr_inexflag_p() != 0 ? Qtrue : Qfalse);
}

/* If erange flag is set, this method returns true. Otherwise nil. */
static VALUE r_mpfr_erangeflag_p(VALUE self)
{
  return (mpfr_erangeflag_p() != 0 ? Qtrue : Qfalse);
}

/* Execute mpfr_check_range( self, p1, rnd ) and return self. */
static VALUE r_mpfr_check_range(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd = r_mpfr_rnd_from_optional_argument(1, 2, argc, argv);
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_check_range(ptr_self, NUM2INT(argv[0]), rnd);
  return self;
}

/* Execute mpfr_subnormalize( self, p1, rnd ) and return self. */
static VALUE r_mpfr_subnormalize(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd = r_mpfr_rnd_from_optional_argument(1, 2, argc, argv);
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_subnormalize(ptr_self, NUM2INT(argv[0]), rnd);
  return self;
}

/* ------------------------------ Exception Related Functions End ------------------------------ */

/* ------------------------------ MPFR allocation Start ------------------------------ */

void r_mpfr_free(void *ptr)
{
  mpfr_clear(ptr);
  free(ptr);
}

VALUE r_mpfr_make_new_fr_obj(MPFR *ptr)
{
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  mpfr_set(ptr_ret, ptr, mpfr_get_default_prec());
  return ret;
}

VALUE r_mpfr_make_new_fr_obj2(MPFR *ptr, int prec)
{
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(ret, ptr_ret, prec);
  mpfr_set(ptr_ret, ptr, prec);
  return ret;
}

static void r_mpfr_convert_to_str_set(MPFR *ptr, VALUE obj, mp_rnd_t rnd)
{
  if(RTEST(rb_funcall(rb_funcall(obj, class, 0), method_defined, 1, __sym_to_str__))){
    char *str = StringValuePtr(obj);
    mpfr_set_str(ptr, str, 10, rnd);
  }else if(RTEST(rb_funcall(rb_funcall(obj, class, 0), method_defined, 1, __sym_to_s__))){
    volatile VALUE tmp = rb_funcall(obj, to_s, 0);
    char *str = StringValuePtr(tmp);
    mpfr_set_str(ptr, str, 10, rnd);
  }
}

void r_mpfr_set_robj(MPFR *ptr, VALUE obj, mp_rnd_t rnd)
{
  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, obj))){
    MPFR *ptr_obj;
    r_mpfr_get_struct(ptr_obj, obj);
    mpfr_set(ptr, ptr_obj, rnd);
  }else{
    switch(TYPE(obj)){
    case T_STRING:
      mpfr_set_str(ptr, StringValuePtr(obj), 10, rnd);
      break;
    case T_FLOAT:
      mpfr_set_d(ptr, NUM2DBL(obj), rnd);
      break;
    case T_FIXNUM:
      mpfr_set_si(ptr, FIX2LONG(obj), rnd);
      break;
    case T_BIGNUM:
      r_mpfr_convert_to_str_set(ptr, obj, rnd);
      break;
    default:
      rb_raise(rb_eArgError, "Invalid class %s for making MPFR.", rb_class2name(obj));
      break;
    }
  }
}

/* If obj is MPFR instance, then this method returns obj. */
/* Otherwise it returns MPFR.new(obj). */
VALUE r_mpfr_new_fr_obj(VALUE obj)
{
  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, obj))){
    return obj;
  }else{
    return rb_funcall(__mpfr_class__, new, 1, obj);
  }
}

static VALUE r_mpfr_alloc(VALUE self)
{
  MPFR *ptr;
  r_mpfr_make_struct(self, ptr);
  return self;
}

/*
  This method returns MPFR instance.
  If there is not an argument, it is set NaN.
  Possible arguments are value, rounding mode, and precesion.
  All arguments are optional.
*/
static VALUE r_mpfr_initialize(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr;
  r_mpfr_get_struct(ptr, self);
  switch(argc){
  case 0:
    mpfr_init(ptr);
    mpfr_set_nan(ptr);
    break;
  case 1:
    mpfr_init(ptr);
    r_mpfr_set_robj(ptr, argv[0], mpfr_get_default_rounding_mode());
    break;
  case 2:
  case 3:
    if(argc == 2){
      mpfr_init(ptr);
    }else{
      mpfr_init2(ptr, NUM2INT(argv[2]));
    }
    r_mpfr_set_robj(ptr, argv[0], r_mpfr_rnd_from_value(argv[1]));
    break;
  default:
    rb_raise(rb_eArgError, "Invalid number of arguments.");
    break;
  }
  return Qtrue;
}

/* This method is the method of initialization for copying object. */
static VALUE r_mpfr_initialize_copy(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_get_struct(ptr_other, other);
  mpfr_init2(ptr_self, mpfr_get_prec(ptr_other));
  mpfr_set(ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  return Qtrue;
}

/* Return array which have MPFR instance converted to from p1 and self. */
static VALUE r_mpfr_coerce(VALUE self, VALUE other)
{
  VALUE val_other;
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_other, ptr_other, mpfr_get_prec(ptr_self));
  r_mpfr_set_robj(ptr_other, other, mpfr_get_default_rounding_mode());
  return rb_ary_new3(2, val_other, self);
}

/* Return NaN. This method takes one optional argument meaning precision. */
static VALUE r_mpfr_nan(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_set_nan(ptr_return);
  return val_ret;
}

/* Return plus infinity. This method takes one optional argument meaning precision. */
static VALUE r_mpfr_pinf(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_set_inf(ptr_return, 1);
  return val_ret;
}

/* Return minus infinity. This method takes one optional argument meaning precision. */
static VALUE r_mpfr_minf(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_set_inf(ptr_return, -1);
  return val_ret;
}

/* ------------------------------ MPFR allocation End ------------------------------ */

/* ------------------------------ Assignment Functions Start ------------------------------ */

/* Reset the precision of self to be exactly p1 bits and set its value to NaN. */
static VALUE r_mpfr_set_prec(VALUE self, VALUE prec)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_set_prec(ptr_self, NUM2INT(prec));
  return INT2NUM((int)mpfr_get_prec(ptr_self));
}

/* Return precision actually used for assignments of self. */
static VALUE r_mpfr_get_prec(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM((int)mpfr_get_prec(ptr_self));
}

/* Arguments are val, rnd, and prec. Set the value of self from val. rnd and prec are optional.*/
static VALUE r_mpfr_set(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mp_rnd_t rnd = r_mpfr_rnd_from_optional_argument(1, 2, argc, argv);
  r_mpfr_set_robj(ptr_self, argv[0], rnd);
  return self;
}

/*
  Arguments are num, exp, rnd, and prec. Set the value of self from num multiplied by two to the power exp.
  Set the value of self from val. rnd and prec are optional.
*/
static VALUE r_mpfr_set_fixnum_2exp(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mp_rnd_t rnd = r_mpfr_rnd_from_optional_argument(2, 3, argc, argv);
  mpfr_set_si_2exp(ptr_self, NUM2INT(argv[0]), NUM2INT(argv[1]), rnd);
  return self;
}

/*
  If p1 is nonnegative Fixnum, set the value of self to plus Inf.
  Otherwise, set the value of self to minus Inf.
*/
static VALUE r_mpfr_set_inf(VALUE self, VALUE sign)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_set_inf(ptr_self, NUM2INT(sign));
  return self;
}

/* Set the value of self to NaN. */
static VALUE r_mpfr_set_nan(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_set_nan(ptr_self);
  return self;
}

/* Swap the values self and p1 efficiently. p1 must be MPFR object. */
static VALUE r_mpfr_swap(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR object.");
  }
  mpfr_swap(ptr_self, ptr_other);
  return self;
}

/* ------------------------------ Assignment Functions End ------------------------------ */

/* ------------------------------ Methods related to string Start ------------------------------ */

/* Output self by mpfr_asprintf( some_val, p1, self ). For example, p1 are "%.Re", "%.Rf" or "%.30Re" etc. */
static VALUE r_mpfr_to_strf(VALUE self, VALUE format_str)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  char *format = StringValuePtr(format_str);
  char *ret_str;
  mpfr_asprintf(&ret_str, format, ptr_self);
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

/* Convert to string. */
static VALUE r_mpfr_to_s(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  char *ret_str;
  mpfr_asprintf(&ret_str, "%.Re", ptr_self);
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

/* Output for debugging. */
static VALUE r_mpfr_inspect(VALUE self)
{
  MPFR *ptr_s;
  r_mpfr_get_struct(ptr_s, self);
  char *ret_str;
  mpfr_asprintf(&ret_str, "#<MPFR:%lx,'%0.Re',%d>", NUM2LONG(rb_funcall(self, object_id, 0)), ptr_s, mpfr_get_prec(ptr_s));
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

/* ------------------------------ Methods related to string End ------------------------------ */

/* ------------------------------ Conversion functions Start ------------------------------ */

/* Return Float object by converting self. Optional argument is rnd meaning rounding mode. See MPFR reference for detail. */
static VALUE r_mpfr_get_d(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return rb_float_new(mpfr_get_d(ptr_self, r_mpfr_rnd_from_optional_argument(0, 1, argc, argv)));
}

/* Return array having Float object d and Fixnum object i such that 0.5 <= abs(d) < 1.0 and */
/* d times 2 raised to exp equals self rounded to double precision. See MPFR reference for detail. */
static VALUE r_mpfr_get_d_2exp(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  long int ret_val2;
  double ret_val1 = mpfr_get_d_2exp(&ret_val2, ptr_self, r_mpfr_rnd_from_optional_argument(0, 1, argc, argv));
  return rb_ary_new3(2, rb_float_new(ret_val1), INT2NUM(ret_val2));
}

/* Return Fixnum object converted after rounding self with respect to rnd which is optional argument. */
static VALUE r_mpfr_get_si(int argc, VALUE *argv, VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM(mpfr_get_si(ptr_self, r_mpfr_rnd_from_optional_argument(0, 1, argc, argv)));
}

/* Return Fixnum object which is nearest integer to self. */
static VALUE r_mpfr_round_to_i(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM(mpfr_get_si(ptr_self, GMP_RNDN));
}

/* Return Fixnum object which is the minimum integer over self. */
static VALUE r_mpfr_ceil_to_i(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM(mpfr_get_si(ptr_self, GMP_RNDU));
}

/* Return Fixnum object which is the maximum integer not over self. */
static VALUE r_mpfr_floor_to_i(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM(mpfr_get_si(ptr_self, GMP_RNDD));
}

/* Return Fixnum object by truncating self. */
static VALUE r_mpfr_truncate_to_i(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2NUM(mpfr_get_si(ptr_self, GMP_RNDZ));
}

/* Return array having String object meaning mantissa and Fixnum object meaning exponent. See MPFR reference for detail. */
static VALUE r_mpfr_get_str(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mp_exp_t e;
  char *str = mpfr_get_str(NULL, &e, 10, 0, ptr_self, GMP_RNDN);
  VALUE ret_str = rb_str_new2(str);
  mpfr_free_str(str);
  return rb_ary_new3(2, ret_str, INT2FIX((int)e));
}

/* ------------------------------ Conversion functions End ------------------------------ */

/* ------------------------------ Basic Arithmetic Functions Start ------------------------------ */

/* Return self + p1. */
static VALUE r_mpfr_add(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
    mpfr_add(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FIXNUM){
    mpfr_add_si(ptr_return, ptr_self, FIX2LONG(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FLOAT){
    mpfr_add_d(ptr_return, ptr_self, NUM2DBL(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_BIGNUM){
    volatile VALUE tmp = rb_funcall(__mpfr_class__, new, 1, other);
    r_mpfr_get_struct(ptr_other, tmp);
    mpfr_add(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR, Fixnum, Float, or Bignum.");    
  }
  return val_ret;
}

/* Return self - p1. */
static VALUE r_mpfr_sub(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
    mpfr_sub(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FIXNUM){
    mpfr_sub_si(ptr_return, ptr_self, FIX2LONG(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FLOAT){
    mpfr_sub_d(ptr_return, ptr_self, NUM2DBL(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_BIGNUM){
    volatile VALUE tmp = rb_funcall(__mpfr_class__, new, 1, other);
    r_mpfr_get_struct(ptr_other, tmp);
    mpfr_sub(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR, Fixnum, Float, or Bignum.");    
  }
  return val_ret;
}

/* Return self * p1. */
static VALUE r_mpfr_mul(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
    mpfr_mul(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FIXNUM){
    mpfr_mul_si(ptr_return, ptr_self, FIX2LONG(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FLOAT){
    mpfr_mul_d(ptr_return, ptr_self, NUM2DBL(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_BIGNUM){
    volatile VALUE tmp = rb_funcall(__mpfr_class__, new, 1, other);
    r_mpfr_get_struct(ptr_other, tmp);
    mpfr_mul(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR, Fixnum, Float, or Bignum.");    
  }
  return val_ret;
}

/* Return self / p1. */
static VALUE r_mpfr_div(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
    mpfr_div(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FIXNUM){
    mpfr_div_si(ptr_return, ptr_self, FIX2LONG(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_FLOAT){
    mpfr_div_d(ptr_return, ptr_self, NUM2DBL(other), mpfr_get_default_rounding_mode());
  }else if(TYPE(other) == T_BIGNUM){
    volatile VALUE tmp = rb_funcall(__mpfr_class__, new, 1, other);
    r_mpfr_get_struct(ptr_other, tmp);
    mpfr_div(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR, Fixnum, Float, or Bignum.");    
  }
  return val_ret;
}

/* Return p1-th power of self. */
static VALUE r_mpfr_pow(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);
  if(TYPE(other) == T_FIXNUM){
    mpfr_pow_si(ptr_return, ptr_self, FIX2LONG(other), mpfr_get_default_rounding_mode());
  }else{
    volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
    r_mpfr_get_struct(ptr_other, tmp_other);
    mpfr_pow(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  }
  return val_ret;
}

/* Return negative value of self. */
static VALUE r_mpfr_neg(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_neg(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* Return absolute value of self. */
static VALUE r_mpfr_abs(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_abs(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* ------------------------------ Basic Arithmetic Functions End ------------------------------ */

/* ------------------------------ Math Basic Arithmetic Functions Start ------------------------------ */

/* This method needs two required arguments and returns p1 + p2. */
static VALUE r_mpfr_math_add(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    mpfr_add(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpfr_add_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    r_mpfr_get_struct(ptr_arg2, rb_funcall(__mpfr_class__, new, 1, argv[1]));
    mpfr_add(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* This method needs two required arguments and returns p1 - p2. */
static VALUE r_mpfr_math_sub(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    mpfr_sub(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpfr_sub_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    r_mpfr_get_struct(ptr_arg2, rb_funcall(__mpfr_class__, new, 1, argv[1]));
    mpfr_sub(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* This method needs two required arguments and returns p1 - p2. */
static VALUE r_mpfr_math_mul(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    mpfr_mul(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpfr_mul_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    r_mpfr_get_struct(ptr_arg2, rb_funcall(__mpfr_class__, new, 1, argv[1]));
    mpfr_mul(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* This method needs two required arguments and returns p1 / p2. */
static VALUE r_mpfr_math_div(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    mpfr_div(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpfr_div_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    r_mpfr_get_struct(ptr_arg2, rb_funcall(__mpfr_class__, new, 1, argv[1]));
    mpfr_div(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* mpfr_sqr(ret, p1, rnd). */
static VALUE r_mpfr_math_sqr(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_sqr(ptr_return, ptr_arg1, rnd);
  return val_ret;
}

/* mpfr_sqrt(ret, p1, rnd). */
static VALUE r_mpfr_math_sqrt(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);

  if(TYPE(argv[0]) == T_FIXNUM){
    long num = FIX2LONG(argv[0]);
    if(num >= 0){
      mpfr_sqrt_ui(ptr_return, (unsigned long)num, rnd);
    }else{
      mpfr_set_nan(ptr_return);
    }
  }else{
    volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
    r_mpfr_get_struct(ptr_arg1, tmp_argv0);
    mpfr_sqrt(ptr_return, ptr_arg1, rnd);
  }
  return val_ret;
}

/* mpfr_rec_sqrt(ret, p1, rnd). */
static VALUE r_mpfr_math_rec_sqrt(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rec_sqrt(ptr_return, ptr_arg1, rnd);
  return val_ret;
}

/* mpfr_cbrt(ret, p1, rnd). */
static VALUE r_mpfr_math_cbrt(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_cbrt(ptr_return, ptr_arg1, rnd);
  return val_ret;
}

/* mpfr_root(ret, p1, rnd). */
static VALUE r_mpfr_math_root(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  int root = NUM2INT(argv[1]);
  if(root > 0){
    mpfr_root(ptr_return, ptr_arg1, root, rnd);
  }else{
    rb_raise(rb_eArgError, "Second argument must be positive Fixnum.");
  }
  return val_ret;
}

/* mpfr_pow(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_pow(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg0, *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg0, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  if(TYPE(argv[1]) == T_FIXNUM){
    mpfr_pow_si(ptr_return, ptr_arg0, FIX2LONG(argv[1]), rnd);
  }else{
    volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
    r_mpfr_get_struct(ptr_arg1, tmp_argv1);
    mpfr_pow(ptr_return, ptr_arg0, ptr_arg1, rnd);
  }
  return val_ret;
}

/* mpfr_dim(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_dim(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg0, *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg0, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_dim(ptr_return, ptr_arg0, ptr_arg1, rnd);
  return val_ret;
}

/* mpfr_mul_2si(ret, p1, p2, rnd). p2 must be integer. */
static VALUE r_mpfr_math_mul_2si(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg0, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg0, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_mul_2si(ptr_return, ptr_arg0, NUM2INT(argv[1]), rnd);
  return val_ret;
}

/* mpfr_div_2si(ret, p1, p2, rnd). p2 must be integer. */
static VALUE r_mpfr_math_div_2si(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg0, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg0, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_div_2si(ptr_return, ptr_arg0, NUM2INT(argv[1]), rnd);
  return val_ret;
}

/* ------------------------------ Math Basic Arithmetic Functions End ------------------------------ */

/* ------------------------------ Comparison Functions Start ------------------------------ */

/* Return negative integer if self < other, negative integer if self > other, or 0 if self == other. */
static VALUE r_mpfr_cmp(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  int val_ret;

  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    r_mpfr_get_struct(ptr_other, other);
    val_ret = mpfr_cmp(ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    val_ret = mpfr_cmp_si(ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    val_ret = mpfr_cmp_d(ptr_self, NUM2DBL(other));
  }else if(TYPE(other) == T_BIGNUM){
    volatile VALUE tmp = rb_funcall(__mpfr_class__, new, 1, other);
    r_mpfr_get_struct(ptr_other, tmp);
    val_ret = mpfr_cmp(ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFR, Fixnum, Float, or Bignum.");    
  }
  return INT2FIX(val_ret);
}

/* mpfr_cmp_ui_2exp(self, p1, p2). */
static VALUE r_mpfr_cmp_ui_2exp(VALUE self, VALUE other, VALUE exp)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  VALUE val_ret;
  int i = NUM2INT(other);
  if(i > 0){
    val_ret = INT2FIX(mpfr_cmp_ui_2exp(ptr_self, i, (mp_exp_t)NUM2INT(exp)));
  }else{
    rb_raise(rb_eArgError, "First argument must be positive Fixnum.");
  }
  return val_ret;
}

/* mpfr_cmp_si_2exp(self, p1, p2). */
static VALUE r_mpfr_cmp_si_2exp(VALUE self, VALUE other, VALUE exp)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2FIX(mpfr_cmp_si_2exp(ptr_self, NUM2INT(other), NUM2INT(exp)));
}

/* mpfr_cmpabs(self, p1). */
static VALUE r_mpfr_cmpabs(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return INT2FIX(mpfr_cmpabs(ptr_self, ptr_other));
}

/* Return true if self is NaN, nil otherwise. */
static VALUE r_mpfr_nan_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  if(mpfr_nan_p(ptr_self) != 0){
    return Qtrue;
  }else{
    return Qfalse;
  }
}

/* Return true if self is infinity, nil otherwise. */
static VALUE r_mpfr_inf_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return (mpfr_inf_p(ptr_self) != 0 ? Qtrue : Qfalse);
}

/* Return 1 if self is plus infinity, -1 if minus infinity. nil otherwise. */
static VALUE r_mpfr_inf_p2(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  if(mpfr_inf_p(ptr_self) != 0){
    return (mpfr_sgn(ptr_self) > 0 ? INT2NUM(1) : INT2NUM(-1));
  }else{
    return Qfalse;
  }
}

/* Return true if self is number, nil otherwise */
static VALUE r_mpfr_number_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return (mpfr_number_p(ptr_self) != 0 ? Qtrue : Qfalse);
}

/* Return true if self is 0, nil otherwise. */
static VALUE r_mpfr_zero_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return (mpfr_zero_p(ptr_self) != 0 ? Qtrue : Qfalse);
}

/* Return true if self is non zero, nil otherwise. */
static VALUE r_mpfr_nonzero_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return (mpfr_zero_p(ptr_self) == 0 ? Qtrue : Qfalse);
}

/* mpfr_sgn(self). */
static VALUE r_mpfr_sgn(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2FIX(mpfr_sgn(ptr_self));
}

/* Return true if self > other, nil otherwise. */
static VALUE r_mpfr_greater_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_greater_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self >= other, nil otherwise. */
static VALUE r_mpfr_greaterequal_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_greaterequal_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self < other, nil otherwise. */
static VALUE r_mpfr_less_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_less_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self <= other, nil otherwise. */
static VALUE r_mpfr_lessequal_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_lessequal_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self < other or self > other, nil otherwise. */
static VALUE r_mpfr_lessgreater_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_lessgreater_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self == other, nil otherwise. */
static VALUE r_mpfr_equal_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_equal_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* Return true if self or other is a NaN, nil otherwise */
static VALUE r_mpfr_unordered_p(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  return (mpfr_unordered_p(ptr_self, ptr_other) != 0 ? Qtrue : Qfalse);
}

/* ------------------------------ Comparison Functions End ------------------------------ */

/* ------------------------------ Integer Related Functions Start  ------------------------------ */

/* mpfr_rint(ret, self, rnd) */
static VALUE r_mpfr_m_rint(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rint(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* mpfr_ceil(ret, self) */
static VALUE r_mpfr_m_ceil(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_ceil(ptr_return, ptr_self);
  return val_ret;
}

/* mpfr_floor(ret, self) */
static VALUE r_mpfr_m_floor(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_floor(ptr_return, ptr_self);
  return val_ret;
}

/* mpfr_round(ret, self) */
static VALUE r_mpfr_m_round(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_round(ptr_return, ptr_self);
  return val_ret;
}

/* mpfr_trunc(ret, self) */
static VALUE r_mpfr_m_trunc(int argc, VALUE *argv, VALUE self)
{
  mp_prec_t prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_trunc(ptr_return, ptr_self);
  return val_ret;
}

/* mpfr_rint_ceil(ret, self, rnd) */
static VALUE r_mpfr_rint_ceil(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rint_ceil(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* mpfr_rint_floor(ret, self, rnd) */
static VALUE r_mpfr_rint_floor(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rint_floor(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* mpfr_rint_round(ret, self, rnd) */
static VALUE r_mpfr_rint_round(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rint_round(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* mpfr_rint_trunc(ret, self, rnd) */
static VALUE r_mpfr_rint_trunc(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_rint_trunc(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* mpfr_frac(ret, self, rnd) */
static VALUE r_mpfr_frac(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_frac(ptr_return, ptr_self, rnd);
  return val_ret;
}

/* Return [ret1, ret2] such that mpfr_modf(ret1, ret2, self, rnd). */
static VALUE r_mpfr_modf(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return1, *ptr_return2;
  VALUE val_ret1, val_ret2;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret1, ptr_return1, prec);
  r_mpfr_make_struct_init2(val_ret2, ptr_return2, prec);
  mpfr_modf(ptr_return1, ptr_return2, ptr_self, rnd);
  return rb_ary_new3(2, val_ret1, val_ret2);
}

/* mpfr_fmod(ret, self, p1, rnd) */
static VALUE r_mpfr_fmod(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_other, tmp_argv0);
  mpfr_fmod(ptr_return, ptr_self, ptr_other, rnd);
  return val_ret;
}

/* mpfr_remainder(ret, self, p1, rnd) */
static VALUE r_mpfr_remainder(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_other, tmp_argv0);
  mpfr_remainder(ptr_return, ptr_self, ptr_other, rnd);
  return val_ret;
}

/* mpfr_remainder(ret, self, p1, mpfr_getdefault_rounding_mode()) */
static VALUE r_mpfr_remainder2(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init(val_ret, ptr_return);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  mpfr_remainder(ptr_return, ptr_self, ptr_other, mpfr_get_default_rounding_mode());
  return val_ret;
}

/* mpfr_remquo(ret1, ret2, self, p1, rnd) */
static VALUE r_mpfr_remquo(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_self, *ptr_other, *ptr_return;
  VALUE val_ret;
  long q;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_other, tmp_argv0);
  mpfr_remquo(ptr_return, &q, ptr_self, ptr_other, rnd);
  return rb_ary_new3(2, val_ret, LONG2FIX(q));
}

/* If value of self is integer, return true. Otherwise, nil. */
static VALUE r_mpfr_integer_p(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return (mpfr_integer_p(ptr_self) != 0 ? Qtrue : Qnil);
}

/* ------------------------------ Integer Related Functions End ------------------------------ */

/* ------------------------------ Miscellaneous Functions Start ------------------------------ */

/* mpfr_nexttoward(self, p1) */
static VALUE r_mpfr_nexttoward(VALUE self, VALUE other)
{
  MPFR *ptr_self, *ptr_other;
  r_mpfr_get_struct(ptr_self, self);
  volatile VALUE tmp_other = r_mpfr_new_fr_obj(other);
  r_mpfr_get_struct(ptr_other, tmp_other);
  mpfr_nexttoward(ptr_self, ptr_other);
  return self;
}

/* mpfr_nextabove(self) */
static VALUE r_mpfr_nextabove(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_nextabove(ptr_self);
  return self;
}

/* mpfr_nextbelow(self) */
static VALUE r_mpfr_nextbelow(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_nextbelow(ptr_self);
  return self;
}

/* If self is not number, return nil. Otherwise return an integer mpfr_get_exp(self). */
static VALUE r_mpfr_get_exp(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  if(mpfr_number_p(ptr_self) != 0){
    return INT2FIX(mpfr_get_exp(ptr_self));
  }else{
    return Qnil;
  }
}

/* arg_exp is integer and we execute mpfr_set_exp(self, arg_exp). */
static VALUE r_mpfr_set_exp(VALUE self, VALUE arg_exp)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mp_exp_t exp = NUM2INT(arg_exp);
  mpfr_set_exp(ptr_self, exp);
  return self;
}

/* Return integer which is mpfr_signbit(self). */
static VALUE r_mpfr_signbit(VALUE self)
{
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  return INT2FIX(mpfr_signbit(ptr_self));
}

/* mpfr_setsign(ret, self, p1, rnd) */
static VALUE r_mpfr_setsign(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  int s = NUM2INT(argv[0]);
  mpfr_setsign(ptr_return, ptr_self, s, rnd);
  return val_ret;
}

/* mpfr_copysign(ret, self, p1, rnd) */
static VALUE r_mpfr_copysign(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_self, *ptr_return, *ptr_arg;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg, tmp_argv0);
  mpfr_copysign(ptr_return, ptr_self, ptr_arg, rnd);
  return val_ret;
}

/* ------------------------------ Miscellaneous Functions End ------------------------------ */

/* ------------------------------ Rounding Mode Related Functions Start ------------------------------ */

/* mpfr_prec_round(ret, prec, rnd) */
static VALUE r_mpfr_prec_round(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpfr_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, mpfr_get_prec(ptr_self));
  mpfr_set(ptr_return, ptr_self, mpfr_get_default_prec());
  mpfr_prec_round(ptr_return, prec, rnd);
  return val_ret;
}

/* mpfr_prec_round(ret, prec, rnd) */
static VALUE r_mpfr_prec_round2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_self;
  r_mpfr_get_struct(ptr_self, self);
  mpfr_prec_round(ptr_self, prec, rnd);
  return self;
}

/* ------------------------------ Rounding Mode Related Functions End ------------------------------ */

/* ------------------------------ Special Functions Start ------------------------------ */

/*
  Ruby MPFR saves the returned value of Special Functions (log, sin, cos, etc)
  in MPFR library to class variable of class MPFR.
  This method returns that class variable meaning the returned value of Special Functions in MPFR library.
  See the MPFR reference for the kind of Special Functions and the meaning of the returned value of Special Functions.
 */
static VALUE r_mpfr_get_special_func_state(VALUE self)
{
  return rb_cv_get(r_mpfr_class, SPECIAL_FUNC_STATE);
}

static void r_mpfr_set_special_func_state(int num)
{
  rb_cv_set(r_mpfr_class, SPECIAL_FUNC_STATE, INT2NUM(num));
}

/* mpfr_log(ret, p1, rnd). */
static VALUE r_mpfr_math_log(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_log(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_log2(ret, p1, rnd). */
static VALUE r_mpfr_math_log2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_log2(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_log10(ret, p1, rnd). */
static VALUE r_mpfr_math_log10(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_log10(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_exp(ret, p1, rnd). */
static VALUE r_mpfr_math_exp(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_exp(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_exp2(ret, p1, rnd). */
static VALUE r_mpfr_math_exp2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_exp2(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_exp10(ret, p1, rnd). */
static VALUE r_mpfr_math_exp10(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_exp10(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_cos(ret, p1, rnd). */
static VALUE r_mpfr_math_cos(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_cos(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_sin(ret, p1, rnd). */
static VALUE r_mpfr_math_sin(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_sin(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_tan(ret, p1, rnd). */
static VALUE r_mpfr_math_tan(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_tan(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_sec(ret, p1, rnd). */
static VALUE r_mpfr_math_sec(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_sec(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_csc(ret, p1, rnd). */
static VALUE r_mpfr_math_csc(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_csc(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_cot(ret, p1, rnd). */
static VALUE r_mpfr_math_cot(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_cot(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* Return [sin, cos] such as mpfr_sin_cos(sin, cos, p1, rnd). */
static VALUE r_mpfr_math_sin_cos(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_return1, *ptr_return2;
  VALUE val_ret1, val_ret2;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret1, ptr_return1, prec);
  r_mpfr_make_struct_init2(val_ret2, ptr_return2, prec);
  mpfr_sin_cos(ptr_return1, ptr_return2, ptr_arg1, rnd);
  return rb_ary_new3(2, ptr_return1, ptr_return2);
}

/* mpfr_acos(ret, p1, rnd). */
static VALUE r_mpfr_math_acos(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_acos(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_asin(ret, p1, rnd). */
static VALUE r_mpfr_math_asin(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_asin(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_atan(ret, p1, rnd). */
static VALUE r_mpfr_math_atan(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_atan(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_atan2(ret, p1, rnd). */
static VALUE r_mpfr_math_atan2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg0, *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg0, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_atan2(ptr_return, ptr_arg0, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_cosh(ret, p1, rnd). */
static VALUE r_mpfr_math_cosh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_cosh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_sinh(ret, p1, rnd). */
static VALUE r_mpfr_math_sinh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_sinh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_tanh(ret, p1, rnd). */
static VALUE r_mpfr_math_tanh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_tanh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* Return array contaning hyperbolic sine and hyperbolic cosine by mpfr_sinh_cosh(ret1, ret2, arg1, rnd). */
static VALUE r_mpfr_math_sinh_cosh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return1, *ptr_return2;
  VALUE val_ret1, val_ret2;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret1, ptr_return1, prec);
  r_mpfr_make_struct_init2(val_ret2, ptr_return2, prec);
  r_mpfr_set_special_func_state(mpfr_sinh_cosh(ptr_return1, ptr_return2, ptr_arg1, rnd));
  return rb_ary_new3(2, val_ret1, val_ret2);
}

/* mpfr_acosh(ret, p1, rnd). */
static VALUE r_mpfr_math_acosh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_acosh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_asinh(ret, p1, rnd). */
static VALUE r_mpfr_math_asinh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_asinh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_atanh(ret, p1, rnd). */
static VALUE r_mpfr_math_atanh(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_atanh(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_fac_ui(ret, p1, rnd). */
static VALUE r_mpfr_math_fac_ui(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  int num = NUM2INT(argv[0]);
  if(num >= 0){
    r_mpfr_set_special_func_state(mpfr_fac_ui(ptr_return, num, rnd));  
  }else{
    rb_raise(rb_eArgError, "First argument must be positive Fixnum.");
  }
  return val_ret;
}

/* mpfr_log1p(ret, p1, rnd). */
static VALUE r_mpfr_math_log1p(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_log1p(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_expm1(ret, p1, rnd). */
static VALUE r_mpfr_math_expm1(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_expm1(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_eint(ret, p1, rnd). */
static VALUE r_mpfr_math_eint(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_eint(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_li2(ret, p1, rnd). */
static VALUE r_mpfr_math_li2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_li2(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_gamma(ret, p1, rnd). */
static VALUE r_mpfr_math_gamma(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_gamma(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_lngamma(ret, p1, rnd). */
static VALUE r_mpfr_math_lngamma(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_lngamma(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* Execute mpfr_lgamma(ret1, ret2, p1, rnd) and return [ret1, ret2]. */
static VALUE r_mpfr_math_lgamma(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  int singp;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_lgamma(ptr_return, &singp, ptr_arg1, rnd));
  return rb_ary_new3(2, val_ret, INT2FIX(singp));
}

/* mpfr_zeta(ret, p1, rnd). */
static VALUE r_mpfr_math_zeta(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  if(TYPE(argv[0]) == T_FIXNUM){
    int num = FIX2LONG(argv[0]);
    if(num >= 0){
      mpfr_zeta_ui(ptr_return, (unsigned long)num, rnd);
      return val_ret;
    }
  }else{
    volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
    r_mpfr_get_struct(ptr_arg1, tmp_argv0);
    mpfr_zeta(ptr_return, ptr_arg1, rnd);
  }
  return val_ret;
}

/* mpfr_erf(ret, p1, rnd). */
static VALUE r_mpfr_math_erf(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_erf(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_erfc(ret, p1, rnd). */
static VALUE r_mpfr_math_erfc(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_erfc(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_j0(ret, p1, rnd). */
static VALUE r_mpfr_math_j0(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_j0(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_j1(ret, p1, rnd). */
static VALUE r_mpfr_math_j1(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_j1(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_jn(ret, p2, p1, rnd) */
static VALUE r_mpfr_math_jn(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  int n = NUM2INT(argv[1]);
  r_mpfr_set_special_func_state(mpfr_jn(ptr_return, n, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_y0(ret, p1, rnd). */
static VALUE r_mpfr_math_y0(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_y0(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_y1(ret, p1, rnd). */
static VALUE r_mpfr_math_y1(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_y1(ptr_return, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_yn(ret, p2, p1, rnd) */
static VALUE r_mpfr_math_yn(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  int n = NUM2INT(argv[1]);
  r_mpfr_set_special_func_state(mpfr_yn(ptr_return, n, ptr_arg1, rnd));
  return val_ret;
}

/* mpfr_fma(ret, p1, p2, p3, rnd). */
static VALUE r_mpfr_math_fma(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 3, 5, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_arg3, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  volatile VALUE tmp_argv2 = r_mpfr_new_fr_obj(argv[2]);
  r_mpfr_get_struct(ptr_arg3, tmp_argv2);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_fma(ptr_return, ptr_arg1, ptr_arg2, ptr_arg3, rnd));
  return val_ret;
}

/* mpfr_fms(ret, p1, p2, p3, rnd). */
static VALUE r_mpfr_math_fms(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 3, 5, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_arg3, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  volatile VALUE tmp_argv2 = r_mpfr_new_fr_obj(argv[2]);
  r_mpfr_get_struct(ptr_arg3, tmp_argv2);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_fms(ptr_return, ptr_arg1, ptr_arg2, ptr_arg3, rnd));
  return val_ret;
}

/* mpfr_agm(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_agm(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_agm(ptr_return, ptr_arg1, ptr_arg2, rnd));
  return val_ret;
}

/* mpfr_hypot(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_hypot(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_hypot(ptr_return, ptr_arg1, ptr_arg2, rnd));
  return val_ret;
}

/* mpfr_const_log2(ret, rnd). */
static VALUE r_mpfr_math_const_log2(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_const_log2(ptr_return, rnd));
  return val_ret;
}

/* mpfr_const_pi(ret, rnd). */
static VALUE r_mpfr_math_const_pi(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_const_pi(ptr_return, rnd));
  return val_ret;
}

/* mpfr_const_euler(ret, rnd). */
static VALUE r_mpfr_math_const_euler(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_const_euler(ptr_return, rnd));
  return val_ret;
}

/* mpfr_const_catalan(ret, rnd). */
static VALUE r_mpfr_math_const_catalan(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_const_catalan(ptr_return, rnd));
  return val_ret;
}

/* Execute mpfr_free_cache(). */
static VALUE r_mpfr_math_free_cache(VALUE self)
{
  mpfr_free_cache();
  return Qnil;
}

/* Calculate sum of MPFR objects. */
static VALUE r_mpfr_math_sum(int argc, VALUE *argv, VALUE self){
  int num;
  for (num = 0; num < argc; num += 1) {
    if(!RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[num]))){ break; }
  }
  
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, num, num + 2, argc, argv);
  MPFR *ptr_return, *ptr_args[num];
  VALUE val_ret;
  int i;
  for(i = 0; i < num; i++){
    volatile VALUE tmp_argvi = r_mpfr_new_fr_obj(argv[i]);
    r_mpfr_get_struct(ptr_args[i], tmp_argvi);
  }
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpfr_set_special_func_state(mpfr_sum(ptr_return, ptr_args, argc, rnd));
  return val_ret;
}

/* ------------------------------ Special Functions End ------------------------------ */

/* ------------------------------ MPFR::Math Miscellaneous Functions Start ------------------------------ */

/* mpfr_min(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_min(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_min(ptr_return, ptr_arg1, ptr_arg2, rnd);
  return val_ret;
}

/* mpfr_max(ret, p1, p2, rnd). */
static VALUE r_mpfr_math_max(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPFR *ptr_arg1, *ptr_arg2, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpfr_new_fr_obj(argv[0]);
  r_mpfr_get_struct(ptr_arg1, tmp_argv0);
  volatile VALUE tmp_argv1 = r_mpfr_new_fr_obj(argv[1]);
  r_mpfr_get_struct(ptr_arg2, tmp_argv1);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_max(ptr_return, ptr_arg1, ptr_arg2, rnd);
  return val_ret;
}



void Init_mpfr()
{
  /* ------------------------------ Class MPFR Start ------------------------------ */
  
  /*
    = Ruby MPFR Library
    This extended library is ruby interface to {the MPFR library (http://www.mpfr.org/)}[http://www.mpfr.org/].
    Please see MPFR documents (for example {http://www.mpfr.org/mpfr-current/mpfr.html}[http://www.mpfr.org/mpfr-current/mpfr.html])
    along with this documents.

    == Class and module
    In this library, class MPFR is that of multiple precision number and
    module MPFR::Math collects singleton methods which mainly have MPFR arguments.

    Generally, the functions in the following items of MPFR reference manual is implemented
    as instance methods of class MPFR.
    - Conversion Functions 
    - Basic Arithmetic Functions 
    - Comparison Functions

    And, the class method of module MPFR::Math mainly consints of the functions
    in "Special functions" items of MPFR reference manual.
    
    == Name of methods and constants
    Except for some methods,
    the names of methods inherit that of MPFR functions in C language and
    each name of method is removed 'mpfr_' from that of correspnding MPFR function in C language.
    The methods m_rint, m_ceil, m_floor, m_round and m_trunc are exceptions and
    respectively corresponts to mpfr_rint, mpfr_ceil, mpfr_floor, mpfr_round and mpfr_trunc.
    Note that there are also methods rint, ceil, floor, round and trunc
    which have respectively defferent actions
    from mpfr_rint, mpfr_ceil, mpfr_floor, mpfr_round and mpfr_trunc.

    Some macros in MPFR library become Constants in Ruby.
    These name is that got rid of 'MPFR_'.

    == Optional arguments of methods (rounding mode and precision)
    If the most methods could have two optional arguments _rnd_ and _prec_,
    _prec_ is precision and _rnd_ is rounding mode.
    If you omit these arguments, default value is used.
    See MPFR.set_default_prec, MPFR.get_default_prec, MPFR.set_default_rounding_mode and MPFR.get_default_rounding_mode.

    If the methods can have argument of rounding mode,
    we can hand the constants MPFR::RNDN, MPFR::RNDZ, MPFR::RNDU and MPFR::RNDD to that.

    == Returned value of methods
    If there is no remark for returned value of method,
    the method returns ruby object corresponding to
    value of MPFR function in C language.

    == Conversion for arguments of method

    When method have argument of MPFR and we set instance of other class to the argument,
    the method tries to convert the argument to MPFR instance.
  */
  r_mpfr_class = rb_define_class("MPFR", rb_cNumeric);
  rb_include_module(r_mpfr_class, rb_mComparable);

  /* ------------------------------ Class MPFR End ------------------------------ */

  /* ------------------------------ Constants Start ------------------------------ */

  /* Version string which mpfr_get_version() returns. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION", rb_str_new2(mpfr_get_version()));
  /* String which mpfr_get_patches() returns. */
  rb_define_const(r_mpfr_class, "MPFR_PATCHES", rb_str_new2(mpfr_get_patches()));
  /* Integer which is macro MPFR_VERSION. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION2", INT2NUM(MPFR_VERSION));
  /* Integer which is macro MPFR_VERSION_MAJOR. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION_MAJOR", INT2NUM(MPFR_VERSION_MAJOR));
  /* Integer which is macro MPFR_VERSION_MINOR. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION_MINOR", INT2NUM(MPFR_VERSION_MINOR));
  /* Integer which is macro MPFR_VERSION_PATCHLEVEL. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION_PATCHLEVEL", INT2NUM(MPFR_VERSION_PATCHLEVEL));
  /* String whichi is macro MPFR_VERSION_STRING. */
  rb_define_const(r_mpfr_class, "MPFR_VERSION_STRING", rb_str_new2(MPFR_VERSION_STRING));

  /* Integer which is macro MPFR_PREC_MAX. */
  rb_define_const(r_mpfr_class, "PREC_MAX", INT2NUM(MPFR_PREC_MAX));
  /* Integer which is macro MPFR_PREC_MIN. */
  rb_define_const(r_mpfr_class, "PREC_MIN", INT2NUM(MPFR_PREC_MIN));
  /* Integer which is macro MPFR_EMAX_DEFAULT. */
  rb_define_const(r_mpfr_class, "EMAX_DEFAULT", INT2NUM(MPFR_EMAX_DEFAULT));
  /* Integer whichi is MPFR_EMIN_DEFAULT. */
  rb_define_const(r_mpfr_class, "EMIN_DEFAULT", INT2NUM(MPFR_EMIN_DEFAULT));

  /* Integer which is macro GMP_RNDN. */
  rb_define_const(r_mpfr_class, "RNDN", INT2NUM(GMP_RNDN));
  /* Integer which is macro GMP_RNDZ. */
  rb_define_const(r_mpfr_class, "RNDZ", INT2NUM(GMP_RNDZ));
  /* Integer which is macro GMP_RNDU. */
  rb_define_const(r_mpfr_class, "RNDU", INT2NUM(GMP_RNDU));
  /* Integer which is macro GMP_RNDD. */
  rb_define_const(r_mpfr_class, "RNDD", INT2NUM(GMP_RNDD));

  /* ------------------------------ Constants End ------------------------------ */

  /* ------------------------------ Precision and Rounding Mode Start ------------------------------ */

  rb_define_singleton_method(r_mpfr_class, "set_default_prec", r_mpfr_set_default_prec, 1);
  rb_define_singleton_method(r_mpfr_class, "get_default_prec", r_mpfr_get_default_prec, 0);
  rb_define_singleton_method(r_mpfr_class, "set_default_rounding_mode", r_mpfr_set_default_rounding_mode, 1);
  rb_define_singleton_method(r_mpfr_class, "get_default_rounding_mode", r_mpfr_get_default_rounding_mode, 0);

  /* ------------------------------ Precision and Rounding Mode End ------------------------------ */

  /* ------------------------------ Exception Related Functions Start ------------------------------ */

  rb_define_singleton_method(r_mpfr_class, "get_emin", r_mpfr_get_emin, 0);
  rb_define_singleton_method(r_mpfr_class, "get_emax", r_mpfr_get_emax, 0);
  rb_define_singleton_method(r_mpfr_class, "set_emin", r_mpfr_set_emin, 1);
  rb_define_singleton_method(r_mpfr_class, "set_emax", r_mpfr_set_emax, 1);
  rb_define_singleton_method(r_mpfr_class, "get_emin_min", r_mpfr_get_emin_min, 0);
  rb_define_singleton_method(r_mpfr_class, "get_emin_max", r_mpfr_get_emin_max, 0);
  rb_define_singleton_method(r_mpfr_class, "get_emax_min", r_mpfr_get_emax_min, 0);
  rb_define_singleton_method(r_mpfr_class, "get_emax_max", r_mpfr_get_emax_max, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_underflow", r_mpfr_clear_underflow, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_overflow", r_mpfr_clear_overflow, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_nanflag", r_mpfr_clear_nanflag, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_inexflag", r_mpfr_clear_inexflag, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_erangeflag", r_mpfr_clear_erangeflag, 0);
  rb_define_singleton_method(r_mpfr_class, "set_underflow", r_mpfr_set_underflow, 0);
  rb_define_singleton_method(r_mpfr_class, "set_overflow", r_mpfr_set_overflow, 0);
  rb_define_singleton_method(r_mpfr_class, "set_nanflag", r_mpfr_set_nanflag, 0);
  rb_define_singleton_method(r_mpfr_class, "set_inexflag", r_mpfr_set_inexflag, 0);
  rb_define_singleton_method(r_mpfr_class, "set_erangeflag", r_mpfr_set_erangeflag, 0);
  rb_define_singleton_method(r_mpfr_class, "clear_flags", r_mpfr_clear_flags, 0);
  rb_define_singleton_method(r_mpfr_class, "underflow_p", r_mpfr_underflow_p, 0);
  rb_define_singleton_method(r_mpfr_class, "overflow_p", r_mpfr_overflow_p, 0);
  rb_define_singleton_method(r_mpfr_class, "nanflag_p", r_mpfr_nanflag_p, 0);
  rb_define_singleton_method(r_mpfr_class, "inexflag_p", r_mpfr_inexflag_p, 0);
  rb_define_singleton_method(r_mpfr_class, "erangeflag_p", r_mpfr_erangeflag_p, 0);

  rb_define_method(r_mpfr_class, "check_range", r_mpfr_check_range, -1);
  rb_define_method(r_mpfr_class, "subnormalize", r_mpfr_subnormalize, -1);

  /* ------------------------------ Exception Related Functions End ------------------------------ */

  /* ------------------------------ MPFR allocation Start ------------------------------ */

  rb_define_alloc_func(r_mpfr_class, r_mpfr_alloc);
  rb_define_private_method(r_mpfr_class, "initialize", r_mpfr_initialize, -1);
  rb_define_private_method(r_mpfr_class, "initialize_copy", r_mpfr_initialize_copy, 1);
  rb_define_method(r_mpfr_class, "coerce", r_mpfr_coerce, 1);

  rb_define_singleton_method(r_mpfr_class, "nan", r_mpfr_nan, -1);
  rb_define_singleton_method(r_mpfr_class, "pinf", r_mpfr_pinf, -1);
  rb_define_singleton_method(r_mpfr_class, "minf", r_mpfr_minf, -1);

  /* ------------------------------ MPFR allocation End ------------------------------ */

  /* ------------------------------ Assignment Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "set_prec", r_mpfr_set_prec, 1);
  rb_define_method(r_mpfr_class, "get_prec", r_mpfr_get_prec, 0);
  rb_define_method(r_mpfr_class, "set", r_mpfr_set, -1);
  rb_define_method(r_mpfr_class, "set_fixnum_2exp", r_mpfr_set_fixnum_2exp, -1);
  rb_define_method(r_mpfr_class, "set_inf", r_mpfr_set_inf, 1);
  rb_define_method(r_mpfr_class, "set_nan", r_mpfr_set_nan, 0);
  rb_define_method(r_mpfr_class, "swap", r_mpfr_swap, 1);

  /* ------------------------------ Assignment Functions End ------------------------------ */

  /* ------------------------------ Methods related to string Start ------------------------------ */

  rb_define_method(r_mpfr_class, "to_strf", r_mpfr_to_strf, 1);
  rb_define_method(r_mpfr_class, "to_s", r_mpfr_to_s, 0);
  rb_define_method(r_mpfr_class, "inspect", r_mpfr_inspect, 0);
  /* ------------------------------ Methods related to string End ------------------------------ */

  /* ------------------------------ Conversion functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "get_d", r_mpfr_get_d, -1);
  rb_define_method(r_mpfr_class, "get_d_2exp", r_mpfr_get_d_2exp, -1);
  rb_define_method(r_mpfr_class, "get_si", r_mpfr_get_si, -1);
  rb_define_method(r_mpfr_class, "get_str", r_mpfr_get_str, 0);
  
  rb_define_method(r_mpfr_class, "round", r_mpfr_round_to_i, 0);
  rb_define_method(r_mpfr_class, "floor", r_mpfr_floor_to_i, 0);
  rb_define_method(r_mpfr_class, "ceil", r_mpfr_ceil_to_i, 0);
  rb_define_method(r_mpfr_class, "truncate", r_mpfr_truncate_to_i, 0);

  rb_define_alias(r_mpfr_class, "to_f", "get_d");
  rb_define_alias(r_mpfr_class, "to_i", "truncate");
  rb_define_alias(r_mpfr_class, "to_int", "truncate");

  /* ------------------------------ Conversion functions End ------------------------------ */
  
  /* ------------------------------ Basic Arithmetic Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "+", r_mpfr_add, 1);
  rb_define_method(r_mpfr_class, "-", r_mpfr_sub, 1);
  rb_define_method(r_mpfr_class, "*", r_mpfr_mul, 1);
  rb_define_method(r_mpfr_class, "/", r_mpfr_div, 1);
  rb_define_method(r_mpfr_class, "**", r_mpfr_pow, 1);
  rb_define_method(r_mpfr_class, "neg", r_mpfr_neg, -1);
  rb_define_method(r_mpfr_class, "abs", r_mpfr_abs, -1);

  /* ------------------------------ Basic Arithmetic Functions End ------------------------------ */

  /* ------------------------------ Comparison Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "cmp", r_mpfr_cmp, 1);
  rb_define_method(r_mpfr_class, "cmp_ui_2exp", r_mpfr_cmp_ui_2exp, 2);
  rb_define_method(r_mpfr_class, "cmp_si_2exp", r_mpfr_cmp_si_2exp, 2);
  rb_define_method(r_mpfr_class, "cmpabs", r_mpfr_cmpabs, 1);
  rb_define_method(r_mpfr_class, "nan_p", r_mpfr_nan_p, 0);
  rb_define_method(r_mpfr_class, "inf_p", r_mpfr_inf_p, 0);
  rb_define_method(r_mpfr_class, "number_p", r_mpfr_number_p, 0);
  rb_define_method(r_mpfr_class, "zero_p", r_mpfr_zero_p, 0);
  rb_define_method(r_mpfr_class, "sgn", r_mpfr_sgn, 0);
  rb_define_method(r_mpfr_class, "greater_p", r_mpfr_greater_p, 1);
  rb_define_method(r_mpfr_class, "greaterequal_p", r_mpfr_greaterequal_p, 1);
  rb_define_method(r_mpfr_class, "less_p", r_mpfr_less_p, 1);
  rb_define_method(r_mpfr_class, "lessequal_p", r_mpfr_lessequal_p, 1);
  rb_define_method(r_mpfr_class, "lessgreater_p", r_mpfr_lessgreater_p, 1);
  rb_define_method(r_mpfr_class, "equal_p", r_mpfr_equal_p, 1);
  rb_define_method(r_mpfr_class, "unordered_p", r_mpfr_unordered_p, 1);
  rb_define_method(r_mpfr_class, "infinite?", r_mpfr_inf_p2, 0);
  
  rb_define_alias(r_mpfr_class, "<=>", "cmp");
  rb_define_alias(r_mpfr_class, "nan?", "nan_p");
  rb_define_alias(r_mpfr_class, "finite?", "number_p");
  rb_define_alias(r_mpfr_class, "zero?", "zero_p");
  rb_define_method(r_mpfr_class, "nonzero?", r_mpfr_nonzero_p, 1);

  /* ------------------------------ Comparison Functions Start ------------------------------ */

  /* ------------------------------ Integer Related Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "m_rint", r_mpfr_m_rint, -1);
  rb_define_method(r_mpfr_class, "m_ceil", r_mpfr_m_ceil, -1);
  rb_define_method(r_mpfr_class, "m_floor", r_mpfr_m_floor, -1);
  rb_define_method(r_mpfr_class, "m_round", r_mpfr_m_round, -1);
  rb_define_method(r_mpfr_class, "m_trunc", r_mpfr_m_trunc, -1);
  rb_define_method(r_mpfr_class, "rint_ceil", r_mpfr_rint_ceil, -1);
  rb_define_method(r_mpfr_class, "rint_floor", r_mpfr_rint_floor, -1);
  rb_define_method(r_mpfr_class, "rint_round", r_mpfr_rint_round, -1);
  rb_define_method(r_mpfr_class, "rint_trunc", r_mpfr_rint_trunc, -1);
  rb_define_method(r_mpfr_class, "frac", r_mpfr_frac, -1);
  rb_define_method(r_mpfr_class, "modf", r_mpfr_modf, -1);
  rb_define_method(r_mpfr_class, "fmod", r_mpfr_fmod, -1);
  rb_define_method(r_mpfr_class, "remainder", r_mpfr_remainder, -1);
  rb_define_method(r_mpfr_class, "%", r_mpfr_remainder2, -1);
  rb_define_method(r_mpfr_class, "remquo", r_mpfr_remquo, -1);
  rb_define_method(r_mpfr_class, "integer_p", r_mpfr_integer_p, 0);

  rb_define_alias(r_mpfr_class, "integer?", "integer_p");
  rb_define_alias(r_mpfr_class, "modulo", "%");

  /* ------------------------------ Integer Related Functions End ------------------------------ */
  
  /* ------------------------------ Miscellaneous Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "nexttoward", r_mpfr_nexttoward, 1);
  rb_define_method(r_mpfr_class, "nextabove", r_mpfr_nextabove, 0);
  rb_define_method(r_mpfr_class, "nextbelow", r_mpfr_nextbelow, 0);
  rb_define_method(r_mpfr_class, "get_exp", r_mpfr_get_exp, 0);
  rb_define_method(r_mpfr_class, "set_exp", r_mpfr_set_exp, 1);
  rb_define_method(r_mpfr_class, "signbit", r_mpfr_signbit, 0);
  rb_define_method(r_mpfr_class, "setsign", r_mpfr_setsign, -1);
  rb_define_method(r_mpfr_class, "copysign", r_mpfr_copysign, -1);

  /* ------------------------------ Miscellaneous Functions End ------------------------------ */

  /* ------------------------------ Rounding Mode Related Functions Start ------------------------------ */

  rb_define_method(r_mpfr_class, "prec_round", r_mpfr_prec_round, -1);
  rb_define_method(r_mpfr_class, "prec_round!", r_mpfr_prec_round2, -1);

  /* ------------------------------ Rounding Mode Related Functions End ------------------------------ */


  /* ------------------------------ Module MPFR::Math Start ------------------------------ */

  /*
    Almost all class methods in this module is the functions in "Special Functions" item of MPFR reference manual.
    First arguments of these methods takes MPFR instance.
    If it is not MPFR instance, it is converted to MPFR instance in this method.
    And all methods have two optional arguments rnd and prec.
    That is, You may call MPFR::Math.some_method(some_required_args, rnd, prec) and
    last two arguments rnd and prec could be omitted.
    The methods collected in this module works as the following imitational C code.
     mpfr_t returned_val;
     mpfr_init2(returned_val, prec);
     mpfr_some_func(returned_val, p1, ..., pn, rnd);
     return returned_val;
    The returned value of mpfr_some_func is saved and if you want to get this value
    then you may use MPFR.get_special_func_state.
  */
  r_mpfr_math = rb_define_module_under(r_mpfr_class, "Math");

  rb_define_class_variable(r_mpfr_class, SPECIAL_FUNC_STATE, INT2FIX(0));
  rb_define_singleton_method(r_mpfr_math, "get_special_func_state", r_mpfr_get_special_func_state, 0);

  /* ------------------------------ Math Basic Arithmetic Functions Start ------------------------------ */

  rb_define_module_function(r_mpfr_math, "add", r_mpfr_math_add, -1);
  rb_define_module_function(r_mpfr_math, "sub", r_mpfr_math_sub, -1);
  rb_define_module_function(r_mpfr_math, "mul", r_mpfr_math_mul, -1);
  rb_define_module_function(r_mpfr_math, "div", r_mpfr_math_div, -1);
  rb_define_module_function(r_mpfr_math, "sqr", r_mpfr_math_sqr, -1);
  rb_define_module_function(r_mpfr_math, "sqrt", r_mpfr_math_sqrt, -1);
  rb_define_module_function(r_mpfr_math, "rec_sqrt", r_mpfr_math_rec_sqrt, -1);
  rb_define_module_function(r_mpfr_math, "cbrt", r_mpfr_math_cbrt, -1);
  rb_define_module_function(r_mpfr_math, "root", r_mpfr_math_root, -1);
  rb_define_module_function(r_mpfr_math, "pow", r_mpfr_math_pow, -1);
  rb_define_module_function(r_mpfr_math, "dim", r_mpfr_math_dim, -1);
  rb_define_module_function(r_mpfr_math, "mul_2si", r_mpfr_math_mul_2si, -1);
  rb_define_module_function(r_mpfr_math, "div_2si", r_mpfr_math_div_2si, -1);

  /* ------------------------------ Math Basic Arithmetic Functions End ------------------------------ */

  /* ------------------------------ Special Functions Start ------------------------------ */

  rb_define_module_function(r_mpfr_math, "log", r_mpfr_math_log, -1);
  rb_define_module_function(r_mpfr_math, "log2", r_mpfr_math_log2, -1);
  rb_define_module_function(r_mpfr_math, "log10", r_mpfr_math_log10, -1);
  rb_define_module_function(r_mpfr_math, "exp", r_mpfr_math_exp, -1);
  rb_define_module_function(r_mpfr_math, "exp2", r_mpfr_math_exp2, -1);
  rb_define_module_function(r_mpfr_math, "exp10", r_mpfr_math_exp10, -1);
  rb_define_module_function(r_mpfr_math, "cos", r_mpfr_math_cos, -1);
  rb_define_module_function(r_mpfr_math, "sin", r_mpfr_math_sin, -1);
  rb_define_module_function(r_mpfr_math, "tan", r_mpfr_math_tan, -1);
  rb_define_module_function(r_mpfr_math, "sec", r_mpfr_math_sec, -1);
  rb_define_module_function(r_mpfr_math, "csc", r_mpfr_math_csc, -1);
  rb_define_module_function(r_mpfr_math, "cot", r_mpfr_math_cot, -1);
  rb_define_module_function(r_mpfr_math, "sin_cos", r_mpfr_math_sin_cos, -1);
  rb_define_module_function(r_mpfr_math, "acos", r_mpfr_math_acos, -1);
  rb_define_module_function(r_mpfr_math, "asin", r_mpfr_math_asin, -1);
  rb_define_module_function(r_mpfr_math, "atan", r_mpfr_math_atan, -1);
  rb_define_module_function(r_mpfr_math, "atan2", r_mpfr_math_atan2, -1);
  rb_define_module_function(r_mpfr_math, "cosh", r_mpfr_math_cosh, -1);
  rb_define_module_function(r_mpfr_math, "sinh", r_mpfr_math_sinh, -1);
  rb_define_module_function(r_mpfr_math, "tanh", r_mpfr_math_tanh, -1);
  rb_define_module_function(r_mpfr_math, "sinh_cosh", r_mpfr_math_sinh_cosh, -1);
  rb_define_module_function(r_mpfr_math, "acosh", r_mpfr_math_acosh, -1);
  rb_define_module_function(r_mpfr_math, "asinh", r_mpfr_math_asinh, -1);
  rb_define_module_function(r_mpfr_math, "atanh", r_mpfr_math_atanh, -1);
  rb_define_module_function(r_mpfr_math, "fac_ui", r_mpfr_math_fac_ui, -1);
  rb_define_module_function(r_mpfr_math, "log1p", r_mpfr_math_log1p, -1);
  rb_define_module_function(r_mpfr_math, "expm1", r_mpfr_math_expm1, -1);
  rb_define_module_function(r_mpfr_math, "eint", r_mpfr_math_eint, -1);
  rb_define_module_function(r_mpfr_math, "li2", r_mpfr_math_li2, -1);
  rb_define_module_function(r_mpfr_math, "gamma", r_mpfr_math_gamma, -1);
  rb_define_module_function(r_mpfr_math, "lngamma", r_mpfr_math_lngamma, -1);
  rb_define_module_function(r_mpfr_math, "lgamma", r_mpfr_math_lgamma, -1);
  rb_define_module_function(r_mpfr_math, "zeta", r_mpfr_math_zeta, -1);
  rb_define_module_function(r_mpfr_math, "erf", r_mpfr_math_erf, -1);
  rb_define_module_function(r_mpfr_math, "erfc", r_mpfr_math_erfc, -1);
  rb_define_module_function(r_mpfr_math, "j0", r_mpfr_math_j0, -1);
  rb_define_module_function(r_mpfr_math, "j1", r_mpfr_math_j1, -1);
  rb_define_module_function(r_mpfr_math, "jn", r_mpfr_math_jn, -1);
  rb_define_module_function(r_mpfr_math, "y0", r_mpfr_math_y0, -1);
  rb_define_module_function(r_mpfr_math, "y1", r_mpfr_math_y1, -1);
  rb_define_module_function(r_mpfr_math, "yn", r_mpfr_math_yn, -1);
  rb_define_module_function(r_mpfr_math, "fma", r_mpfr_math_fma, -1);
  rb_define_module_function(r_mpfr_math, "fms", r_mpfr_math_fms, -1);
  rb_define_module_function(r_mpfr_math, "agm", r_mpfr_math_agm, -1);
  rb_define_module_function(r_mpfr_math, "hypot", r_mpfr_math_hypot, -1);
  rb_define_module_function(r_mpfr_math, "const_log2", r_mpfr_math_const_log2, -1);
  rb_define_module_function(r_mpfr_math, "const_pi", r_mpfr_math_const_pi, -1);
  rb_define_module_function(r_mpfr_math, "const_euler", r_mpfr_math_const_euler, -1);
  rb_define_module_function(r_mpfr_math, "const_catalan", r_mpfr_math_const_catalan, -1);
  rb_define_module_function(r_mpfr_math, "free_cache", r_mpfr_math_free_cache, 0);
  rb_define_module_function(r_mpfr_math, "sum", r_mpfr_math_sum, -1);

  /* ------------------------------ Special Functions End ------------------------------ */

  /* ------------------------------ MPFR::Math Miscellaneous Functions Start ------------------------------ */

  rb_define_module_function(r_mpfr_math, "min", r_mpfr_math_min, -1);
  rb_define_module_function(r_mpfr_math, "max", r_mpfr_math_max, -1);

  /* ------------------------------ MPFR::Math Miscellaneous Functions End ------------------------------ */

  /* ------------------------------ Module MPFR::Math End ------------------------------ */

  eqq = rb_intern("===");
  to_s = rb_intern("to_s");
  new = rb_intern("new");
  class = rb_intern("class");
  method_defined = rb_intern("method_defined?");
  object_id = rb_intern("object_id");

  __mpfr_class__ = rb_eval_string("MPFR");
  __sym_to_s__ = rb_eval_string(":to_s");
  __sym_to_str__ = rb_eval_string(":to_str");

}
