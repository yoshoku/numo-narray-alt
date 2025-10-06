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
  spec.name          = 'numo-narray-alt'
  spec.version       = NARRAY_VERSION
  spec.authors       = ['yoshoku']
  spec.email         = ['yoshoku@outlook.com']
  spec.description   = 'Numo::NArray Alternative is an experimental project forked from Numo::NArray.'
  spec.summary       = 'Numo::NArray Alternative is an experimental project forked from Numo::NArray.'
  spec.homepage      = 'https://github.com/yoshoku/numo-narray-alt'
  spec.license       = 'BSD-3-Clause'
  spec.required_ruby_version = '>= 2.2'

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = spec.homepage
  spec.metadata['changelog_uri'] = "#{spec.homepage}/blob/main/CHANGELOG.md"
  spec.metadata['documentation_uri'] = "https://gemdocs.org/gems/#{spec.name}/#{spec.version}/"

  spec.files         = `git ls-files Gemfile LICENSE README.md Rakefile lib ext numo-narray-alt.gemspec`.split($INPUT_RECORD_SEPARATOR) # rubocop:disable Layout/LineLength
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions    = ['ext/numo/narray/extconf.rb']
  spec.metadata['rubygems_mfa_required'] = 'true'
end
