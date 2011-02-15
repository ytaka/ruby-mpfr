require 'mkmf'

i = 0
while i < ARGV.size
  case ARGV[i]
  when '--ldflags'
    if args = ARGV[i+1]
      i += 1
      $LDFLAGS += " #{args}"
    end
  else
    raise "Invalid option: #{ARGV[i]}"
  end
  i += 1
end

$CFLAGS += " -Wall"

dir_config("mpfr")
dir_config("mpc")
dir_config("gmp")
if have_header('mpfr.h') && have_library('mpfr') && have_header('mpc.h') && have_library('mpc') && have_header('gmp.h') && have_library('gmp')
  create_makefile("mpc")
end

