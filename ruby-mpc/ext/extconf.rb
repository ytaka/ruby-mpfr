require 'mkmf'

if have_header('mpfr.h') && have_library('mpfr') && have_header('mpc.h') && have_library('mpc') && have_header('gmp.h') && have_library('gmp')
  create_makefile("ruby_mpc")
end

