# frozen_string_literal: true

require 'English'

lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

open('ext/numo/narray/numo/narray.h') do |f|
  f.each_line do |l|
    if /NARRAY_VERSION "([\d.]+)"/ =~ l
      NARRAY_VERSION = Regexp.last_match(1)
      break
    end
  end
end

Gem::Specification.new do |spec|
  spec.name          = 'numo-narray'
  spec.version       = NARRAY_VERSION
  spec.authors       = ['Masahiro TANAKA']
  spec.email         = ['masa16.tanaka@gmail.com']
  spec.description   = 'Numo::NArray - New NArray class library in Ruby/Numo.'
  spec.summary       = 'alpha release of Numo::NArray - New NArray class library in Ruby/Numo (NUmerical MOdule)'
  spec.homepage      = 'https://github.com/ruby-numo/numo-narray'
  spec.license       = 'BSD-3-Clause'
  spec.required_ruby_version = '>= 2.2'

  spec.files         = `git ls-files Gemfile LICENSE README.md Rakefile lib ext numo-narray.gemspec spec`.split($INPUT_RECORD_SEPARATOR)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/numo/narray/extconf.rb']
  spec.metadata['rubygems_mfa_required'] = 'true'
end
