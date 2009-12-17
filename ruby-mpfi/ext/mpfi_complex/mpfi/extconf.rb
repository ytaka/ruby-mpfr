require 'mkmf'

$CFLAGS += " -Wall"

dir_config("mpfr")
dir_config("mpfi")
dir_config("gmp")
if have_header('mpfr.h') && have_library('mpfr') && have_header('gmp.h') && have_library('gmp')
  create_makefile("mpfi/complex")
end
