begin
  require 'rspec'
rescue LoadError
  require 'rubygems' unless ENV['NO_RUBYGEMS']
  gem 'rspec'
  require 'rspec'
end

$:.unshift(File.expand_path(File.dirname(__FILE__)) + '/../../lib')
$:.unshift(File.expand_path(File.dirname(__FILE__)) + '/../../ext')
$:.unshift(File.expand_path(File.dirname(__FILE__)))
require 'mpfr/mpfr'
require "generate_number_modulue"
