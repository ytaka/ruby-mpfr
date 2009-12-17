begin
  require 'spec'
rescue LoadError
  require 'rubygems' unless ENV['NO_RUBYGEMS']
  gem 'rspec'
  require 'spec'
end

$:.unshift(File.dirname(__FILE__) + '/../../lib')
$:.unshift(*Dir.glob(File.dirname(__FILE__) + '/../../ext/*'))
$:.unshift(File.dirname(__FILE__))

require 'mpfr'
require 'mpfr/matrix'
require "generate_matrix_arguments"

