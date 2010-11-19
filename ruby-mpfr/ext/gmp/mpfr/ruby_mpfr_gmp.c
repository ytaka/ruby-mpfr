#include "ruby_mpfr_gmp.h"

/* Return a random number of MPFR object. */
VALUE r_gmprandstate_mpfr_urandomb2(int argc, VALUE *argv, VALUE self)
{
  MP_RANDSTATE *ptr_self;
  MPFR *ptr_return;
  VALUE val_ret;
  mp_prec_t prec;

  prec = r_mpfr_prec_from_optional_argument(0, 1, argc, argv);
  mprandstate_get_struct(self, ptr_self);
  r_mpfr_make_struct_init2(val_ret, ptr_return, prec);
  mpfr_urandomb(ptr_return, ptr_self);
  return val_ret;
}

void Init_mpfr_gmp()
{
  rb_define_method(cGMP_RandState, "mpfr_urandomb2", r_gmprandstate_mpfr_urandomb2, -1);
}
