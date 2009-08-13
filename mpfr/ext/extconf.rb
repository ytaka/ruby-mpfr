require 'mkmf'

if have_header('mpfr.h') && have_library('mpfr') && have_header('gmp.h') && have_library('gmp')
  create_makefile("mpfr")
end
