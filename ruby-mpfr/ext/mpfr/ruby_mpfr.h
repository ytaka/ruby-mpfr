#ifndef _RUBY_MPFR_H_
#define _RUBY_MPFR_H_

#include <ruby.h>
#include <mpfr.h>
#include <stdio.h>

typedef __mpfr_struct MPFR;

VALUE r_mpfr_class, r_mpfr_math;

#define r_mpfr_make_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfr_class, MPFR, 0, r_mpfr_free, c_var); }
#define r_mpfr_make_struct_init(ruby_var, c_var) { r_mpfr_make_struct(ruby_var, c_var); mpfr_init(c_var); }
#define r_mpfr_make_struct_init2(ruby_var, c_var, prec) { r_mpfr_make_struct(ruby_var, c_var); mpfr_init2(c_var, prec); }
#define r_mpfr_get_struct(c_var, ruby_var) { Data_Get_Struct(ruby_var, MPFR, c_var); }

#define r_mpfr_temp_alloc(var) { var=ALLOC_N(MPFR, 1); }
#define r_mpfr_temp_alloc_init(var) { r_mpfr_temp_alloc(var); mpfr_init(var); }
#define r_mpfr_temp_alloc_init2(var, prec) { r_mpfr_temp_alloc(var); mpfr_init2(var, prec); }
#define r_mpfr_temp_free(var) { mpfr_clear(var); free(var); }

#define r_mpfr_check_number(c_val) { if(mpfr_number_p(c_val) == 0) rb_raise(rb_eArgError, "Not an ordinary number."); }
#define r_mpfr_check_positive_number(c_val) { if(mpfr_number_p(c_val) == 0 && mpfr_sgn(c_val) <= 0) rb_raise(rb_eArgError, "Not a positive number."); }
#define r_mpfr_check_non_negative_number(c_val) { if(mpfr_number_p(c_val) == 0 && mpfr_sgn(c_val) < 0) rb_raise(rb_eArgError, "Not a non negative number."); }
#define r_mpfr_check_negative_number(c_val) { if(mpfr_number_p(c_val) == 0 && mpfr_sgn(c_val) >= 0) rb_raise(rb_eArgError, "Not a negative number."); }
#define r_mpfr_check_non_positive_number(c_val) { if(mpfr_number_p(c_val) == 0 && mpfr_sgn(c_val) > 0) rb_raise(rb_eArgError, "Not a non positive number."); }

void r_mpfr_free(void *ptr);
VALUE r_mpfr_make_new_fr_obj(MPFR *ptr);
VALUE r_mpfr_make_new_fr_obj2(MPFR *ptr, int prec);
VALUE r_mpfr_new_fr_obj(VALUE obj);
void r_mpfr_set_robj(MPFR *ptr, VALUE obj, mp_rnd_t rnd);
VALUE r_mpfr_robj_to_mpfr(VALUE obj, int argc, VALUE *argv);

mp_rnd_t r_mpfr_rnd_from_value(VALUE rnd);
mp_rnd_t r_mpfr_rnd_from_optional_argument(int min, int max, int argc, VALUE *argv);
mp_rnd_t r_mpfr_prec_from_optional_argument(int min, int max, int argc, VALUE *argv);
void r_mpfr_get_rnd_prec_from_optional_arguments(mp_rnd_t *rnd, mp_prec_t *prec, int min, int max, int argc, VALUE *argv);

char *r_mpfr_dump_to_string(MPFR *ptr_s);
void r_mpfr_load_string(MPFR *ptr_s, const char *dump);

#endif /* _RUBY_MPFR_H_ */

