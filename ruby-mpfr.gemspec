# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "mpfr/version"

Gem::Specification.new do |s|
  s.name        = "ruby-mpfr"
  s.version     = MPFR::VERSION
  s.authors     = ["Takayuki YAMAGUCHI"]
  s.email       = ["d@ytak.info"]
  s.homepage    = ""
  s.summary     = "Ruby bindings of MPFR"
  s.description = "Ruby bindings of MPFR that is a C library for multiple-precision floating-point computations"

  s.rubyforge_project = "ruby-mpfr"

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib", "ext", "ext/gmp", "ext/mpfr_matrix"]
  s.extensions    = Dir.glob('ext/**/extconf.rb')

  # specify any dependencies here; for example:
  s.add_development_dependency "rspec"
  s.add_development_dependency "yard"
  s.add_development_dependency "extconf-task"
  # s.add_runtime_dependency "rest-client"
end
