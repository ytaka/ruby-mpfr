# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-mpfr}
  s.version = "0.0.5"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Takayuki YAMAGUCHI"]
  s.date = %q{2009-12-19}
  s.description = %q{ruby-mpfr is library to use MPFR[http://www.mpfr.org/] which is a C library for
multiple-precision floating-point computations.
ruby-mpfr also includes Matrix class of MPFR; MPFR::Matrix, MPFR::SquareMatrix,
MPFR::ColumnVector, and MPFR::RowVector.}
  s.email = ["d@ytak.info"]
  s.extensions = ["ext/mpfr/extconf.rb", "ext/mpfr_matrix/mpfr/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc"]
  s.files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc", "Rakefile", "ext/mpfr/extconf.rb", "ext/mpfr/ruby_mpfr.c", "ext/mpfr/ruby_mpfr.h", "ext/mpfr_matrix/mpfr/extconf.rb", "ext/mpfr_matrix/mpfr/func_mpfr_matrix.c", "ext/mpfr_matrix/mpfr/func_mpfr_matrix.h", "ext/mpfr_matrix/mpfr/ruby_mpfr.h", "ext/mpfr_matrix/mpfr/ruby_mpfr_matrix.c", "ext/mpfr_matrix/mpfr/ruby_mpfr_matrix.h", "lib/mpfr/matrix.rb", "lib/mpfr/version.rb", "ruby-mpfr.gemspec", "script/console", "script/destroy", "script/generate", "spec/mpfr/allocate_spec.rb", "spec/mpfr/arithmetic_spec.rb", "spec/mpfr/comparison_spec.rb", "spec/mpfr/constant_spec.rb", "spec/mpfr/conversion_spec.rb", "spec/mpfr/exception_spec.rb", "spec/mpfr/functions_spec.rb", "spec/mpfr/generate_number_modulue.rb", "spec/mpfr/precision_roundmode_spec.rb", "spec/mpfr/rounding_spec.rb", "spec/mpfr/set_value_spec.rb", "spec/mpfr/spec_helper.rb", "spec/mpfr/string_spec.rb", "spec/mpfr_matrix/generate_matrix_arguments.rb", "spec/mpfr_matrix/mpfr_matrix_alloc_spec.rb", "spec/mpfr_matrix/mpfr_matrix_arithmetic_spec.rb", "spec/mpfr_matrix/mpfr_matrix_set_element_spec.rb", "spec/mpfr_matrix/mpfr_matrix_string_spec.rb", "spec/mpfr_matrix/mpfr_square_matrix_spec.rb", "spec/mpfr_matrix/spec_helper.rb", "spec/spec.opts", "tasks/extconf.rake"]
  s.homepage = %q{[URL] http://rubyforge.org/projects/ruby-mpfr/}
  s.post_install_message = %q{PostInstall.txt}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{ruby-mpfr}
  s.rubygems_version = %q{1.3.5}
  s.summary = %q{ruby-mpfr is library to use MPFR[http://www.mpfr.org/] which is a C library for multiple-precision floating-point computations}

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
