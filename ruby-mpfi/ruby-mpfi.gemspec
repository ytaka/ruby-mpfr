# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-mpfi}
  s.version = "0.0.4"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Takayuki YAMAGUCHI"]
  s.date = %q{2011-02-15}
  s.description = %q{ruby-mpfi is an extended C library to use MPFI[http://gforge.inria.fr/projects/mpfi/]
which is the library of the interval arithmetic with multiprecision.}
  s.email = ["d@ytak.info"]
  s.extensions = ["ext/mpfi/extconf.rb", "ext/mpfi_complex/mpfi/extconf.rb", "ext/mpfi_matrix/mpfi/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc"]
  s.files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc", "Rakefile", "ext/mpfi/extconf.rb", "ext/mpfi/func_mpfi_extention.c", "ext/mpfi/func_mpfi_extention.h", "ext/mpfi/ruby_mpfi.c", "ext/mpfi/ruby_mpfi.h", "ext/mpfi/ruby_mpfr.h", "ext/mpfi_complex/mpfi/extconf.rb", "ext/mpfi_complex/mpfi/func_mpfi_extention.h", "ext/mpfi_complex/mpfi/func_ruby_mpfi_complex.c", "ext/mpfi_complex/mpfi/func_ruby_mpfi_complex.h", "ext/mpfi_complex/mpfi/ruby_mpfi.h", "ext/mpfi_complex/mpfi/ruby_mpfi_complex.c", "ext/mpfi_complex/mpfi/ruby_mpfi_complex.h", "ext/mpfi_complex/mpfi/ruby_mpfr.h", "ext/mpfi_matrix/mpfi/extconf.rb", "ext/mpfi_matrix/mpfi/func_mpfi_extention.h", "ext/mpfi_matrix/mpfi/func_mpfi_matrix.c", "ext/mpfi_matrix/mpfi/func_mpfi_matrix.h", "ext/mpfi_matrix/mpfi/func_mpfr_matrix.h", "ext/mpfi_matrix/mpfi/ruby_mpfi.h", "ext/mpfi_matrix/mpfi/ruby_mpfi_matrix.c", "ext/mpfi_matrix/mpfi/ruby_mpfi_matrix.h", "ext/mpfi_matrix/mpfi/ruby_mpfr.h", "ext/mpfi_matrix/mpfi/ruby_mpfr_matrix.h", "lib/mpfi/matrix.rb", "lib/mpfi/version.rb", "ruby-mpfi.gemspec", "spec/mpfi/generate_number_module.rb", "spec/mpfi/mpfi_alloc_spec.rb", "spec/mpfi/mpfi_diam_arithmetic_spec.rb", "spec/mpfi/mpfi_interval_arithmetic_spec.rb", "spec/mpfi/mpfi_interval_functions_spec.rb", "spec/mpfi/mpfi_math_functions_spec.rb", "spec/mpfi/mpfi_set_operation_spec.rb", "spec/mpfi/ruby-mpfi_spec.rb", "spec/mpfi/spec_helper.rb", "spec/mpfi/string_spec.rb", "spec/mpfi_complex/spec_helper.rb", "spec/mpfi_matrix/generate_matrix_arguments.rb", "spec/mpfi_matrix/mpfi_matrix_alloc_spec.rb", "spec/mpfi_matrix/mpfi_matrix_arithmetic_spec.rb", "spec/mpfi_matrix/mpfi_matrix_interval_func_spec.rb", "spec/mpfi_matrix/mpfi_matrix_set_element_spec.rb", "spec/mpfi_matrix/mpfi_matrix_set_operation_spec.rb", "spec/mpfi_matrix/mpfi_matrix_string_spec.rb", "spec/mpfi_matrix/mpfi_matrix_subdivision_spec.rb", "spec/mpfi_matrix/mpfi_square_matrix_spec.rb", "spec/mpfi_matrix/mpfi_vector_spec.rb", "spec/mpfi_matrix/spec_helper.rb", "tasks/extconf.rake", ".gemtest"]
  s.homepage = %q{http://rubyforge.org/projects/ruby-mpfr/}
  s.post_install_message = %q{PostInstall.txt}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib"]
  s.rubyforge_project = %q{ruby-mpfi}
  s.rubygems_version = %q{1.5.2}
  s.summary = %q{ruby-mpfi is an extended C library to use MPFI[http://gforge.inria.fr/projects/mpfi/] which is the library of the interval arithmetic with multiprecision.}

  if s.respond_to? :specification_version then
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
      s.add_development_dependency(%q<hoe>, [">= 2.9.1"])
    else
      s.add_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
      s.add_dependency(%q<hoe>, [">= 2.9.1"])
    end
  else
    s.add_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
    s.add_dependency(%q<hoe>, [">= 2.9.1"])
  end
end
