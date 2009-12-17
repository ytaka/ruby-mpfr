begin
  require 'spec'
rescue LoadError
  require 'rubygems' unless ENV['NO_RUBYGEMS']
  require 'spec'
end
begin
  require 'spec/rake/spectask'
rescue LoadError
  puts <<-EOS
To use rspec for testing you must install rspec gem:
    gem install rspec
EOS
  exit(0)
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
