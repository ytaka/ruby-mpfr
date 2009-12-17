#include <stdio.h>
#include <ruby.h>
#include <mpfr.h>
#include <mpfi.h>
#include <mpfi_io.h>
#include "ruby_mpfi.h"

VALUE r_mpfi_complex;

typedef struct __MPFIComplex{
  MPFI *re;
  MPFI *im;
} MPFIComplex;

/* Although x->re and x->im can have different precision from each other for MPFIComplex x, */
/* we implement mpfi_complex_get_prec assuming that they have same presition. */
#define mpfi_complex_get_prec(x) mpfi_get_prec(x->re)

void mpfi_complex_init(MPFIComplex *x);
void mpfi_complex_clear(MPFIComplex *x);
void mpfi_complex_set_zeros(MPFIComplex *x);
void mpfi_complex_set_real_part(MPFIComplex *x, MPFI *a);
void mpfi_complex_set_imaginary_part(MPFIComplex *x, MPFI *a);
void mpfi_complex_set(MPFIComplex *new, MPFIComplex *x);
void mpfi_complex_conjugate(MPFIComplex *new, MPFIComplex *x);
void mpfi_complex_add(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y);
void mpfi_complex_sub(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y);
void mpfi_complex_mul(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y);
void mpfi_complex_div(MPFIComplex *new, MPFIComplex *x, MPFIComplex *y);

/* void mpfi_complex_mul_real(MPFIComplex *new, MPFIComplex *x, MPFI *y); */
/* void mpfi_complex_mul_pure_imaginary(MPFIComplex *new, MPFIComplex *x, MPFI *y); */
/* void mpfi_complex_div_real(MPFIComplex *new, MPFIComplex *x, MPFI *y); */
/* void mpfi_complex_div_pure_imaginary(MPFIComplex *new, MPFIComplex *x, MPFI *y); */
/* void mpfi_complex_abs(MPFI *new, MPFIComplex *x); */
