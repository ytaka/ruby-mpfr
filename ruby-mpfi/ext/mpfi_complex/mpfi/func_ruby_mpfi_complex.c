#include "func_ruby_mpfi_complex.h"

void mpfi_complex_init(MPFIComplex *x){
  x->re = ALLOC_N(MPFI, 1);
  x->im = ALLOC_N(MPFI, 1);
  mpfi_init(x->re);
  mpfi_init(x->im);
}

void mpfi_complex_clear(MPFIComplex *x){
  mpfi_clear(x->re);
  mpfi_clear(x->im);
  free(x->re);
  free(x->im);
}

void mpfi_complex_set_zeros(MPFIComplex *x){
  mpfi_set_si(x->re, 0);
  mpfi_set_si(x->im, 0);
}

void mpfi_complex_set_real_part(MPFIComplex *x, MPFI *a){
  mpfi_set(x->re, a);
}

void mpfi_complex_set_imaginary_part(MPFIComplex *x, MPFI *a){
  mpfi_set(x->im, a);
}

void mpfi_complex_set(MPFIComplex *new, MPFIComplex *x){
  mpfi_set(new->re, x->re);
  mpfi_set(new->im, x->im);
}

void mpfi_complex_conjugate(MPFIComplex *new, MPFIComplex *x){
  mpfi_set(new->re, x->re);
  mpfi_neg(new->im, x->im);
}

void mpfi_complex_add(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y){
  mpfi_add(new->re, x->re, y->re);
  mpfi_add(new->im, x->im, y->im);
}

void mpfi_complex_sub(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y){
  mpfi_sub(new->re, x->re, y->re);
  mpfi_sub(new->im, x->im, y->im);
}

void mpfi_complex_mul(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y){
  MPFI *t1, *t2, *t_re;
  r_mpfi_temp_alloc_init2(t1, mpfi_complex_get_prec(new));
  r_mpfi_temp_alloc_init2(t2, mpfi_complex_get_prec(new));
  r_mpfi_temp_alloc_init2(t_re, mpfi_complex_get_prec(new));

  mpfi_mul(t1, x->re, y->re);
  mpfi_mul(t2, x->im, y->im);
  mpfi_sub(t_re, t1, t2);

  mpfi_mul(t1, x->re, y->im);
  mpfi_mul(t2, x->im, y->re);
  mpfi_add(new->im, t1, t2);

  mpfi_set(new->re, t_re);

  r_mpfi_temp_free(t1);
  r_mpfi_temp_free(t2);
  r_mpfi_temp_free(t_re);
}

void mpfi_complex_div(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y){
  MPFI *t1, *t2, *t_re, *deno;
  r_mpfi_temp_alloc_init2(t1, mpfi_complex_get_prec(new));
  r_mpfi_temp_alloc_init2(t2, mpfi_complex_get_prec(new));
  r_mpfi_temp_alloc_init2(t_re, mpfi_complex_get_prec(new));
  r_mpfi_temp_alloc_init2(deno, mpfi_complex_get_prec(new));

  mpfi_mul(t2, y->re, y->re);
  mpfi_mul(t1, y->im, y->im);
  mpfi_add(deno, t2, t1);

  mpfi_mul(t2, x->re, y->re);
  mpfi_mul(t1, x->im, y->im);
  mpfi_add(t2, t2, t1);
  mpfi_div(t_re, t2, deno);

  mpfi_mul(t2, x->im, y->re);
  mpfi_mul(t1, x->re, y->im);
  mpfi_sub(t2, t2, t1);
  mpfi_div(new->im, t2, deno);

  mpfi_set(new->re, t_re);

  r_mpfi_temp_free(t1);
  r_mpfi_temp_free(t2);
  r_mpfi_temp_free(t_re);
  r_mpfi_temp_free(deno);
}

/* void mpfi_complex_mul_real(MPFIComplex *new, MPFIComplex *x, MPFI *y){ */
/*   mpfi_mul(new->re, x->re, y); */
/*   mpfi_mul(new->im, x->im, y); */
/* } */

/* void mpfi_complex_mul_pure_imaginary(MPFIComplex *new, MPFIComplex *x, MPFI *y){ */
/*   mpfi_mul(new->re, x->im, y); */
/*   mpfi_neg(new->re, new->re); */
/*   mpfi_mul(new->im, x->re, y); */
/* } */

/* void mpfi_complex_div_real(MPFIComplex *new, MPFIComplex *x, MPFI *y){ */
/*   mpfi_div(new->re, x->re, y); */
/*   mpfi_div(new->im, x->im, y); */
/* } */

/* void mpfi_complex_div_pure_imaginary(MPFIComplex *new, MPFIComplex *x, MPFI *y){ */
/*   mpfi_div(new->re, x->im, y); */
/*   mpfi_div(new->im, x->re, y); */
/*   mpfi_neg(new->im, new->im); */
/* } */

/* void mpfi_complex_abs(MPFI *new, MPFIComplex *x){ */
/*   MPFI *tmp; */
/*   r_mpfi_temp_alloc_init(tmp); */
/*   mpfi_mul(tmp, x->re, x->re); */
/*   mpfi_mul(new, x->im, x->im); */
/*   mpfi_add(new, new, tmp); */
/*   mpfi_sqrt(new, new); */
/*   r_mpfi_temp_free(tmp); */
/* } */
