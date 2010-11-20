begin
  require 'gmp'
rescue
  STDERR.puts "Can not load 'gmp'. Please execute 'gem install gmp'."
  raise
end

require 'mpfr/mpfr_gmp'
