#include "ruby_mpfi.h"

#define CLASS_VAL_FUNCTION_STATE "@@function_state"

static ID eqq, to_s, new, class, method_defined, object_id;
static VALUE __mpfi_class__, __mpfr_class__, __sym_to_s__, __sym_to_str__;


/* ------------------------------ function state start ------------------------------ */

/* Save the value of last function state. */
void r_mpfi_set_function_state(int num)
{
  rb_cv_set(r_mpfi_class, CLASS_VAL_FUNCTION_STATE, INT2NUM(num));
}

/* Return state of last function of MPFI. */
VALUE r_mpfi_get_function_state(VALUE self)
{
  return rb_cv_get(r_mpfi_class, CLASS_VAL_FUNCTION_STATE);
}

/* ------------------------------ function state end ------------------------------ */

/* ------------------------------ allocation start ------------------------------ */

void r_mpfi_free(void *ptr)
{
  mpfi_clear(ptr);
  free(ptr);
}

VALUE r_mpfi_make_new_fi_obj(MPFI *ptr)
{
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  mpfi_set(ptr_ret, ptr);
  return ret;
}

static void r_mpfi_set_interv_from_robjs (MPFI *ptr, VALUE a1, VALUE a2)
{
  if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, a1)) && RTEST(rb_funcall(__mpfr_class__, eqq, 1, a2))){
    MPFR *ptr_a1, *ptr_a2;
    r_mpfr_get_struct(ptr_a1, a1);
    r_mpfr_get_struct(ptr_a2, a2);
    r_mpfi_set_function_state(mpfi_interv_fr(ptr, ptr_a1, ptr_a2));
  }else if((TYPE(a1) == T_FIXNUM) && (TYPE(a2) == T_FIXNUM)){
    r_mpfi_set_function_state(mpfi_interv_si(ptr, FIX2LONG(a1), FIX2LONG(a2)));
  }else if((TYPE(a1) == T_FLOAT) && (TYPE(a2) == T_FLOAT)){
    r_mpfi_set_function_state(mpfi_interv_d(ptr, NUM2DBL(a1), NUM2DBL(a2)));
  }else{
    MPFR *ptr_a1, *ptr_a2;
    volatile VALUE tmp_a1 = r_mpfr_new_fr_obj(a1);
    volatile VALUE tmp_a2 = r_mpfr_new_fr_obj(a2);
    r_mpfr_get_struct(ptr_a1, tmp_a1);
    r_mpfr_get_struct(ptr_a2, tmp_a2);
    r_mpfi_set_function_state(mpfi_interv_fr(ptr, ptr_a1, ptr_a2));
  }
}

static void r_mpfi_set_from_array (MPFI *ptr, VALUE obj)
{
  if (RARRAY_LEN(obj) != 2) {
    rb_raise(rb_eArgError, "Invalid size of array for MPFI initialization.");
  }
  r_mpfi_set_interv_from_robjs(ptr, rb_ary_entry(obj, 0), rb_ary_entry(obj, 1));
}

/* VALUE obj must have method to_s or to_str.
   The formats of strings that we can use are "number" or "[number1, number2]"
   (refer MPFI documents). */
static void r_mpfi_set_from_string (MPFI *ptr, VALUE obj)
{
  char *str;
  if(RTEST(rb_funcall(rb_funcall(obj, class, 0), method_defined, 1, __sym_to_str__))){
    str = StringValuePtr(obj);
  }else if(RTEST(rb_funcall(rb_funcall(obj, class, 0), method_defined, 1, __sym_to_s__))){
    VALUE tmp = rb_funcall(obj, to_s, 0);
    str = StringValuePtr(tmp);
  }
  if(mpfi_set_str(ptr, str, 10) != 0) {
    rb_raise(rb_eArgError, "Invalid string format of MPFI initialization: \"%s\"", str);
  }
}

void r_mpfi_set_robj(MPFI *ptr, VALUE obj)
{
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, obj))){
    MPFI *ptr_obj;
    r_mpfi_get_struct(ptr_obj, obj);
    mpfi_set(ptr, ptr_obj);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, obj))){
    MPFR *ptr_obj;
    r_mpfr_get_struct(ptr_obj, obj);
    mpfi_set_fr(ptr, ptr_obj);
  }else{
    switch(TYPE(obj)){
    case T_FLOAT:
      mpfi_set_d(ptr, NUM2DBL(obj));
      break;
    case T_FIXNUM:
      mpfi_set_si(ptr, FIX2LONG(obj));
      break;
    case T_ARRAY:
      r_mpfi_set_from_array(ptr, obj);
      break;
    default:
      r_mpfi_set_from_string(ptr, obj);
      break;
    }
  }
}

VALUE r_mpfi_new_fi_obj(VALUE obj)
{
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, obj))){
    return obj;
  }else{
    return rb_funcall(__mpfi_class__, new, 1, obj);
  }
}

/* Initializing and Assigning Intervals */

/* Allocation function. */
static VALUE r_mpfi_alloc(VALUE self)
{
  MPFI *ptr;
  r_mpfi_make_struct(self, ptr);
  return self;
}

static void r_mpfi_set_initial_value(MPFI *ptr, int argc, VALUE *argv)
{
  switch(argc){
  case 0:
    mpfi_init(ptr);
    break;
  case 1:
    mpfi_init(ptr);
    r_mpfi_set_robj(ptr, argv[0]);
    break;
  case 2:
    mpfi_init2(ptr, NUM2INT(argv[1]));
    r_mpfi_set_robj(ptr, argv[0]);
    break;
  default:
    rb_raise(rb_eArgError, "Invalid number of arguments.");
    break;
  }
}

/* Return new MPFI instance. The same arguments as MPFI.new is acceptable. */
static VALUE r_mpfi_global_new(int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr;
  VALUE val;
  r_mpfi_make_struct(val, ptr);
  r_mpfi_set_initial_value(ptr, argc, argv);
  return val;
}

/* Two optional arguments are acceptable.
   The first argument means value of number,
   which is MPFR, Float, Fixnum, String, and Array having two elements.
   The string must be "number" or "[number1, number2]" (refer MPFI documents).
   The second one is precision and its class is Fixnum. */
static VALUE r_mpfi_initialize(int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr;
  r_mpfi_get_struct(ptr, self);
  r_mpfi_set_initial_value(ptr, argc, argv);
  return Qtrue;
}

/* initialize_copy. */
static VALUE r_mpfi_initialize_copy(VALUE self, VALUE other)
{
  MPFI *ptr_self, *ptr_other;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_other, other);
  mpfi_init2(ptr_self, mpfi_get_prec(ptr_other));
  mpfi_set(ptr_self, ptr_other);
  return Qtrue;
}

/* Return array which have MPFI instance converted to from p1 and self. */
static VALUE r_mpfi_coerce(VALUE self, VALUE other)
{
  VALUE val_other;
  MPFI *ptr_self, *ptr_other;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_make_struct_init2(val_other, ptr_other, mpfi_get_prec(ptr_self));
  r_mpfi_set_robj(ptr_other, other);
  return rb_ary_new3(2, val_other, self);
}

/* Set the value. */
static VALUE r_mpfi_set (VALUE self, VALUE arg)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_set_robj(ptr_self, arg);
  return self;
}

/* Swap value for other MPFI instance. */
static VALUE r_mpfi_swap (VALUE self, VALUE other)
{
  MPFI *ptr_self, *ptr_other;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_other, other);
  mpfi_swap(ptr_self, ptr_other);
  return self;
}

char *r_mpfi_dump_to_string(MPFI *ptr_s)
{
  char *ret_str, *str_right, *str_left;
  str_left = r_mpfr_dump_to_string(r_mpfi_left_ptr(ptr_s));
  str_right = r_mpfr_dump_to_string(r_mpfi_right_ptr(ptr_s));
  if(!mpfr_asprintf(&ret_str, "%s %s", str_left, str_right)) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  mpfr_free_str(str_left);
  mpfr_free_str(str_right);
  return ret_str;
}

static VALUE r_mpfi_marshal_dump(VALUE self)
{
  MPFI *ptr_s;
  r_mpfi_get_struct(ptr_s, self);
  char *ret_str = r_mpfi_dump_to_string(ptr_s);
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

void r_mpfi_load_string(MPFI *ptr_s, const char *dump)
{
  int i;
  char *left;
  i = 0;
  while (dump[i] != ' ') {
    i++;
  }
  i++;
  left = malloc(sizeof(char) * i);
  strncpy(left, dump, i - 1);
  left[i - 1] = '\0';
  r_mpfr_load_string(r_mpfi_left_ptr(ptr_s), left);
  free(left);
  r_mpfr_load_string(r_mpfi_right_ptr(ptr_s), dump + i);
}

static VALUE r_mpfi_marshal_load(VALUE self, VALUE dump_string)
{
  MPFI *ptr_s;
  char *dump;
  r_mpfi_get_struct(ptr_s, self);
  Check_Type(dump_string, T_STRING);
  dump = RSTRING_PTR(dump_string);
  r_mpfi_load_string(ptr_s, dump);
  return self;
}

/* ------------------------------ allocation end ------------------------------ */

/* ------------------------------ Rounding Modes and Precision Handling start ------------------------------*/

/* Need to consider returned value later. */
/*
  Set the precision of self.
  Notice that mpfi_set_prec is different from reference manual. This is strange.
*/
static VALUE r_mpfi_set_prec (VALUE self, VALUE prec)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  /* if(mpfi_set_prec(ptr_self, NUM2INT(prec)) != 0){ */
  /*   rb_raise(rb_eRuntimeError, "Memory allocation failed for mpfi_set_prec."); */
  /* } */
  mpfi_set_prec(ptr_self, NUM2INT(prec));
  return self;
}

/* Need to consider returned value later. */
/* Return precision of self. */
static VALUE r_mpfi_get_prec (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  return INT2NUM((int)mpfi_get_prec(ptr_self));  
}

/* Need to consider returned value later. */
/* mpfi_round_prec(self, prec) */
static VALUE r_mpfi_round_prec (VALUE self, VALUE prec)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_set_function_state(mpfi_round_prec(ptr_self, NUM2INT(prec)));
  return self;
}
/* ------------------------------ Rounding Modes and Precision Handling end ------------------------------*/

/* ------------------------------ string start ------------------------------ */

/* String for inspect. */
static VALUE r_mpfi_inspect(VALUE self)
{
  MPFI *ptr_s;
  r_mpfi_get_struct(ptr_s, self);
  char *ret_str;
  if (!mpfr_asprintf(&ret_str, "#<MPFI:%lx,['%.Re %.Re'],%d>",
		    NUM2LONG(rb_funcall(self, object_id, 0)), r_mpfi_left_ptr(ptr_s),
		    r_mpfi_right_ptr(ptr_s), mpfi_get_prec(ptr_s))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

/* Return array having two strings to which endpoints is converted. */
static VALUE r_mpfi_to_str_ary(VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  char *ret_str1, *ret_str2;
  if (!mpfr_asprintf(&ret_str1, "%.Re", r_mpfi_left_ptr(ptr_self))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  if (!mpfr_asprintf(&ret_str2, "%.Re", r_mpfi_right_ptr(ptr_self))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  VALUE str1 = rb_str_new2(ret_str1), str2 = rb_str_new2(ret_str2);
  mpfr_free_str(ret_str1);
  mpfr_free_str(ret_str2);
  return rb_ary_new3(2, str1, str2);
}

/* Return array having two strings to which endpoints are converted by mpfr_asprintf with format_str. */
static VALUE r_mpfi_to_strf_ary(VALUE self, VALUE format_str)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  char *format = StringValuePtr(format_str);
  char *ret_str1, *ret_str2;
  if (!mpfr_asprintf(&ret_str1, format, r_mpfi_left_ptr(ptr_self))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  if (!mpfr_asprintf(&ret_str2, format, r_mpfi_right_ptr(ptr_self))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  VALUE str1 = rb_str_new2(ret_str1), str2 = rb_str_new2(ret_str2);
  mpfr_free_str(ret_str1);
  mpfr_free_str(ret_str2);
  return rb_ary_new3(2, str1, str2);
}

/* ------------------------------ string end ------------------------------ */

/* ------------------------------ Basic Arithmetic Functions start ------------------------------ */

/* Return self + p1. */
static VALUE r_mpfi_add (VALUE self, VALUE other)
{
  VALUE val_ret;
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_make_struct_init(val_ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    mpfi_add(ptr_ret, ptr_self, ptr_other);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpfi_add_fr(ptr_ret, ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    mpfi_add_si(ptr_ret, ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    mpfi_add_d(ptr_ret, ptr_self, NUM2DBL(other));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    mpfi_add(ptr_ret, ptr_self, ptr_other);
  }
  return val_ret;
}

/* Return self - p1. */
static VALUE r_mpfi_sub (VALUE self, VALUE other)
{
  VALUE val_ret;
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_make_struct_init(val_ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    mpfi_sub(ptr_ret, ptr_self, ptr_other);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpfi_sub_fr(ptr_ret, ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    mpfi_sub_si(ptr_ret, ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    mpfi_sub_d(ptr_ret, ptr_self, NUM2DBL(other));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    mpfi_sub(ptr_ret, ptr_self, ptr_other);
  }
  return val_ret;
}

/* Return self * p1. */
static VALUE r_mpfi_mul (VALUE self, VALUE other)
{
  VALUE val_ret;
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_make_struct_init(val_ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    mpfi_mul(ptr_ret, ptr_self, ptr_other);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpfi_mul_fr(ptr_ret, ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    mpfi_mul_si(ptr_ret, ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    mpfi_mul_d(ptr_ret, ptr_self, NUM2DBL(other));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    mpfi_mul(ptr_ret, ptr_self, ptr_other);
  }
  return val_ret;
}

/* Return self / p1. */
static VALUE r_mpfi_div (VALUE self, VALUE other)
{
  VALUE val_ret;
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_make_struct_init(val_ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    mpfi_div(ptr_ret, ptr_self, ptr_other);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    mpfi_div_fr(ptr_ret, ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    mpfi_div_si(ptr_ret, ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    mpfi_div_d(ptr_ret, ptr_self, NUM2DBL(other));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    mpfi_div(ptr_ret, ptr_self, ptr_other);
  }
  return val_ret;
}

/* mpfi_mul_2si(ret, self, p1) */
static VALUE r_mpfi_mul_2si (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  mpfi_mul_2si(ptr_ret, ptr_self, NUM2INT(argv[0]));
  return val_ret;
}

/* mpfi_div_2si(ret, self, p1) */
static VALUE r_mpfi_div_2si (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  mpfi_div_2si(ptr_ret, ptr_self, NUM2INT(argv[0]));
  return val_ret;
}

/* mpfi_neg(ret, self) */
static VALUE r_mpfi_neg(int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  mpfi_neg(ptr_ret, ptr_self);
  return val_ret;
}

/* mpfi_inv(ret, self) */
static VALUE r_mpfi_inv(int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  mpfi_inv(ptr_ret, ptr_self);
  return val_ret;
}

/* mpfi_abs(ret, self) */
static VALUE r_mpfi_abs(int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  mpfi_abs(ptr_ret, ptr_self);
  return val_ret;
}

/* ------------------------------ Basic Arithmetic Functions end ------------------------------ */

/* ------------------------------ Comparison Functions start ------------------------------ */

/* For MPFI instance, use mpfi_cmp. */
static VALUE r_mpfi_cmp (VALUE self, VALUE other)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  int ret;

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    ret = mpfi_cmp(ptr_self, ptr_other);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    ret = mpfi_cmp_fr(ptr_self, ptr_other);
  }else if(TYPE(other) == T_FIXNUM){
    ret = mpfi_cmp_si(ptr_self, FIX2LONG(other));
  }else if(TYPE(other) == T_FLOAT){
    ret = mpfi_cmp_d(ptr_self, NUM2DBL(other));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    ret = mpfi_cmp(ptr_self, ptr_other);
  }
  return NUM2INT(ret);
}

/* Return true if mpfi_is_pos(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_pos (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_pos(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_strictly_pos(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_strictly_pos (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_strictly_pos(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_nonneg(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_nonneg (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_nonneg(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_neg(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_neg (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_neg(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_strictly_neg(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_strictly_neg (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_strictly_neg(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_nonpos(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_nonpos (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_nonpos(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_zero(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_zero (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_is_zero(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_has_zero(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_has_zero (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_has_zero(ptr_self) > 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_nan_p(self) != 0. Otherwise, nil. */
static VALUE r_mpfi_nan_p (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_nan_p(ptr_self) != 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_inf_p(self) != 0. Otherwise, nil. */
static VALUE r_mpfi_inf_p (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_inf_p(ptr_self) != 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_bounded_p(self) != 0. Otherwise, nil. */
static VALUE r_mpfi_bounded_p (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if (mpfi_bounded_p(ptr_self) != 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/*
  Return true if self.right equals other.right and self.left equals other.right
  by mpfr_equal_p.
*/
static VALUE r_mpfi_equal_p (VALUE self, VALUE other)
{
  MPFI *ptr_self, *ptr_other;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_other, other);
  if (mpfr_equal_p(r_mpfi_left_ptr(ptr_self), r_mpfi_left_ptr(ptr_other)) != 0 &&
      mpfr_equal_p(r_mpfi_right_ptr(ptr_self), r_mpfi_right_ptr(ptr_other)) != 0) {
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* ------------------------------ Comparison Functions end ------------------------------ */

/* ------------------------------ Interval Functions with Floating-point Results start ------------------------------ */

/* mpfi_diam_abs(ret, self) */
static VALUE r_mpfi_diam_abs (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_diam_abs(ptr_ret, ptr_self));
  return val_ret;
}

/* mpfi_diam_rel(ret, self) */
static VALUE r_mpfi_diam_rel (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_diam_rel(ptr_ret, ptr_self));
  return val_ret;
}

/* mpfi_diam(ret, self) */
static VALUE r_mpfi_diam (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_diam(ptr_ret, ptr_self));
  return val_ret;
}

/* mpfi_mag(ret, self) */
static VALUE r_mpfi_mag (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_mag(ptr_ret, ptr_self));
  return val_ret;
}

/* mpfi_mig(ret, self) */
static VALUE r_mpfi_mig (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_mig(ptr_ret, ptr_self));
  return val_ret;
}

/* mpfi_mid(ret, self) */
static VALUE r_mpfi_mid (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_mid(ptr_ret, ptr_self));
  return val_ret;
}

/* Return MPFI instance of which value is middle of self. */
static VALUE r_mpfi_mid_interval (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  mpfi_mid_interval(ptr_ret, ptr_self);
  return val_ret;
}

/* mpfi_alea(ret, self) */
static VALUE r_mpfi_alea (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  mpfi_alea(ptr_ret, ptr_self);
  return val_ret;
}

/* ------------------------------ Interval Functions with Floating-point Results end ------------------------------ */

/* ------------------------------ Conversion Functions start ------------------------------ */

/* Return float by mpfi_get_d(self). */
static VALUE r_mpfi_get_d(VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  return rb_float_new(mpfi_get_d(ptr_self));
}

/* Return MPFR by mpfi_get_fr(ret, self). */
static VALUE r_mpfi_get_fr(VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init2(val_ret, ptr_ret, mpfi_get_prec(ptr_self));
  mpfi_get_fr(ptr_ret, ptr_self);
  return val_ret;
}
/* ------------------------------ Conversion Functions end ------------------------------ */

/* ------------------------------ Functions Operating on Endpoints start ------------------------------ */

/* mpfi_get_left(ret, self) */
static VALUE r_mpfi_get_left (VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_self;
  MPFR *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_ret, mpfi_get_prec(ptr_self));
  mpfi_get_left(ptr_ret, ptr_self);
  return val_ret;
}

/* mpfi_get_right(ret, self) */
static VALUE r_mpfi_get_right (VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_self;
  MPFR *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_ret, ptr_ret, mpfi_get_prec(ptr_self));
  mpfi_get_right(ptr_ret, ptr_self);
  return val_ret;
}

/* Return true if mpfi_revert_if_needed(self) != 0. Otherwise, nil. */
static VALUE r_mpfi_revert_if_needed (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if(mpfi_revert_if_needed(ptr_self) != 0){
    return Qtrue;
  }else{
    return Qnil;
  };
}

/* Extend the interval so that it contains other. */
static VALUE r_mpfi_put (VALUE self, VALUE other)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);

  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    r_mpfi_set_function_state(mpfi_put(ptr_self, ptr_other));
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    r_mpfi_set_function_state(mpfi_put_fr(ptr_self, ptr_other));
  }else if(TYPE(other) == T_FIXNUM){
    r_mpfi_set_function_state(mpfi_put_si(ptr_self, FIX2LONG(other)));
  }else if(TYPE(other) == T_FLOAT){
    r_mpfi_set_function_state(mpfi_put_d(ptr_self, NUM2DBL(other)));
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    r_mpfi_set_function_state(mpfi_put(ptr_self, ptr_other));
  }
  return self;
}

/* Change two endpoints so that it are the same as a1 and a2. */
static VALUE r_mpfi_interv (VALUE self, VALUE a1, VALUE a2)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_set_interv_from_robjs(ptr_self, a1, a2);
  return self;
}

/* Return new MPFI of which endpoints are the same as p1 and p2. */
static VALUE r_mpfi_interval (int argc, VALUE *argv, VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(2, 3, argc, argv));
  r_mpfi_set_interv_from_robjs(ptr_ret, argv[0], argv[1]);
  return val_ret;
}

/* Return array having two MPFR numbers which are endpoints. */
static VALUE r_mpfi_endpoints (VALUE self)
{
  VALUE val_left, val_right;
  MPFI *ptr_self;
  MPFR *ptr_left, *ptr_right;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfr_make_struct_init2(val_left, ptr_left, mpfi_get_prec(ptr_self));
  r_mpfr_make_struct_init2(val_right, ptr_right, mpfi_get_prec(ptr_self));
  mpfi_get_left(ptr_left, ptr_self);
  mpfi_get_right(ptr_right, ptr_self);
  return rb_ary_new3(2, val_left, val_right);
}

/* ------------------------------ Functions Operating on Endpoints end ------------------------------ */

/* ------------------------------ Set Functions on Intervals start ------------------------------ */

/* This method corresponds to mpfi_is_strictly_inside. */
static VALUE r_mpfi_strictly_include (VALUE self, VALUE other)
{
  MPFI *ptr_self, *ptr_other;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_other, other);
  if(mpfi_is_strictly_inside(ptr_other, ptr_self) > 0){
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* This method corresponds to mpfi_is_inside. */
static VALUE r_mpfi_include (VALUE self, VALUE other)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  int result;
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, other))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, other);
    result = mpfi_is_inside(ptr_other, ptr_self);
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, other))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, other);
    result = mpfi_is_inside_fr(ptr_other, ptr_self);
  }else if(TYPE(other) == T_FIXNUM){
    result = mpfi_is_inside_si(FIX2LONG(other), ptr_self);
  }else if(TYPE(other) == T_FLOAT){
    result = mpfi_is_inside_d(NUM2DBL(other), ptr_self);
  }else{
    MPFI *ptr_other;
    volatile VALUE tmp_other = r_mpfi_new_fi_obj(other);
    r_mpfi_get_struct(ptr_other, tmp_other);
    result = mpfi_is_inside(ptr_other, ptr_self);
  }
  if(result > 0){
    return Qtrue;
  }else{
    return Qnil;
  }
}

/* Return true if mpfi_is_empty(self) > 0. Otherwise, nil. */
static VALUE r_mpfi_is_empty (VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  if(mpfi_is_empty(ptr_self) > 0){
    return Qtrue;
  }else{
    return Qnil;
  }
}

/*
  If the intersection of two intervals is empty, this method returns nil.
  Otherwise, it returns the intersection.
*/
static VALUE r_mpfi_intersect (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_a0, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_a0, argv[0]);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_intersect(ptr_ret, ptr_self, ptr_a0));
  if(mpfi_is_empty(ptr_ret) > 0){
    return Qnil;
  }else{
    return val_ret;
  }
}

/*
  This method returns the intersection of two intervals.
  The returned value may be empty interval.
*/
static VALUE r_mpfi_intersect2 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_a0, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_a0, argv[0]);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_intersect(ptr_ret, ptr_self, ptr_a0));
  return val_ret;
}

/* mpfi_union(ret, self, p1) */
static VALUE r_mpfi_union (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_a0, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  r_mpfi_get_struct(ptr_a0, argv[0]);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_union(ptr_ret, ptr_self, ptr_a0));
  return val_ret;
}

/* ------------------------------ Set Functions on Intervals end ------------------------------ */

/* ------------------------------ Miscellaneous Interval Functions start ------------------------------ */

/* mpfi_increase(self, p1) */
static VALUE r_mpfi_increase (VALUE self, VALUE a0)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  MPFR *ptr_a0;
  volatile VALUE tmp_a0 = r_mpfr_new_fr_obj(a0);
  r_mpfr_get_struct(ptr_a0, tmp_a0);
  r_mpfi_set_function_state(mpfi_increase(ptr_self, ptr_a0));
  return self;
}

/* mpfi_blow(ret, self, p1) */
static VALUE r_mpfi_blow (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret;
  r_mpfi_get_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_blow(ptr_ret, ptr_self, NUM2DBL(argv[0])));
  return val_ret;
}

/* Return array [ret1, ret2] by mpfi_bisect(ret1, ret2, self). */
static VALUE r_mpfi_bisect (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self, *ptr_ret1, *ptr_ret2;
  r_mpfi_get_struct(ptr_self, self);
  int prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  VALUE val_ret1, val_ret2;
  r_mpfi_make_struct_init2(val_ret1, ptr_ret1, prec);
  r_mpfi_make_struct_init2(val_ret2, ptr_ret2, prec);
  r_mpfi_set_function_state(mpfi_bisect(ptr_ret1, ptr_ret2, ptr_self));
  return rb_ary_new3(2, val_ret1, val_ret2);
}

/* Retrun 0 if this function puts subdivision to *ret. */
/* Otherwise, return -1. */
void r_mpfi_subdivision_func(int num, MPFI *ret[], mpfi_t x)
{
  int i;
  mpfr_t l;
  mpfr_init(l);
  mpfr_sub(l, r_mpfi_right_ptr(x), r_mpfi_left_ptr(x), GMP_RNDD);
  mpfr_div_si(l, l, num, GMP_RNDD);
  mpfr_t x_diam;
  mpfr_init(x_diam);
  mpfi_diam_abs(x_diam, x);
  if(mpfr_cmp(x_diam, l) > 0 && num > 1){
    mpfr_set(r_mpfi_left_ptr(ret[0]), r_mpfi_left_ptr(x), GMP_RNDN);
    mpfr_add(r_mpfi_right_ptr(ret[0]), r_mpfi_left_ptr(ret[0]), l, GMP_RNDU);

    for(i = 1; i < num - 1; i ++){
      mpfr_set(r_mpfi_left_ptr(ret[i]), r_mpfi_right_ptr(ret[i - 1]), GMP_RNDN);
      mpfr_add(r_mpfi_right_ptr(ret[i]), r_mpfi_left_ptr(ret[i]), l, GMP_RNDU);
    }

    mpfr_set(r_mpfi_left_ptr(ret[i]), r_mpfi_right_ptr(ret[i - 1]), GMP_RNDN);
    mpfr_set(r_mpfi_right_ptr(ret[i]), r_mpfi_right_ptr(x), GMP_RNDN);
  }else{
    mpfr_set(r_mpfi_left_ptr(ret[0]), r_mpfi_left_ptr(x), GMP_RNDN);
    mpfr_set(r_mpfi_right_ptr(ret[0]), r_mpfi_right_ptr(x), GMP_RNDN);
  }
  mpfr_clear(x_diam);
  mpfr_clear(l);
}

/* Return array having MPFI instances by subdividing. */
static VALUE r_mpfi_subdivision (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_self;
  r_mpfi_get_struct(ptr_self, self);
  int i, num = NUM2INT(argv[0]);
  int prec = r_mpfr_prec_from_optional_argument(1, 2, argc, argv);
  MPFI *f[num];
  VALUE vf[num];
  for(i = 0; i < num; i++){
    r_mpfi_make_struct_init2(vf[i], f[i], prec);
  }
  r_mpfi_subdivision_func(num, f, ptr_self);
  return rb_ary_new4(num, vf);
}
/* ------------------------------ Miscellaneous Interval Functions end ------------------------------ */

/* ------------------------------ Mathematical Basic Arithmetic Functions start ------------------------------ */

/* Addition. */
static VALUE r_mpfi_math_add (int argc, VALUE *argv, VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_a0, *ptr_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(2, 3, argc, argv));
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, argv[1]))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_add(ptr_ret, ptr_a0, ptr_other));
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_add_fr(ptr_ret, ptr_a0, ptr_other));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    r_mpfi_set_function_state(mpfi_add_si(ptr_ret, ptr_a0, FIX2LONG(argv[1])));
  }else if(TYPE(argv[1]) == T_FLOAT){
    r_mpfi_set_function_state(mpfi_add_d(ptr_ret, ptr_a0, NUM2DBL(argv[1])));
  }else{
    MPFI *ptr_a2;
    volatile VALUE tmp_argv1 = r_mpfi_new_fi_obj(argv[1]);
    r_mpfi_get_struct(ptr_a2, tmp_argv1);
    r_mpfi_set_function_state(mpfi_add(ptr_ret, ptr_a0, ptr_a2));
  }
  return val_ret;
}

/* Subtraction. */
static VALUE r_mpfi_math_sub (int argc, VALUE *argv, VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_a0, *ptr_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(2, 3, argc, argv));
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, argv[1]))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_sub(ptr_ret, ptr_a0, ptr_other));
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_sub_fr(ptr_ret, ptr_a0, ptr_other));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    r_mpfi_set_function_state(mpfi_sub_si(ptr_ret, ptr_a0, FIX2LONG(argv[1])));
  }else if(TYPE(argv[1]) == T_FLOAT){
    r_mpfi_set_function_state(mpfi_sub_d(ptr_ret, ptr_a0, NUM2DBL(argv[1])));
  }else{
    MPFI *ptr_a2;
    volatile VALUE tmp_argv1 = r_mpfi_new_fi_obj(argv[1]);
    r_mpfi_get_struct(ptr_a2, tmp_argv1);
    r_mpfi_set_function_state(mpfi_sub(ptr_ret, ptr_a0, ptr_a2));
  }
  return val_ret;
}

/* Multiplication. */
static VALUE r_mpfi_math_mul (int argc, VALUE *argv, VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_a0, *ptr_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(2, 3, argc, argv));
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, argv[1]))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_mul(ptr_ret, ptr_a0, ptr_other));
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_mul_fr(ptr_ret, ptr_a0, ptr_other));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    r_mpfi_set_function_state(mpfi_mul_si(ptr_ret, ptr_a0, FIX2LONG(argv[1])));
  }else if(TYPE(argv[1]) == T_FLOAT){
    r_mpfi_set_function_state(mpfi_mul_d(ptr_ret, ptr_a0, NUM2DBL(argv[1])));
  }else{
    MPFI *ptr_a2;
    volatile VALUE tmp_argv1 = r_mpfi_new_fi_obj(argv[1]);
    r_mpfi_get_struct(ptr_a2, tmp_argv1);
    r_mpfi_set_function_state(mpfi_mul(ptr_ret, ptr_a0, ptr_a2));
  }
  return val_ret;
}

/* Division. */
static VALUE r_mpfi_math_div (int argc, VALUE *argv, VALUE self)
{
  VALUE val_ret;
  MPFI *ptr_a0, *ptr_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(2, 3, argc, argv));
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  if(RTEST(rb_funcall(__mpfi_class__, eqq, 1, argv[1]))){
    MPFI *ptr_other;
    r_mpfi_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_div(ptr_ret, ptr_a0, ptr_other));
  }else if(RTEST(rb_funcall(__mpfr_class__, eqq, 1, argv[1]))){
    MPFR *ptr_other;
    r_mpfr_get_struct(ptr_other, argv[1]);
    r_mpfi_set_function_state(mpfi_div_fr(ptr_ret, ptr_a0, ptr_other));
  }else if(TYPE(argv[1]) == T_FIXNUM){
    r_mpfi_set_function_state(mpfi_div_si(ptr_ret, ptr_a0, FIX2LONG(argv[1])));
  }else if(TYPE(argv[1]) == T_FLOAT){
    r_mpfi_set_function_state(mpfi_div_d(ptr_ret, ptr_a0, NUM2DBL(argv[1])));
  }else{
    MPFI *ptr_a2;
    volatile VALUE tmp_argv1 = r_mpfi_new_fi_obj(argv[1]);
    r_mpfi_get_struct(ptr_a2, tmp_argv1);
    r_mpfi_set_function_state(mpfi_div(ptr_ret, ptr_a0, ptr_a2));
  }
  return val_ret;
}

/* mpfi_sqr(ret, p1) */
static VALUE r_mpfi_math_sqr (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_sqr(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_sqrt(ret, p1) */
static VALUE r_mpfi_math_sqrt (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_sqrt(ptr_ret, ptr_a0));
  return val_ret;
}

/* ------------------------------ Mathematical Basic Arithmetic Functions end ------------------------------ */

/* ------------------------------ Special Functions start ------------------------------ */

/* mpfi_log(ret, p1) */
static VALUE r_mpfi_math_log (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_log(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_exp(ret, p1) */
static VALUE r_mpfi_math_exp (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_exp(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_exp2(ret, p1) */
static VALUE r_mpfi_math_exp2 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_exp2(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_cos(ret, p1) */
static VALUE r_mpfi_math_cos (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_cos(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_sin(ret, p1) */
static VALUE r_mpfi_math_sin (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_sin(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_tan(ret, p1) */
static VALUE r_mpfi_math_tan (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_tan(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_acos(ret, p1) */
static VALUE r_mpfi_math_acos (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_acos(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_asin(ret, p1) */
static VALUE r_mpfi_math_asin (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_asin(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_atan(ret, p1) */
static VALUE r_mpfi_math_atan (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_atan(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_cosh(ret, p1) */
static VALUE r_mpfi_math_cosh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_cosh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_sinh(ret, p1) */
static VALUE r_mpfi_math_sinh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_sinh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_tanh(ret, p1) */
static VALUE r_mpfi_math_tanh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_tanh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_acosh(ret, p1) */
static VALUE r_mpfi_math_acosh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_acosh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_asinh(ret, p1) */
static VALUE r_mpfi_math_asinh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_asinh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_atanh(ret, p1) */
static VALUE r_mpfi_math_atanh (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_atanh(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_log1p(ret, p1) */
static VALUE r_mpfi_math_log1p (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_log1p(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_expm1(ret, p1) */
static VALUE r_mpfi_math_expm1 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_expm1(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_log2(ret, p1) */
static VALUE r_mpfi_math_log2 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_log2(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_log10(ret, p1) */
static VALUE r_mpfi_math_log10 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_a0, *ptr_ret;
  volatile VALUE tmp_argv0 = r_mpfi_new_fi_obj(argv[0]);
  r_mpfi_get_struct(ptr_a0, tmp_argv0);
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(1, 2, argc, argv));
  r_mpfi_set_function_state(mpfi_log10(ptr_ret, ptr_a0));
  return val_ret;
}

/* mpfi_const_log2(ret) */
static VALUE r_mpfi_math_const_log2 (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_ret;
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_const_log2(ptr_ret));
  return val_ret;
}

/* mpfi_const_pi(ret) */
static VALUE r_mpfi_math_const_pi (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_ret;
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_const_pi(ptr_ret));
  return val_ret;
}

/* mpfi_const_euler(ret) */
static VALUE r_mpfi_math_const_euler (int argc, VALUE *argv, VALUE self)
{
  MPFI *ptr_ret;
  VALUE val_ret;
  r_mpfi_make_struct_init2(val_ret, ptr_ret, r_mpfr_prec_from_optional_argument(0, 1, argc, argv));
  r_mpfi_set_function_state(mpfi_const_euler(ptr_ret));
  return val_ret;
}

/* ------------------------------ Special Functions end ------------------------------ */

void Init_mpfi()
{
  r_mpfi_class = rb_define_class("MPFI", rb_cNumeric);

  /* ------------------------------ function state start ------------------------------ */
  rb_define_class_variable(r_mpfi_class, CLASS_VAL_FUNCTION_STATE, INT2FIX(0));
  rb_define_singleton_method(r_mpfi_class, "function_state", r_mpfi_get_function_state, 0);
  /* ------------------------------ function state end ------------------------------ */

  /* ------------------------------ allocation start ------------------------------ */
  rb_define_global_function("MPFI", r_mpfi_global_new, -1);
  rb_define_alloc_func(r_mpfi_class, r_mpfi_alloc);
  rb_define_private_method(r_mpfi_class, "initialize", r_mpfi_initialize, -1);
  rb_define_private_method(r_mpfi_class, "initialize_copy", r_mpfi_initialize_copy, 1);

  rb_define_method(r_mpfi_class, "coerce", r_mpfi_coerce, 1);
  rb_define_method(r_mpfi_class, "set", r_mpfi_set, 1);
  rb_define_method(r_mpfi_class, "swap", r_mpfi_swap, 1);
  /* ------------------------------ allocation end ------------------------------ */

  /* ------------------------------ Rounding Modes and Precision Handling start ------------------------------*/
  rb_define_method(r_mpfi_class, "set_prec", r_mpfi_set_prec, 1);
  rb_define_method(r_mpfi_class, "get_prec", r_mpfi_get_prec, 0);
  rb_define_method(r_mpfi_class, "round_prec", r_mpfi_round_prec, 1);
  /* ------------------------------ Rounding Modes and Precision Handling end ------------------------------*/

  /* ------------------------------ string start ------------------------------ */
  rb_define_method(r_mpfi_class, "inspect", r_mpfi_inspect, 0);

  rb_define_method(r_mpfi_class, "marshal_dump", r_mpfi_marshal_dump, 0);
  rb_define_method(r_mpfi_class, "marshal_load", r_mpfi_marshal_load, 1);

  rb_define_method(r_mpfi_class, "to_str_ary", r_mpfi_to_str_ary, 0);
  rb_define_method(r_mpfi_class, "to_strf_ary", r_mpfi_to_strf_ary, 1);
  /* ------------------------------ string end ------------------------------ */

  /* ------------------------------ Basic Arithmetic Functions start ------------------------------ */
  rb_define_method(r_mpfi_class, "+", r_mpfi_add, 1);
  rb_define_method(r_mpfi_class, "-", r_mpfi_sub, 1);
  rb_define_method(r_mpfi_class, "*", r_mpfi_mul, 1);
  rb_define_method(r_mpfi_class, "/", r_mpfi_div, 1);

  rb_define_method(r_mpfi_class, "mul_2si", r_mpfi_mul_2si, -1);
  rb_define_method(r_mpfi_class, "div_2si", r_mpfi_div_2si, -1);

  rb_define_method(r_mpfi_class, "neg", r_mpfi_neg, -1);
  rb_define_method(r_mpfi_class, "inv", r_mpfi_inv, -1);
  rb_define_method(r_mpfi_class, "abs", r_mpfi_abs, -1);
  /* ------------------------------ Basic Arithmetic Functions end ------------------------------ */

  /* ------------------------------ Comparison Functions start ------------------------------ */
  rb_define_method(r_mpfi_class, "cmp", r_mpfi_cmp, 1);
  rb_define_method(r_mpfi_class, "is_pos", r_mpfi_is_pos, 0);
  rb_define_method(r_mpfi_class, "is_strictly_pos", r_mpfi_is_strictly_pos, 0);
  rb_define_method(r_mpfi_class, "is_nonneg", r_mpfi_is_nonneg, 0);
  rb_define_method(r_mpfi_class, "is_neg", r_mpfi_is_neg, 0);
  rb_define_method(r_mpfi_class, "is_strictly_neg", r_mpfi_is_strictly_neg, 0);
  rb_define_method(r_mpfi_class, "is_nonpos", r_mpfi_is_nonpos, 0);
  rb_define_method(r_mpfi_class, "is_zero", r_mpfi_is_zero, 0);
  rb_define_method(r_mpfi_class, "has_zero", r_mpfi_has_zero, 0);
  rb_define_method(r_mpfi_class, "nan_p", r_mpfi_nan_p, 0);
  rb_define_method(r_mpfi_class, "inf_p", r_mpfi_inf_p, 0);
  rb_define_method(r_mpfi_class, "bounded_p", r_mpfi_bounded_p, 0);
  rb_define_method(r_mpfi_class, "equal_p", r_mpfi_equal_p, 1);

  rb_define_alias(r_mpfi_class, "cmp", "<=>");
  rb_define_alias(r_mpfi_class, "pos?", "is_pos");
  rb_define_alias(r_mpfi_class, "strictly_pos?", "is_strictly_pos");
  rb_define_alias(r_mpfi_class, "nonneg?", "is_nonneg");
  rb_define_alias(r_mpfi_class, "neg?", "is_neg");
  rb_define_alias(r_mpfi_class, "strictly_neg?", "is_strictly_neg");
  rb_define_alias(r_mpfi_class, "nonpos?", "is_nonpos");
  rb_define_alias(r_mpfi_class, "zero?", "is_zero");
  rb_define_alias(r_mpfi_class, "has_zero?", "has_zero");
  rb_define_alias(r_mpfi_class, "nan?", "nan_p");
  rb_define_alias(r_mpfi_class, "inf?", "inf_p");
  rb_define_alias(r_mpfi_class, "bounded?", "bounded_p");
  rb_define_method(r_mpfi_class, "==", r_mpfi_equal_p, 1);
  /* ------------------------------ Comparison Functions end ------------------------------ */

  /* ------------------------------ Interval Functions with Floating-point Results start ------------------------------ */
  rb_define_method(r_mpfi_class, "diam_abs", r_mpfi_diam_abs, -1);
  rb_define_method(r_mpfi_class, "diam_rel", r_mpfi_diam_rel, -1);
  rb_define_method(r_mpfi_class, "diam", r_mpfi_diam, -1);
  rb_define_method(r_mpfi_class, "mag", r_mpfi_mag, -1);
  rb_define_method(r_mpfi_class, "mig", r_mpfi_mig, -1);
  rb_define_method(r_mpfi_class, "mid", r_mpfi_mid, -1);
  rb_define_method(r_mpfi_class, "mid_interval", r_mpfi_mid_interval, -1);
  rb_define_method(r_mpfi_class, "alea", r_mpfi_alea, -1);
  /* ------------------------------ Interval Functions with Floating-point Results end ------------------------------ */

  /* ------------------------------ Conversion Functions start ------------------------------ */
  rb_define_method(r_mpfi_class, "get_d", r_mpfi_get_d, 0);
  rb_define_method(r_mpfi_class, "get_fr", r_mpfi_get_fr, 0);
  rb_define_alias(r_mpfi_class, "to_f", "get_d");
  rb_define_alias(r_mpfi_class, "to_fr", "get_fr");
  /* ------------------------------ Conversion Functions end ------------------------------ */

  /* ------------------------------ Functions Operating on Endpoints start ------------------------------ */
  rb_define_method(r_mpfi_class, "get_left", r_mpfi_get_left, 0);
  rb_define_method(r_mpfi_class, "get_right", r_mpfi_get_right, 0);
  rb_define_method(r_mpfi_class, "revert_if_needed", r_mpfi_revert_if_needed, 0);
  rb_define_method(r_mpfi_class, "put", r_mpfi_put, 1);
  rb_define_method(r_mpfi_class, "interv", r_mpfi_interv, 2);

  rb_define_alias(r_mpfi_class, "left", "get_left");
  rb_define_alias(r_mpfi_class, "right", "get_right");
  rb_define_method(r_mpfi_class, "endpoints", r_mpfi_endpoints, 0);
  
  rb_define_singleton_method(r_mpfi_class, "interval", r_mpfi_interval, -1);
  /* ------------------------------ Functions Operating on Endpoints end ------------------------------ */

  /* ------------------------------ Set Functions on Intervals start ------------------------------ */
  rb_define_method(r_mpfi_class, "strictly_include?", r_mpfi_strictly_include, 1);
  rb_define_method(r_mpfi_class, "include?", r_mpfi_include, 1);
  
  rb_define_method(r_mpfi_class, "is_empty", r_mpfi_is_empty, 0);
  rb_define_method(r_mpfi_class, "intersect", r_mpfi_intersect, -1);
  rb_define_method(r_mpfi_class, "intersect2", r_mpfi_intersect2, -1);
  rb_define_method(r_mpfi_class, "union", r_mpfi_union, -1);

  rb_define_alias(r_mpfi_class, "empty?", "is_empty");
  /* ------------------------------ Set Functions on Intervals end ------------------------------ */

  /* ------------------------------ Miscellaneous Interval Functions start ------------------------------ */
  rb_define_method(r_mpfi_class, "increase", r_mpfi_increase, 1);
  rb_define_method(r_mpfi_class, "blow", r_mpfi_blow, -1);
  rb_define_method(r_mpfi_class, "bisect", r_mpfi_bisect, -1);
  rb_define_method(r_mpfi_class, "subdivision", r_mpfi_subdivision, -1);
  /* ------------------------------ Miscellaneous Interval Functions end ------------------------------ */
  
  r_mpfi_math = rb_define_module_under(r_mpfi_class, "Math");  

  /* ------------------------------ Mathematical Basic Arithmetic Functions start ------------------------------ */
  rb_define_module_function(r_mpfi_math, "add", r_mpfi_math_add, -1);
  rb_define_module_function(r_mpfi_math, "sub", r_mpfi_math_sub, -1);
  rb_define_module_function(r_mpfi_math, "mul", r_mpfi_math_mul, -1);
  rb_define_module_function(r_mpfi_math, "div", r_mpfi_math_div, -1);

  rb_define_module_function(r_mpfi_math, "sqr", r_mpfi_math_sqr, -1);
  rb_define_module_function(r_mpfi_math, "sqrt", r_mpfi_math_sqrt, -1);
  /* ------------------------------ Mathematical Basic Arithmetic Functions end ------------------------------ */

  /* ------------------------------ Special Functions start ------------------------------ */
  rb_define_module_function(r_mpfi_math, "log", r_mpfi_math_log, -1);
  rb_define_module_function(r_mpfi_math, "exp", r_mpfi_math_exp, -1);
  rb_define_module_function(r_mpfi_math, "exp2", r_mpfi_math_exp2, -1);
  rb_define_module_function(r_mpfi_math, "cos", r_mpfi_math_cos, -1);
  rb_define_module_function(r_mpfi_math, "sin", r_mpfi_math_sin, -1);
  rb_define_module_function(r_mpfi_math, "tan", r_mpfi_math_tan, -1);
  rb_define_module_function(r_mpfi_math, "acos", r_mpfi_math_acos, -1);
  rb_define_module_function(r_mpfi_math, "asin", r_mpfi_math_asin, -1);
  rb_define_module_function(r_mpfi_math, "atan", r_mpfi_math_atan, -1);
  rb_define_module_function(r_mpfi_math, "cosh", r_mpfi_math_cosh, -1);
  rb_define_module_function(r_mpfi_math, "sinh", r_mpfi_math_sinh, -1);
  rb_define_module_function(r_mpfi_math, "tanh", r_mpfi_math_tanh, -1);
  rb_define_module_function(r_mpfi_math, "acosh", r_mpfi_math_acosh, -1);
  rb_define_module_function(r_mpfi_math, "asinh", r_mpfi_math_asinh, -1);
  rb_define_module_function(r_mpfi_math, "atanh", r_mpfi_math_atanh, -1);
  rb_define_module_function(r_mpfi_math, "log1p", r_mpfi_math_log1p, -1);
  rb_define_module_function(r_mpfi_math, "expm1", r_mpfi_math_expm1, -1);
  rb_define_module_function(r_mpfi_math, "log2", r_mpfi_math_log2, -1);
  rb_define_module_function(r_mpfi_math, "log10", r_mpfi_math_log10, -1);
  rb_define_module_function(r_mpfi_math, "const_log2", r_mpfi_math_const_log2, -1);
  rb_define_module_function(r_mpfi_math, "const_pi", r_mpfi_math_const_pi, -1);
  rb_define_module_function(r_mpfi_math, "const_euler", r_mpfi_math_const_euler, -1);
  /* ------------------------------ Special Functions end ------------------------------ */

  eqq = rb_intern("===");
  to_s = rb_intern("to_s");
  new = rb_intern("new");
  class = rb_intern("class");
  object_id = rb_intern("object_id");
  method_defined = rb_intern("method_defined?");
  __mpfi_class__ = rb_eval_string("MPFI");
  __mpfr_class__ = rb_eval_string("MPFR");
  __sym_to_s__ = rb_eval_string(":to_s");
  __sym_to_str__ = rb_eval_string(":to_str");

}

