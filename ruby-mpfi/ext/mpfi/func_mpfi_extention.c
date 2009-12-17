#include <mpfr.h>
#include <mpfi.h>
#include <mpfi_io.h>
#include "ruby_mpfi.h"
#include "func_mpfi_extention.h"

void mpfi_mid_interval(mpfi_t ret, mpfi_t x){
  mpfr_t left, right;
  mpfr_init2(left, mpfi_get_prec(x));
  mpfr_init2(right, mpfi_get_prec(x));

  mpfr_add(left, r_mpfi_left_ptr(x), r_mpfi_right_ptr(x), GMP_RNDD);
  mpfr_add(right, r_mpfi_left_ptr(x), r_mpfi_right_ptr(x), GMP_RNDU);

  mpfr_div_ui(r_mpfi_left_ptr(ret), left, 2, GMP_RNDD);
  mpfr_div_ui(r_mpfi_right_ptr(ret), right, 2, GMP_RNDU);

  mpfr_clear(left);
  mpfr_clear(right);
}

/* Retrun 0 if this function puts subdivision to *ret. */
/* Otherwise, return -1. */
int mpfi_subdivision(int num, mpfi_t *ret, mpfi_t x){
  int i, ret_val = -1;
  mpfr_t l;
  mpfr_init(l);
  mpfr_sub(l, r_mpfi_right_ptr(x), r_mpfi_left_ptr(x), GMP_RNDD);
  mpfr_div_si(l, l, num, GMP_RNDD);
  
  mpfr_t x_diam;
  mpfr_init(x_diam);
  mpfi_diam_abs(x_diam, x);
  if(mpfr_cmp(x_diam, l) > 0){
    mpfr_set(r_mpfi_left_ptr(*ret), r_mpfi_left_ptr(x), GMP_RNDN);
    mpfr_add(r_mpfi_right_ptr(*ret), r_mpfi_left_ptr(*ret), l, GMP_RNDU);

    for(i = 1; i < num - 1; i ++){
      mpfr_set(r_mpfi_left_ptr(*ret + i), r_mpfi_right_ptr(*ret + i - 1), GMP_RNDN);
      mpfr_add(r_mpfi_right_ptr(*ret + i), r_mpfi_left_ptr(*ret + i), l, GMP_RNDU);
    }

    mpfr_set(r_mpfi_left_ptr(*ret + i), r_mpfi_right_ptr(*ret + i - 1), GMP_RNDN);
    mpfr_set(r_mpfi_right_ptr(*ret + i), r_mpfi_right_ptr(x), GMP_RNDN);
    ret_val = 0;
  }
  
  mpfr_clear(x_diam);
  mpfr_clear(l);
  return ret_val;
}

