# frozen_string_literal: true

require 'bundler/gem_tasks'

task :doc do
  dir = 'ext/numo/narray'
  src = %w[array.c data.c index.c math.c narray.c rand.c struct.c]
        .map { |s| File.join(dir, s) } +
        [File.join(dir, 't_*.c'), 'lib/numo/narray/extra.rb']
  sh 'cd ext/numo/narray; ruby extconf.rb; make src'
  sh "rm -rf yard .yardoc; yard doc -o yard -m markdown -r README.md #{src.join(' ')}"
end

require 'ruby_memcheck' if ENV['BUNDLE_WITH'] == 'memcheck'
require 'rake/testtask'

if ENV['BUNDLE_WITH'] == 'memcheck'
  test_config = lambda do |t|
    t.libs << 'test'
    t.libs << 'lib'
    t.test_files = FileList['test/**/test_*.rb']
  end
  Rake::TestTask.new(test: :compile, &test_config)
  namespace :test do
    RubyMemcheck::TestTask.new(valgrind: :compile, &test_config)
  end
else
  Rake::TestTask.new(:test) do |t|
    t.libs << 'test'
    t.libs << 'lib'
    t.test_files = FileList['test/**/test_*.rb']
  end
end

require 'rake/extensiontask'
Rake::ExtensionTask.new('numo/narray')
