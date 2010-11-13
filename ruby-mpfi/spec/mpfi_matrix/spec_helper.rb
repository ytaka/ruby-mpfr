begin
  require 'rspec'
rescue LoadError
  require 'rubygems' unless ENV['NO_RUBYGEMS']
  gem 'rspec'
  require 'rspec'
end

$:.unshift(File.dirname(__FILE__) + '/../../lib')
$:.unshift(*Dir.glob(File.dirname(__FILE__) + '/../../ext/*'))
$:.unshift(File.dirname(__FILE__))

require 'mpfr'
require 'mpfr/matrix'
require 'mpfi'
require 'mpfi/complex'
require 'mpfi/matrix'
require "generate_matrix_arguments"

