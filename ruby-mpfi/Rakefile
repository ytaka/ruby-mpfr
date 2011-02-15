require 'rubygems'
gem 'hoe', '>= 2.1.0'
require 'hoe'
require 'fileutils'
require './lib/mpfi/version.rb'

Hoe.plugin :newgem
# Hoe.plugin :website
# Hoe.plugin :cucumberfeatures

# Generate all the Rake tasks
# Run 'rake -T' to see list of generated tasks (from gem root directory)
$hoe = Hoe.spec 'ruby-mpfi' do
  self.developer 'Takayuki YAMAGUCHI', 'd@ytak.info'
  self.post_install_message = 'PostInstall.txt' # TODO remove if post-install message not required
  self.rubyforge_name       = self.name # TODO this is default value
  self.extra_deps         = [['ruby-mpfr','>= 0.0.4']]
  self.spec_extras[:extensions] = ["ext/mpfi/extconf.rb", "ext/mpfi_complex/mpfi/extconf.rb", "ext/mpfi_matrix/mpfi/extconf.rb"]
  self.extra_rdoc_files << 'README.rdoc'
end

desc "Run 'make realclean' for extended libraries"
task "ext:realclean" do
  Dir.glob("ext/**/Makefile").each do |path|
    system("cd #{File.dirname(path)}; make realclean")
  end
end

desc "Run 'make clean' for extended libraries"
task "ext:clean" do
  Dir.glob("ext/**/Makefile").each do |path|
    system("cd #{File.dirname(path)}; make clean")
  end
end

desc "Run 'make realclean' for extended libraries"
task 'ext:make' do
  Dir.glob("ext/**/extconf.rb").each do |path|
    system("cd #{File.dirname(path)}; ruby extconf.rb && make")
  end
end

# TODO - want other tests/tasks run by default? Add them to the list
# remove_task :default
# task :default => [:spec, :features]
