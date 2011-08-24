require "bundler/gem_tasks"

desc "Run 'make realclean' for extended libraries"
task "ext:realclean" do
  Dir.glob("ext/**/Makefile").each do |path|
    sh "cd #{File.dirname(path)}; make realclean"
  end
end

desc "Run 'make clean' for extended libraries"
task "ext:clean" do
  Dir.glob("ext/**/Makefile").each do |path|
    sh "cd #{File.dirname(path)}; make clean"
  end
end

desc "Run 'make realclean' for extended libraries"
task 'ext:make' do
  Dir.glob("ext/**/extconf.rb").each do |path|
    sh "cd #{File.dirname(path)}; ruby extconf.rb && make"
  end
end
