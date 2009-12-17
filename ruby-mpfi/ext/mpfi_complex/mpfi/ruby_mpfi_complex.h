#ifndef __RUBY_MPFI_COMPLEX__
#define __RUBY_MPFI_COMPLEX__

#include "func_ruby_mpfi_complex.h"

#define r_mpfi_make_complex_struct(ruby_var, mpfi_complex_var) { ruby_var = Data_Make_Struct(r_mpfi_complex, MPFIComplex, 0, r_mpfi_complex_free, mpfi_complex_var); }
#define r_mpfi_make_complex_struct_init(ruby_var, mpfi_complex_var) { ruby_var = Data_Make_Struct(r_mpfi_complex, MPFIComplex, 0, r_mpfi_complex_free, mpfi_complex_var); mpfi_complex_init(mpfi_complex_var);}
#define r_mpfi_get_complex_struct(mpfi_complex_var, ruby_var) { Data_Get_Struct(ruby_var, MPFIComplex, mpfi_complex_var); }

#define r_mpfi_complex_temp_alloc_init(mpfi_complex_var) { mpfi_complex_var = ALLOC_N(MPFIComplex, 1); mpfi_complex_init(mpfi_complex_var); }
#define r_mpfi_complex_temp_free(mpfi_complex_var) { mpfi_complex_clear(mpfi_complex_var); free(mpfi_complex_var); }

void r_mpfi_complex_free(void *ptr);

#endif
