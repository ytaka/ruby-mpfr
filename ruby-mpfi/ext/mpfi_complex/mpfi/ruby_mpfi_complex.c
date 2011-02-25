#include "ruby_mpfi_complex.h"

static ID object_id;

void r_mpfi_complex_free(void *ptr){
  mpfi_complex_clear(ptr);
  free(ptr);
}

/* Allocation function for MPF::Complex. */
static VALUE r_mpfi_complex_alloc (VALUE self){
  MPFIComplex *ptr;
  r_mpfi_make_complex_struct(self, ptr);
  return self;
}

static void r_mpfi_complex_set_initial_value(MPFIComplex *ptr, int argc, VALUE *argv)
{
  mpfi_complex_init(ptr);
  switch (argc) {
  case 0:
    mpfi_complex_set_zeros(ptr);    
    break;
  case 1:
    r_mpfi_set_robj(ptr->re, argv[0]);
    mpfi_set_si(ptr->im, 0);
    break;
  case 2:
    r_mpfi_set_robj(ptr->re, argv[0]);
    r_mpfi_set_robj(ptr->im, argv[1]);
    break;
  default:
    rb_raise(rb_eArgError, "Number of MPF::Complex.new arguments must be lesser than three.");
    break;
  }  
}

/* Return new MPFI::Complex instance. The same arguments as MPFI::Complex.new is acceptable. */
static VALUE r_mpfi_complex_global_new(int argc, VALUE *argv, VALUE self)
{
  MPFIComplex *ptr;
  VALUE val;
  r_mpfi_make_complex_struct(val, ptr);
  r_mpfi_complex_set_initial_value(ptr, argc, argv);
  return val;
}

/* Initialization function for MPF::Complex. */
static VALUE r_mpfi_complex_initialize (int argc, VALUE *argv, VALUE self){
  MPFIComplex *ptr;
  r_mpfi_get_complex_struct(ptr, self);
  r_mpfi_complex_set_initial_value(ptr, argc, argv);
  return Qtrue;
}

/* initialize_copy */
static VALUE r_mpfi_complex_initialize_copy (VALUE self, VALUE other){
  MPFIComplex *ptr_self, *ptr_other;
  r_mpfi_get_complex_struct(ptr_self, self);
  r_mpfi_get_complex_struct(ptr_other, other);
  mpfi_complex_init(ptr_self);
  mpfi_complex_set(ptr_self, ptr_other);
  return Qtrue;
}

/* Return real part. */
static VALUE r_mpfi_complex_real (VALUE self){
  MPFIComplex *ptr_self;
  r_mpfi_get_complex_struct(ptr_self, self);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  mpfi_set(ptr_ret, ptr_self->re);
  return ret;
}

/* Return imaginary part. */
static VALUE r_mpfi_complex_imaginary (VALUE self){
  MPFIComplex *ptr_self;
  r_mpfi_get_complex_struct(ptr_self, self);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  mpfi_set(ptr_ret, ptr_self->im);
  return ret;
}

/* If _p1_ is 0, return real part. If _p1_ is 1, return imaginary part. */
static VALUE r_mpfi_complex_element (VALUE self, VALUE arg){
  MPFIComplex *ptr_self;
  r_mpfi_get_complex_struct(ptr_self, self);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  switch(NUM2INT(arg)){
  case 0:
    mpfi_set(ptr_ret, ptr_self->re);
    break;
  case 1:
    mpfi_set(ptr_ret, ptr_self->im);
    break;
  default:
    rb_raise(rb_eArgError, "Argument must be 0 or 1.");
    break;
  }
  return ret;
}

/* Return conjugate complex number. */
static VALUE r_mpfi_complex_conjugate (VALUE self){
  MPFIComplex *ptr_self, *ptr_ret;
  r_mpfi_get_complex_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_make_complex_struct_init(ret, ptr_ret);
  mpfi_complex_conjugate(ptr_ret, ptr_self);
  return ret;
}

/* static VALUE r_mpfi_complex_abs (VALUE self){ */
/*   MPFIComplex *ptr_self; */
/*   r_mpfi_get_complex_struct(ptr_self, self); */
/*   VALUE ret; */
/*   MPFI *ptr_ret; */
/*   r_mpfi_make_struct_init(ret, ptr_ret); */
/*   mpfi_complex_abs(ptr_ret, ptr_self); */
/*   return ret; */
/* } */


/* String for inspect. */
static VALUE r_mpfi_complex_inspect(VALUE self){
  MPFIComplex *ptr_s;
  r_mpfi_get_complex_struct(ptr_s, self);
  char *ret_str;
  if (!mpfr_asprintf(&ret_str, "#<MPFI:%lx,['%.Re %.Re', '%.Re %.Re'], [%d, %d]>",
		     NUM2LONG(rb_funcall(self, object_id, 0)), r_mpfi_left_ptr(ptr_s->re),
		     r_mpfi_right_ptr(ptr_s->re), r_mpfi_left_ptr(ptr_s->im), r_mpfi_right_ptr(ptr_s->im),
		     mpfi_get_prec(ptr_s->re), mpfi_get_prec(ptr_s->im))) {
    rb_raise(rb_eFatal, "Can not allocate a string by mpfr_asprintf.");
  }
  VALUE ret_val = rb_str_new2(ret_str);
  mpfr_free_str(ret_str);
  return ret_val;
}

/* Return array including strings which the elements is converted to by _p1_. */
static VALUE r_mpfi_complex_str_ary(VALUE self, VALUE format_str){
  MPFIComplex *ptr_self;
  r_mpfi_get_complex_struct(ptr_self, self);
  char *format = StringValuePtr(format_str), *tmp_str;
  VALUE ret_val[2];
  gmp_asprintf(&tmp_str, format, ptr_self->re);
  ret_val[0] = rb_str_new2(tmp_str);
  free(tmp_str);
  gmp_asprintf(&tmp_str, format, ptr_self->im);
  ret_val[1] = rb_str_new2(tmp_str);
  free(tmp_str);
  return rb_ary_new4(2, ret_val);
}

/* Return _self_ + _p1_.*/
static VALUE r_mpfi_complex_add (VALUE self, VALUE other){
  MPFIComplex *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_complex_struct(ptr_self, self);
  r_mpfi_get_complex_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_complex_struct_init(ret, ptr_ret);
  mpfi_complex_add(ptr_ret, ptr_self, ptr_other);
  return ret;
}

/* Return _self_ - _p1_.*/
static VALUE r_mpfi_complex_sub (VALUE self, VALUE other){
  MPFIComplex *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_complex_struct(ptr_self, self);
  r_mpfi_get_complex_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_complex_struct_init(ret, ptr_ret);
  mpfi_complex_sub(ptr_ret, ptr_self, ptr_other);
  return ret;
}

/* Return _self_ * _p1_.*/
static VALUE r_mpfi_complex_mul (VALUE self, VALUE other){
  MPFIComplex *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_complex_struct(ptr_self, self);
  r_mpfi_get_complex_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_complex_struct_init(ret, ptr_ret);
  mpfi_complex_mul(ptr_ret, ptr_self, ptr_other);
  return ret;
}

/* Return _self_ / _p1_.*/
static VALUE r_mpfi_complex_div (VALUE self, VALUE other){
  MPFIComplex *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_complex_struct(ptr_self, self);
  r_mpfi_get_complex_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_complex_struct_init(ret, ptr_ret);
  mpfi_complex_div(ptr_ret, ptr_self, ptr_other);
  return ret;
}


void Init_complex(){
  VALUE tmp_mpfi_class = rb_define_class("MPFI", rb_cNumeric);

  r_mpfi_complex = rb_define_class_under(tmp_mpfi_class, "Complex", rb_cObject);

  rb_define_singleton_method(r_mpfi_complex, "Complex", r_mpfi_complex_global_new, -1);
  rb_define_alloc_func(r_mpfi_complex, r_mpfi_complex_alloc);
  rb_define_private_method(r_mpfi_complex, "initialize", r_mpfi_complex_initialize, -1);
  rb_define_private_method(r_mpfi_complex, "initialize_copy", r_mpfi_complex_initialize_copy, 1);

  rb_define_method(r_mpfi_complex, "real", r_mpfi_complex_real, 0);
  rb_define_method(r_mpfi_complex, "imaginary", r_mpfi_complex_imaginary, 0);
  rb_define_method(r_mpfi_complex, "element", r_mpfi_complex_element, 1);
  rb_define_method(r_mpfi_complex, "conjugate", r_mpfi_complex_conjugate, 0);
  rb_define_alias(r_mpfi_complex, "[]", "element");


  rb_define_method(r_mpfi_complex, "inspect", r_mpfi_complex_inspect, 0);
  rb_define_method(r_mpfi_complex, "str_ary", r_mpfi_complex_str_ary, 1);


  rb_define_method(r_mpfi_complex, "add", r_mpfi_complex_add, 1);
  rb_define_method(r_mpfi_complex, "sub", r_mpfi_complex_sub, 1);
  rb_define_method(r_mpfi_complex, "mul", r_mpfi_complex_mul, 1);
  rb_define_method(r_mpfi_complex, "div", r_mpfi_complex_div, 1);

  rb_define_alias(r_mpfi_complex, "+", "add");
  rb_define_alias(r_mpfi_complex, "-", "sub");
  rb_define_alias(r_mpfi_complex, "*", "mul");
  rb_define_alias(r_mpfi_complex, "/", "div");

  object_id = rb_intern("object_id");
    
}

