require 'mkmf'

$CFLAGS += " -Wall"

dir_config("mpfr")
dir_config("mpc")
dir_config("gmp")
if have_header('mpfr.h') && have_library('mpfr') && have_header('mpc.h') && have_library('mpc') && have_header('gmp.h') && have_library('gmp')
  create_makefile("mpc")
end

