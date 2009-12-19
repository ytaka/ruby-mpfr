#include "ruby_mpc.h"

#define CLASS_VAL_FUNCTION_STATE "@@function_state"

static ID eqq, new,  object_id;
static VALUE __mpc_class__, __mpfr_class__;

/* Default rounding mode */
mp_rnd_t __rb_mpc_default_rounding_mode = MPC_RNDNN;

void rb_mpc_extended_set_default_rounding_mode (mpc_rnd_t rnd_mode)
{
  if (rnd_mode >= 0 && MPC_RND_RE(rnd_mode) < 4 && MPC_RND_IM(rnd_mode) < 4)
    __rb_mpc_default_rounding_mode = rnd_mode;
}

mpc_rnd_t rb_mpc_extended_get_default_rounding_mode (void)
{
  return __rb_mpc_default_rounding_mode;
}

mp_prec_t rb_mpc_get_max_prec (mpc_t x)
{
  mp_prec_t precre = mpfr_get_prec(mpc_realref (x)), precim = mpfr_get_prec(mpc_imagref (x));
  return (precre > precim ? precre : precim);
}

/* State of function */

void r_mpc_set_c_function_state(int num)
{
  rb_cv_set(r_mpc_class, CLASS_VAL_FUNCTION_STATE, rb_ary_new3(2, INT2NUM(MPC_INEX_RE(num)), INT2NUM(MPC_INEX_IM(num))));
}

void r_mpc_set_fr_function_state(int num)
{
  rb_cv_set(r_mpc_class, CLASS_VAL_FUNCTION_STATE, INT2NUM(num));
}


/* If argc equals max, convert last argument to rounding mode number. */
/* Otherwise, return defoult rounding mode number. */
mpc_rnd_t r_mpc_rnd_from_optional_argument(int min, int max, int argc, VALUE *argv)
{
  mp_rnd_t rnd;
  if(argc >= min && argc <= max){
    if(argc == max){
      rnd = r_mpc_rnd_from_value(argv[max-1]);
    }else{
      rnd = rb_mpc_extended_get_default_rounding_mode();
    }
  }else{
    rb_raise(rb_eArgError, "Invalid number of arguments.");
  }
  return rnd;
}

/* If argc equals max, convert last argument to precision number. */
/* Otherwise, return defoult precision number. */
mpc_rnd_t r_mpc_prec_from_optional_argument(int min, int max, int argc, VALUE *argv)
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
void r_mpc_get_rnd_prec_from_optional_arguments(mpc_rnd_t *rnd, mp_prec_t *prec, int min, int max,
						int argc, VALUE *argv)
{
  if(argc >= min && argc <= max){
    if(argc >= max - 1){
      *rnd = r_mpc_rnd_from_value(argv[max - 2]);
    }else{
      *rnd = rb_mpc_extended_get_default_rounding_mode();
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

/* Set the default rounding mode. The default rounding mode is MPC::RNDNN. */
static VALUE r_mpc_set_default_rounding_mode(VALUE self, VALUE rnd)
{
  mp_rnd_t a = NUM2INT(rnd);
  rb_mpc_extended_set_default_rounding_mode(a);
  return INT2FIX(rb_mpc_extended_get_default_rounding_mode());
}

/* Get the default rounding mode. */
static VALUE r_mpc_get_default_rounding_mode(VALUE self)
{
  return INT2NUM(rb_mpc_extended_get_default_rounding_mode());
}

/* Return function state. */
static VALUE r_mpc_get_function_state(VALUE self)
{
  return rb_cv_get(r_mpc_class, CLASS_VAL_FUNCTION_STATE);
}

/* ---------- Allocation ---------- */

void r_mpc_free(void *ptr){
  mpc_clear(ptr);
  free(ptr);
}

/* Allocation function. */
static VALUE r_mpc_alloc(VALUE self)
{
  MPC *ptr;
  r_mpc_make_struct(self, ptr);
  return self;
}

VALUE r_mpc_new_c_obj(VALUE obj)
{
  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, obj))){
    return obj;
  }else{
    return rb_funcall(__mpc_class__, new, 1, obj);
  }
}


static void r_mpc_set_from_two_objects(MPC *ptr, VALUE real, VALUE imag, mpc_rnd_t rnd)
{
  r_mpfr_set_robj(mpc_realref(ptr), real, MPC_RND_RE(rnd));
  r_mpfr_set_robj(mpc_imagref(ptr), imag, MPC_RND_IM(rnd));
}

static void r_mpc_set_from_one_object(MPC *ptr, VALUE obj, mpc_rnd_t rnd)
{
  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, obj))){
    MPC *ptr_obj;
    r_mpc_get_struct(ptr_obj, obj);
    mpc_set(ptr, ptr_obj, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, obj))){
    MPFR *ptr_obj;
    r_mpfr_get_struct(ptr_obj, obj);
    mpc_set_fr(ptr, ptr_obj, rnd);
  }else{
    switch(TYPE(obj)){
    case T_FLOAT:
      mpc_set_d(ptr, NUM2DBL(obj), rnd);
      break;
    case T_FIXNUM:
      mpc_set_si(ptr, FIX2LONG(obj), rnd);
      break;
    default:
      r_mpfr_set_robj(mpc_realref(ptr), obj, MPC_RND_RE(rnd));
      mpfr_set_si(mpc_imagref(ptr), 0, MPC_RND_IM(rnd));
      break;
    }
  }
}

static void r_mpc_set_initial_value(MPC *ptr, int argc, VALUE *argv)
{
  switch(argc){
  case 0:
    mpc_init2(ptr, mpfr_get_default_prec());
    break;
  case 1:
    mpc_init2(ptr, mpfr_get_default_prec());
    r_mpc_set_from_one_object(ptr, argv[0], rb_mpc_extended_get_default_rounding_mode());
    break;
  case 2:
    mpc_init2(ptr, mpfr_get_default_prec());
    r_mpc_set_from_two_objects(ptr, argv[0], argv[1], rb_mpc_extended_get_default_rounding_mode());
    break;
  case 3:
    mpc_init2(ptr, mpfr_get_default_prec());
    r_mpc_set_from_two_objects(ptr, argv[0], argv[1], r_mpc_rnd_from_value(argv[2]));
    break;
  case 4:
    mpc_init2(ptr, NUM2INT(argv[3]));
    r_mpc_set_from_two_objects(ptr, argv[0], argv[1], r_mpc_rnd_from_value(argv[2]));
    break;
  default:
    rb_raise(rb_eArgError, "Invalid number of arguments.");
    break;
  }  
}

/* Return new MPC instance. The same arguments as MPC.new is acceptable. */
static VALUE r_mpc_global_new(int argc, VALUE *argv, VALUE self)
{
  MPC *ptr;
  VALUE val;
  r_mpc_make_struct(val, ptr);
  r_mpc_set_initial_value(ptr, argc, argv);
  return val;
}


/*
  Arguments may be the following types by number.
  0. none
  1. MPC or array having two elements
  2. real part and imaginary part
  3. real part, imaginary part, and rounding mode
  4. real part, imaginary part, rounding mode and precision
*/
static VALUE r_mpc_initialize(int argc, VALUE *argv, VALUE self)
{
  MPC *ptr;
  r_mpc_get_struct(ptr, self);
  r_mpc_set_initial_value(ptr, argc, argv);
  return Qtrue;
}

/* initialize_copy */
static VALUE r_mpc_initialize_copy(VALUE self, VALUE other)
{
  MPC *ptr_self, *ptr_other;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_get_struct(ptr_other, other);
  mpc_init2(ptr_self, rb_mpc_get_max_prec(ptr_other));
  mpc_set(ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  return Qtrue;
}

/* Return array which have MPC instance converted to from p1 and self. */
static VALUE r_mpc_coerce(VALUE self, VALUE other)
{
  VALUE val_other;
  MPC *ptr_self, *ptr_other;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init2(val_other, ptr_other, rb_mpc_get_max_prec(ptr_self));
  r_mpc_set_from_one_object(ptr_other, other, rb_mpc_extended_get_default_rounding_mode());
  return rb_ary_new3(2, val_other, self);
}

/* String */

/* inspect */
static VALUE r_mpc_inspect(VALUE self)
{
  MPC *ptr_s;
  r_mpc_get_struct(ptr_s, self);
  char *ret_str;
  mpfr_asprintf(&ret_str, "#<MPC:%lx,['%.Re','%.Re'],[%d,%d]>",
		NUM2LONG(rb_funcall(self, object_id, 0)), mpc_realref (ptr_s), mpc_imagref (ptr_s),
		mpfr_get_prec(mpc_realref (ptr_s)), mpfr_get_prec(mpc_imagref (ptr_s)));
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}


/* Projection and Decomposing Functions */

/* Return the real part. */
static VALUE r_mpc_real (VALUE self)
{
  MPC *ptr_self;
  r_mpc_get_struct(ptr_self, self);
  VALUE ret_val;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(ret_val, ptr_ret, mpfr_get_prec(mpc_realref(ptr_self)));
  r_mpc_set_fr_function_state(mpc_real(ptr_ret, ptr_self, MPC_RNDNN));
  return ret_val;
}

/* Return the imaginary part. */
static VALUE r_mpc_imag (VALUE self)
{
  MPC *ptr_self;
  r_mpc_get_struct(ptr_self, self);
  VALUE ret_val;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(ret_val, ptr_ret, mpfr_get_prec(mpc_imagref(ptr_self)));
  r_mpc_set_fr_function_state(mpc_imag(ptr_ret, ptr_self, MPC_RNDNN));
  return ret_val;
}

/* Return the argument. */
static VALUE r_mpc_arg (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self;
  r_mpc_get_struct(ptr_self, self);
  VALUE ret_val;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(ret_val, ptr_ret, prec);
  r_mpc_set_fr_function_state(mpc_arg(ptr_ret, ptr_self, rnd));
  return ret_val;
}

/* Return the projection onto the Riemann sphere. */
static VALUE r_mpc_proj (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self;
  r_mpc_get_struct(ptr_self, self);
  VALUE ret_val;
  MPC *ptr_ret;
  r_mpc_make_struct_init2(ret_val, ptr_ret, prec);
  r_mpc_set_c_function_state(mpc_proj(ptr_ret, ptr_self, rnd));
  return ret_val;
}

/* Basic Arithmetic Functions */

/* Return self + p1. */
static VALUE r_mpc_add(VALUE self, VALUE other)
{
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init(val_ret, ptr_return);
  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, other))){
    MPC *ptr_other;
    r_mpc_get_struct(ptr_other, other);
    mpc_add(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpc_add_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else{
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, rb_funcall(__mpc_class__, new, 1, other));
    mpc_add_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }
  return val_ret;
}

/* Return self / p1. */
static VALUE r_mpc_sub(VALUE self, VALUE other)
{
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, other))){
    MPC *ptr_other;
    r_mpc_get_struct(ptr_other, other);
    mpc_sub(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpc_sub_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else{
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, rb_funcall(__mpc_class__, new, 1, other));
    mpc_sub_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }
  return val_ret;
}

/* Return self * p1. */
static VALUE r_mpc_mul(VALUE self, VALUE other)
{
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, other))){
    MPC *ptr_other;
    r_mpc_get_struct(ptr_other, other);
    mpc_mul(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpc_mul_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else{
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, rb_funcall(__mpc_class__, new, 1, other));
    mpc_mul_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }
  return val_ret;
}

/* Return self / p1. */
static VALUE r_mpc_div(VALUE self, VALUE other)
{
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init(val_ret, ptr_return);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, other))){
    MPC *ptr_other;
    r_mpc_get_struct(ptr_other, other);
    mpc_div(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpc_div_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }else{
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, rb_funcall(__mpc_class__, new, 1, other));
    mpc_div_fr(ptr_return, ptr_self, ptr_other, rb_mpc_extended_get_default_rounding_mode());
  }
  return val_ret;
}

/* Multiply self by _i_. */
static VALUE r_mpc_mul_i (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_mul_i(ptr_return, ptr_self, NUM2INT(argv[0]), rnd));
  return val_ret;
}

/* Multiply self by -1. */
static VALUE r_mpc_neg (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_neg(ptr_return, ptr_self, rnd));
  return val_ret;
}

/* Return conjugate of self. */
static VALUE r_mpc_conj (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self, *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_conj(ptr_return, ptr_self, rnd));
  return val_ret;
}

/* Return absolute value. */
static VALUE r_mpc_abs (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self;
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_fr_function_state(mpc_abs(ptr_return, ptr_self, rnd));
  return val_ret;
}

/* Return the norm (the square of absolute value). */
static VALUE r_mpc_norm (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 0, 2, argc, argv);
  MPC *ptr_self;
  MPFR *ptr_return;
  VALUE val_ret;
  r_mpc_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_fr_function_state(mpc_norm(ptr_return, ptr_self, rnd));
  return val_ret;
}

/* module function */

/* Return self + p1. */
static VALUE r_mpc_math_add(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg1, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, argv[1]))){
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, argv[1]);
    mpc_add(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_arg2;
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    r_mpc_set_c_function_state(mpc_add_fr(ptr_return, ptr_arg1, ptr_arg2, rnd));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpc_add_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, rb_funcall(__mpc_class__, new, 1, argv[1]));
    mpc_add(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* Return self - p1. */
static VALUE r_mpc_math_sub(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg1, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, argv[1]))){
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, argv[1]);
    mpc_sub(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_arg2;
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    r_mpc_set_c_function_state(mpc_sub_fr(ptr_return, ptr_arg1, ptr_arg2, rnd));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    int other = FIX2LONG(argv[1]);
    if (other >= 0) {
      mpc_sub_ui(ptr_return, ptr_arg1, other, rnd);
    } else {
      mpc_add_ui(ptr_return, ptr_arg1, -other, rnd);
    }
  }else{
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, rb_funcall(__mpc_class__, new, 1, argv[1]));
    mpc_sub(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* Return self * p1. */
static VALUE r_mpc_math_mul(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg1, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, argv[1]))){
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, argv[1]);
    mpc_mul(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_arg2;
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    r_mpc_set_c_function_state(mpc_mul_fr(ptr_return, ptr_arg1, ptr_arg2, rnd));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpc_mul_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, rb_funcall(__mpc_class__, new, 1, argv[1]));
    mpc_mul(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}

/* Return self / p1. */
static VALUE r_mpc_math_div(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg1, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, argv[1]))){
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, argv[1]);
    mpc_div(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_arg2;
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    r_mpc_set_c_function_state(mpc_div_fr(ptr_return, ptr_arg1, ptr_arg2, rnd));
  }else{
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, rb_funcall(__mpc_class__, new, 1, argv[1]));
    mpc_div(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}


/* mpc_mul_2exp(ret, p1, p2, rnd) */
static VALUE r_mpc_math_mul_2exp (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_mul_2exp(ptr_return, ptr_arg, NUM2LONG(argv[1]), rnd));
  return val_ret;
}

/* mpc_div_2exp(ret, p1, p2, rnd) */
static VALUE r_mpc_math_div_2exp (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_div_2exp(ptr_return, ptr_arg, NUM2LONG(argv[1]), rnd));
  return val_ret;
}


/* mpc_sqr(ret, p1, rnd) */
static VALUE r_mpc_math_sqr (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_sqr(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_sqrt(ret, p1, rnd) */
static VALUE r_mpc_math_sqrt (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_sqrt(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_pow(ret, p1, p2, rnd), mpc_pow_fr(ret, p1, p2, rnd) etc. */
static VALUE r_mpc_math_pow(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 2, 4, argc, argv);
  MPC *ptr_arg1, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg1, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);

  if(RTEST(rb_funcall(__mpc_class__, eqq, 1, argv[1]))){
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, argv[1]);
    mpc_pow(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_arg2;
    r_mpfr_get_struct(ptr_arg2, argv[1]);
    r_mpc_set_c_function_state(mpc_pow_fr(ptr_return, ptr_arg1, ptr_arg2, rnd));
  }else if(TYPE(argv[1]) == T_FLOAT){
    mpc_pow_d(ptr_return, ptr_arg1, NUM2DBL(argv[1]), rnd);
  }else if(TYPE(argv[1]) == T_FIXNUM){
    mpc_pow_si(ptr_return, ptr_arg1, FIX2LONG(argv[1]), rnd);
  }else{
    MPC *ptr_arg2;
    r_mpc_get_struct(ptr_arg2, rb_funcall(__mpc_class__, new, 1, argv[1]));
    mpc_pow(ptr_return, ptr_arg1, ptr_arg2, rnd);
  }
  return val_ret;
}


/* mpc_exp(ret, p1, rnd) */
static VALUE r_mpc_math_exp (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_exp(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_log(ret, p1, rnd) */
static VALUE r_mpc_math_log (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_log(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_sin(ret, p1, rnd) */
static VALUE r_mpc_math_sin (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_sin(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_cos(ret, p1, rnd) */
static VALUE r_mpc_math_cos (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_cos(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_tan(ret, p1, rnd) */
static VALUE r_mpc_math_tan (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_tan(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_sinh(ret, p1, rnd) */
static VALUE r_mpc_math_sinh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_sinh(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_cosh(ret, p1, rnd) */
static VALUE r_mpc_math_cosh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_cosh(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_tanh(ret, p1, rnd) */
static VALUE r_mpc_math_tanh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_tanh(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_asin(ret, p1, rnd) */
static VALUE r_mpc_math_asin (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_asin(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_acos(ret, p1, rnd) */
static VALUE r_mpc_math_acos (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_acos(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_atan(ret, p1, rnd) */
static VALUE r_mpc_math_atan (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_atan(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_asinh(ret, p1, rnd) */
static VALUE r_mpc_math_asinh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_asinh(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_acosh(ret, p1, rnd) */
static VALUE r_mpc_math_acosh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_acosh(ptr_return, ptr_arg, rnd));
  return val_ret;
}

/* mpc_atanh(ret, p1, rnd) */
static VALUE r_mpc_math_atanh (int argc, VALUE *argv, VALUE self)
{
  mpc_rnd_t rnd;
  mp_prec_t prec;
  r_mpc_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  MPC *ptr_arg, *ptr_return;
  VALUE val_ret;
  volatile VALUE tmp_argv0 = r_mpc_new_c_obj(argv[0]);
  r_mpc_get_struct(ptr_arg, tmp_argv0);
  r_mpc_make_struct_init2(val_ret, ptr_return, prec);
  r_mpc_set_c_function_state(mpc_atanh(ptr_return, ptr_arg, rnd));
  return val_ret;
}


void Init_mpc(){
  r_mpc_class = rb_define_class("MPC", rb_cNumeric);

  rb_define_singleton_method(r_mpc_class, "set_default_rounding_mode", r_mpc_set_default_rounding_mode, 1);
  rb_define_singleton_method(r_mpc_class, "get_default_rounding_mode", r_mpc_get_default_rounding_mode, 0);

  /* Integer which is macro MPC_RNDNN. */
  rb_define_const(r_mpc_class, "RNDNN", INT2NUM(MPC_RNDNN));
  /* Integer which is macro MPC_RNDNZ. */
  rb_define_const(r_mpc_class, "RNDNZ", INT2NUM(MPC_RNDNZ));
  /* Integer which is macro MPC_RNDNU. */
  rb_define_const(r_mpc_class, "RNDNU", INT2NUM(MPC_RNDNU));
  /* Integer which is macro MPC_RNDND. */
  rb_define_const(r_mpc_class, "RNDND", INT2NUM(MPC_RNDND));
  /* Integer which is macro MPC_RNDZN. */
  rb_define_const(r_mpc_class, "RNDZN", INT2NUM(MPC_RNDZN));
  /* Integer which is macro MPC_RNDZZ. */
  rb_define_const(r_mpc_class, "RNDZZ", INT2NUM(MPC_RNDZZ));
  /* Integer which is macro MPC_RNDZU. */
  rb_define_const(r_mpc_class, "RNDZU", INT2NUM(MPC_RNDZU));
  /* Integer which is macro MPC_RNDZD. */
  rb_define_const(r_mpc_class, "RNDZD", INT2NUM(MPC_RNDZD));
  /* Integer which is macro MPC_RNDUN */
  rb_define_const(r_mpc_class, "RNDUN", INT2NUM(MPC_RNDUN));
  /* Integer which is macro MPC_RNDUZ. */
  rb_define_const(r_mpc_class, "RNDUZ", INT2NUM(MPC_RNDUZ));
  /* Integer which is macro MPC_RNDUU. */
  rb_define_const(r_mpc_class, "RNDUU", INT2NUM(MPC_RNDUU));
  /* Integer which is macro MPC_RNDUD. */
  rb_define_const(r_mpc_class, "RNDUD", INT2NUM(MPC_RNDUD));
  /* Integer which is macro MPC_RNDDN. */
  rb_define_const(r_mpc_class, "RNDDN", INT2NUM(MPC_RNDDN));
  /* Integer which is macro MPC_RNDDZ. */
  rb_define_const(r_mpc_class, "RNDDZ", INT2NUM(MPC_RNDDZ));
  /* Integer which is macro MPC_RNDDU. */
  rb_define_const(r_mpc_class, "RNDDU", INT2NUM(MPC_RNDDU));
  /* Integer which is macro MPC_RNDDD. */
  rb_define_const(r_mpc_class, "RNDDD", INT2NUM(MPC_RNDDD));

  /* Integer which is macro MPC_VERSION_MAJOR. */
  rb_define_const(r_mpc_class, "VERSION_MAJOR", INT2NUM(MPC_VERSION_MAJOR));
  /* Integer which is macro MPC_VERSION_MINOR. */
  rb_define_const(r_mpc_class, "VERSION_MINOR", INT2NUM(MPC_VERSION_MINOR));
  /* Integer which is macro MPC_VERSION_PATCHLEVEL. */
  rb_define_const(r_mpc_class, "VERSION_PATCHLEVEL", INT2NUM(MPC_VERSION_PATCHLEVEL));
  /* Stirng which is macro MPC_VERSION_STRING. */
  rb_define_const(r_mpc_class, "VERSION_STRING", rb_str_new2(MPC_VERSION_STRING));
  

  rb_define_singleton_method(r_mpc_class, "get_function_state", r_mpc_get_function_state, 0);

  rb_define_global_function("MPC", r_mpc_global_new, -1);
  rb_define_alloc_func(r_mpc_class, r_mpc_alloc);
  rb_define_private_method(r_mpc_class, "initialize", r_mpc_initialize, -1);
  rb_define_private_method(r_mpc_class, "initialize_copy", r_mpc_initialize_copy, 1);

  rb_define_method(r_mpc_class, "coerce", r_mpc_coerce, 1);

  rb_define_method(r_mpc_class, "inspect", r_mpc_inspect, 0);


  rb_define_method(r_mpc_class, "real", r_mpc_real, 0);
  rb_define_method(r_mpc_class, "imag", r_mpc_imag, 0);
  rb_define_method(r_mpc_class, "arg", r_mpc_arg, -1);
  rb_define_method(r_mpc_class, "proj", r_mpc_proj, -1);




  /* Basic Arithmetic Functions */
  rb_define_method(r_mpc_class, "+", r_mpc_add, 1);
  rb_define_method(r_mpc_class, "-", r_mpc_sub, 1);
  rb_define_method(r_mpc_class, "*", r_mpc_mul, 1);
  rb_define_method(r_mpc_class, "/", r_mpc_div, 1);
  rb_define_method(r_mpc_class, "mul_i", r_mpc_mul_i, -1);
  rb_define_method(r_mpc_class, "neg", r_mpc_neg, -1);
  rb_define_method(r_mpc_class, "conj", r_mpc_conj, -1);
  rb_define_method(r_mpc_class, "abs", r_mpc_abs, -1);
  rb_define_method(r_mpc_class, "norm", r_mpc_norm, -1);

    
  
  r_mpc_math = rb_define_module_under(r_mpc_class, "Math");  

  rb_define_module_function(r_mpc_math, "add", r_mpc_math_add, -1);
  rb_define_module_function(r_mpc_math, "sub", r_mpc_math_sub, -1);
  rb_define_module_function(r_mpc_math, "mul", r_mpc_math_mul, -1);
  rb_define_module_function(r_mpc_math, "div", r_mpc_math_div, -1);
  rb_define_module_function(r_mpc_math, "mul_2exp", r_mpc_math_mul_2exp, -1);
  rb_define_module_function(r_mpc_math, "div_2exp", r_mpc_math_div_2exp, -1);

  rb_define_module_function(r_mpc_math, "sqr", r_mpc_math_sqr, -1);
  rb_define_module_function(r_mpc_math, "sqrt", r_mpc_math_sqrt, -1);
  rb_define_module_function(r_mpc_math, "pow", r_mpc_math_pow, -1);
  rb_define_module_function(r_mpc_math, "exp", r_mpc_math_exp, -1);
  rb_define_module_function(r_mpc_math, "log", r_mpc_math_log, -1);
  rb_define_module_function(r_mpc_math, "sin", r_mpc_math_sin, -1);
  rb_define_module_function(r_mpc_math, "cos", r_mpc_math_cos, -1);
  rb_define_module_function(r_mpc_math, "tan", r_mpc_math_tan, -1);
  rb_define_module_function(r_mpc_math, "sinh", r_mpc_math_sinh, -1);
  rb_define_module_function(r_mpc_math, "cosh", r_mpc_math_cosh, -1);
  rb_define_module_function(r_mpc_math, "tanh", r_mpc_math_tanh, -1);
  rb_define_module_function(r_mpc_math, "asin", r_mpc_math_asin, -1);
  rb_define_module_function(r_mpc_math, "acos", r_mpc_math_acos, -1);
  rb_define_module_function(r_mpc_math, "atan", r_mpc_math_atan, -1);
  rb_define_module_function(r_mpc_math, "asinh", r_mpc_math_asinh, -1);
  rb_define_module_function(r_mpc_math, "acosh", r_mpc_math_acosh, -1);
  rb_define_module_function(r_mpc_math, "atanh", r_mpc_math_atanh, -1);

  eqq = rb_intern("===");
  new = rb_intern("new");
  object_id = rb_intern("object_id");
  __mpc_class__ = rb_eval_string("MPC");
  __mpfr_class__ = rb_eval_string("MPFR");

}

