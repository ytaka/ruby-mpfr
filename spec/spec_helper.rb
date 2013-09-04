require "rspec"

gem_root = File.expand_path(File.join(File.dirname(__FILE__), ".."))
$:.unshift(File.join(gem_root, "lib"))
Dir.glob(File.join(gem_root, "ext/*")).each do |dir|
  $:.unshift(dir)
end

require "mpfr"
require "mpfr/matrix"
require "mpfr/gmp"
require File.join(gem_root, "spec/mpfr/generate_number_modulue")
require File.join(gem_root, "spec/mpfr_matrix/generate_matrix_arguments")
