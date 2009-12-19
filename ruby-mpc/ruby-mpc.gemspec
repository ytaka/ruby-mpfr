# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-mpc}
  s.version = "0.0.3"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Takayuki YAMAGUCHI"]
  s.date = %q{2009-12-19}
  s.description = %q{FIX (describe your package)}
  s.email = ["d@ytak.info"]
  s.extensions = ["ext/mpc/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc"]
  s.files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc", "Rakefile", "ext/mpc/extconf.rb", "ext/mpc/ruby_mpc.c", "ext/mpc/ruby_mpc.h", "ext/mpc/ruby_mpfr.h", "lib/mpc/version.rb", "ruby-mpc.gemspec", "script/console", "script/destroy", "script/generate", "spec/mpc/arithmetic_op_spec.rb", "spec/mpc/const_spec.rb", "spec/mpc/functions_spec.rb", "spec/mpc/generate_complex_number.rb", "spec/mpc/spec_helper.rb", "spec/spec.opts", "tasks/extconf.rake"]
  s.homepage = %q{http://rubyforge.org/projects/ruby-mpfr/}
  s.post_install_message = %q{PostInstall.txt}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib", "ext"]
  s.rubyforge_project = %q{ruby-mpc}
  s.rubygems_version = %q{1.3.5}
  s.summary = %q{FIX (describe your package)}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
      s.add_development_dependency(%q<hoe>, [">= 2.4.0"])
    else
      s.add_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
      s.add_dependency(%q<hoe>, [">= 2.4.0"])
    end
  else
    s.add_dependency(%q<ruby-mpfr>, [">= 0.0.4"])
    s.add_dependency(%q<hoe>, [">= 2.4.0"])
  end
end
