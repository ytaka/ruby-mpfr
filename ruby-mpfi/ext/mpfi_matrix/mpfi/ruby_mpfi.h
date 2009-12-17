#ifndef _RUBY_MPFI_H_
#define _RUBY_MPFI_H_

#include <stdio.h>
#include <ruby.h>
#include <mpfr.h>
#include <mpfi.h>
#include <mpfi_io.h>
#include "func_mpfi_extention.h"
#include "ruby_mpfr.h"

typedef __mpfi_struct MPFI;

VALUE r_mpfi_class, r_mpfi_math;;

#define r_mpfi_make_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfi_class, MPFI, 0, r_mpfi_free, c_var); }
#define r_mpfi_make_struct_init(ruby_var, c_var) { r_mpfi_make_struct(ruby_var, c_var); mpfi_init(c_var); }
#define r_mpfi_make_struct_init2(ruby_var, c_var, prec) { r_mpfi_make_struct(ruby_var, c_var); mpfi_init2(c_var, prec); }
#define r_mpfi_get_struct(c_var, ruby_var) { Data_Get_Struct(ruby_var, MPFI, c_var); }

#define r_mpfi_temp_alloc(c_var) { c_var=ALLOC_N(MPFI, 1); }
#define r_mpfi_temp_alloc_init(c_var) { r_mpfi_temp_alloc(c_var); mpfi_init(c_var); }
#define r_mpfi_temp_alloc_init2(c_var, prec) { r_mpfi_temp_alloc(c_var); mpfi_init2(c_var, prec); }
#define r_mpfi_temp_free(c_var) { mpfi_clear(c_var); free(c_var); }

#define r_mpfi_left_ptr(c_var) (&((c_var)->left))
#define r_mpfi_right_ptr(c_var) (&((c_var)->right))

void r_mpfi_free(void *ptr);
VALUE r_mpfi_make_new_fi_obj(MPFI *ptr);
VALUE r_mpfi_new_fi_obj(VALUE obj);
void r_mpfi_set_robj(MPFI *ptr, VALUE obj);

int r_mpfi_subdivision2(int num, MPFI *ret, mpfi_t x);

void r_mpfi_set_function_state(int num);
VALUE r_mpfr_get_function_state(VALUE self);

#endif /* _RUBY_MPFI_H_ */
