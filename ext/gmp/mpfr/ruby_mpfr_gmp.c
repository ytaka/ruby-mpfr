#include "ruby_mpfr_gmp.h"

#if defined(MPFR) && defined(HAVE_MPFR_H)
#define R_MPF_GET_PREC(mpf) mpfr_get_prec(mpf)
#else
#define R_MPF_GET_PREC(mpf) mpf_get_prec(mpf)
#endif

static VALUE r_mpfr_to_mpf(int argc, VALUE *argv, VALUE self)
{
  VALUE ptr_return;
  MPFR *ptr_self;
  MP_FLOAT *ptr_mpf;
  mp_rnd_t rnd;
  rnd = r_mpfr_rnd_from_optional_argument(0, 1, argc, argv);
  r_mpfr_get_struct(ptr_self, self);
  mpf_make_struct_init(ptr_return, ptr_mpf, mpfr_get_prec(ptr_self));
#if defined(MPFR) && defined(HAVE_MPFR_H)
  mpfr_set(ptr_mpf, ptr_self, rnd);
#else
  mpfr_get_f(ptr_mpf, ptr_self, rnd);
#endif
  return ptr_return;
}

static VALUE r_mpfr_to_mpz(int argc, VALUE *argv, VALUE self)
{
  VALUE ptr_return;
  MPFR *ptr_self;
  MP_INT *ptr_mpz;
  mp_rnd_t rnd;
  rnd = r_mpfr_rnd_from_optional_argument(0, 1, argc, argv);
  r_mpfr_get_struct(ptr_self, self);
  mpz_make_struct_init(ptr_return, ptr_mpz);
  mpfr_get_z(ptr_mpz, ptr_self, rnd);
  return ptr_return;
}

static VALUE r_mpfr_get_z_2exp(VALUE self)
{
  VALUE ptr_return;
  MPFR *ptr_self;
  MP_INT *ptr_mpz;
  long int exp;
  r_mpfr_get_struct(ptr_self, self);
  mpz_make_struct_init(ptr_return, ptr_mpz);
  exp = mpfr_get_z_2exp(ptr_mpz, ptr_self);
  return rb_ary_new3(2, ptr_return, INT2NUM(exp));
}

static VALUE r_gmpf_to_fr(int argc, VALUE *argv, VALUE self)
{
  VALUE ptr_return;
  MP_FLOAT *ptr_self;
  MPFR *ptr_mpfr;
  mp_rnd_t rnd;
  mp_prec_t prec;
  mpf_get_struct(self, ptr_self);
  switch(argc) {
  case 2:
    switch(TYPE(argv[0])){
    case T_SYMBOL:
      rnd = r_mpfr_rnd_from_value(argv[0]);
      if (FIXNUM_P(argv[1])) {
	prec = NUM2INT(argv[1]);
      } else {
	rb_raise(rb_eArgError, "Invalid arguments for GMP::F#to_fr; argc=%d\n", argc);
      }
      break;
    case T_FIXNUM:
      prec = NUM2INT(argv[0]);
      if (SYMBOL_P(argv[1])) {
	rnd = r_mpfr_rnd_from_value(argv[1]);
      } else {
	rb_raise(rb_eArgError, "Invalid arguments for GMP::F#to_fr; argc=%d\n", argc);
      }
      break;
    default:
      rb_raise(rb_eArgError, "Invalid arguments for GMP::F#to_fr; argc=%d\n", argc);
    }
    break;
  case 1:
    switch(TYPE(argv[0])){
    case T_SYMBOL:
      prec = R_MPF_GET_PREC(ptr_self);
      rnd = r_mpfr_rnd_from_value(argv[0]);
      break;
    case T_FIXNUM:
      prec = NUM2INT(argv[0]);
      rnd = mpfr_get_default_rounding_mode();
      break;
    default:
      rb_raise(rb_eArgError, "Invalid arguments for GMP::F#to_fr; argc=%d\n", argc);
    }
    break;
  case 0:
    prec = R_MPF_GET_PREC(ptr_self);
    rnd = mpfr_get_default_rounding_mode();
    break;
  default:
    rb_raise(rb_eArgError, "Invalid number of arguments for GMP::F#to_fr; argc=%d\n", argc);
  }
  r_mpfr_make_struct_init2(ptr_return, ptr_mpfr, prec);
  mpfr_set(ptr_mpfr, ptr_self, rnd);
  return ptr_return;
}

static VALUE r_gmpz_to_fr(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  MP_INT *ptr_self;
  VALUE ptr_return;
  MPFR *ptr_mpfr;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  mpz_get_struct(self, ptr_self);
  r_mpfr_make_struct_init2(ptr_return, ptr_mpfr, prec);
  mpfr_set_z(ptr_mpfr, ptr_self, rnd);
  return ptr_return;
}

static VALUE r_gmpq_to_fr(int argc, VALUE *argv, VALUE self)
{
  mp_rnd_t rnd;
  mp_prec_t prec;
  MP_RAT *ptr_self;
  VALUE ptr_return;
  MPFR *ptr_mpfr;
  r_mpfr_get_rnd_prec_from_optional_arguments(&rnd, &prec, 1, 3, argc, argv);
  mpq_get_struct(self, ptr_self);
  r_mpfr_make_struct_init2(ptr_return, ptr_mpfr, prec);
  mpfr_set_q(ptr_mpfr, ptr_self, rnd);
  return ptr_return;
}

/* Return a random number of MPFR object. */
static VALUE r_gmprandstate_mpfr_urandomb2(int argc, VALUE *argv, VALUE self)
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
  rb_define_method(r_mpfr_class, "to_mpf", r_mpfr_to_mpf, -1);
  rb_define_method(r_mpfr_class, "to_mpz", r_mpfr_to_mpz, -1);
  rb_define_method(r_mpfr_class, "get_z_2exp", r_mpfr_get_z_2exp, 0);

  rb_define_method(cGMP_F, "to_fr", r_gmpf_to_fr, -1);
  rb_define_method(cGMP_Z, "to_fr", r_gmpz_to_fr, -1);
  rb_define_method(cGMP_Q, "to_fr", r_gmpq_to_fr, -1);

  rb_define_method(cGMP_RandState, "mpfr_urandomb2", r_gmprandstate_mpfr_urandomb2, -1);
}
