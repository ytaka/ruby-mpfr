# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-mpfr}
  s.version = "0.0.3"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Takayuki YAMAGUCHI"]
  s.date = %q{2009-12-13}
  s.description = %q{ruby-mpfr is library to use MPFR[http://www.mpfr.org/] which is a C library for
multiple-precision floating-point computations.}
  s.email = ["d@ytak.info"]
  s.extensions = ["ext/mpfr/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc"]
  s.files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc", "Rakefile", "ext/mpfr/extconf.rb", "ext/mpfr/ruby_mpfr.c", "ext/mpfr/ruby_mpfr.h", "ext/mpfr_matrix/extconf.rb", "ext/mpfr_matrix/func_mpfr_matrix.c", "ext/mpfr_matrix/func_mpfr_matrix.h", "ext/mpfr_matrix/func_vector_2d.c", "ext/mpfr_matrix/func_vector_2d.h", "ext/mpfr_matrix/matrix_alloc.c", "ext/mpfr_matrix/matrix_arithmetic.c", "ext/mpfr_matrix/matrix_conversion.c", "ext/mpfr_matrix/matrix_element.c", "ext/mpfr_matrix/matrix_function.c", "ext/mpfr_matrix/matrix_string.c", "ext/mpfr_matrix/ruby_mpfr_matrix.c", "ext/mpfr_matrix/ruby_mpfr_matrix.h", "ext/mpfr_matrix/square_matrix_function.c", "ext/mpfr_matrix/vector_2d_function.c", "ext/mpfr_matrix/vector_function.c", "ext/mpfr_matrix/yasnippet_mpfr_matrix.el", "lib/ruby-mpfr/version.rb", "ruby-mpfr.gemspec", "script/console", "script/destroy", "script/generate", "spec/allocate_spec.rb", "spec/arithmetic_spec.rb", "spec/comparison_spec.rb", "spec/constant_spec.rb", "spec/conversion_spec.rb", "spec/exception_spec.rb", "spec/functions_spec.rb", "spec/generate_number_modulue.rb", "spec/precision_roundmode_spec.rb", "spec/rounding_spec.rb", "spec/set_value_spec.rb", "spec/spec.opts", "spec/spec_helper.rb", "spec/string_spec.rb", "tasks/rspec.rake"]
  s.homepage = %q{http://rubyforge.org/projects/ruby-mpfr/}
  s.post_install_message = %q{PostInstall.txt}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{ruby-mpfr}
  s.rubygems_version = %q{1.3.5}
  s.summary = %q{ruby-mpfr is library to use MPFR[http://www.mpfr.org/] which is a C library for multiple-precision floating-point computations.}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<hoe>, [">= 2.4.0"])
    else
      s.add_dependency(%q<hoe>, [">= 2.4.0"])
    end
  else
    s.add_dependency(%q<hoe>, [">= 2.4.0"])
  end
end
