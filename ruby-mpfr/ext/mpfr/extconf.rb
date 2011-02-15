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

REQUIRE_VERSION = 3

dir_config("mpfr")
dir_config("gmp")
if have_header('mpfr.h') && have_library('mpfr') && have_header('gmp.h') && have_library('gmp')
  if have_macro('MPFR_VERSION_MAJOR', 'mpfr.h') do |src|
      src + <<SRC
#if MPFR_VERSION_MAJOR < #{REQUIRE_VERSION}
# error
>>>>>> MPFR_VERSION_MAJOR requires larger than #{REQUIRE_VERSION} <<<<<<
#endif
SRC
    end
    create_makefile("mpfr")
  end
end
