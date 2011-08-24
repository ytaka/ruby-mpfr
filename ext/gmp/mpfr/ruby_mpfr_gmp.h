#ifndef _RUBY_MPFR_GMP_H_
#define _RUBY_MPFR_GMP_H_

/*
  In ruby_mpfr.h we set 'typedef __mpfr_struct MPFR' and
  in ruby_gmp.h MPFR is a flag meaning the existence of mpfr.h.
  So, we must define the following.
*/
#define MPFR MPFR

#include "gmp_header/ruby_gmp.h"
#include "../../mpfr/ruby_mpfr.h"

#endif /* _RUBY_MPFR_GMP_H_ */
