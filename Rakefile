task :copy_ruby_mpfr_h do |t|
  src = File.expand_path('ruby-mpfr/ext/mpfr/ruby_mpfr.h')
  files = FileList['**/ruby_mpfr.h'].map { |path| File.expand_path(path) }
  files.delete(src)
  files.delete_if { |path| path =~ /\/pkg\// }
  files.each do |path|
    cp(src, path)
  end
end
