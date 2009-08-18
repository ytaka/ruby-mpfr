#ifndef _RUBY_MPC_H_
#define _RUBY_MPC_H_

#include <stdio.h>
#include <ruby.h>
#include <mpfr.h>
#include <mpc.h>
#include "../../ruby-mpfr/ext/ruby_mpfr.h"

typedef __mpc_struct MPC;

VALUE r_mpc_class, r_mpc_math;

#define r_mpc_make_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpc_class, MPC, 0, r_mpc_free, c_var); }
#define r_mpc_make_struct_init(ruby_var, c_var) { r_mpc_make_struct(ruby_var, c_var); mpc_init(c_var); }
#define r_mpc_make_struct_init2(ruby_var, c_var, prec) { r_mpc_make_struct(ruby_var, c_var); mpc_init2(c_var, prec); }
#define r_mpc_get_struct(c_var, ruby_var) { Data_Get_Struct(ruby_var, MPC, c_var); }

#define r_mpc_temp_alloc(c_var) { c_var=ALLOC_N(MPC, 1); }
#define r_mpc_temp_alloc_init(c_var) { r_mpc_temp_alloc(c_var); mpc_init(c_var); }
#define r_mpc_temp_alloc_init2(c_var, prec) { r_mpc_temp_alloc(c_var); mpc_init2(c_var, prec); }
#define r_mpc_temp_free(c_var) { mpc_clear(c_var); free(c_var); }

#define r_mpc_rnd_from_value(rnd) (mp_rnd_t)NUM2INT(rnd)

void r_mpc_free(void *ptr);
VALUE r_mpc_new_c_obj(VALUE obj);
/* VALUE r_mpc_new_robj(MPC *ptr); */
/* void r_mpc_set_robj(MPC *ptr, VALUE obj); */

/* Arguments */
mpc_rnd_t r_mpc_rnd_from_optional_argument(int min, int max, int argc, VALUE *argv);
mpc_rnd_t r_mpc_prec_from_optional_argument(int min, int max, int argc, VALUE *argv);
void r_mpc_get_rnd_prec_from_optional_arguments(mpc_rnd_t *rnd, mp_prec_t *prec, int min, int max,
						int argc, VALUE *argv);

void mpc_extended_set_default_rounding_mode (mpc_rnd_t rnd_mode);
mpc_rnd_t mpc_extended_get_default_rounding_mode (void);
mp_prec_t mpc_extended_get_max_prec (mpc_t x);
void r_mpc_set_function_state(int num);

#endif /* _RUBY_MPC_H_ */
