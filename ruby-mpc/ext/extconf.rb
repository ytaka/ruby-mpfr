require 'mkmf'

lib_dir = File.expand_path("~/Work/Library")
dir_config('mpfr', lib_dir)
dir_config('mpc', lib_dir)
dir_config('gmp', lib_dir)

$CFLAGS += " -Wall"
$LDFLAGS += " -Wl,-R#{lib_dir}/lib"

if have_header('mpfr.h') && have_library('mpfr') && have_header('mpc.h') && have_library('mpc') && have_header('gmp.h') && have_library('gmp')
  create_makefile("ruby_mpc")
end

