# frozen_string_literal: true

require "numo/narray.#{RUBY_PLATFORM.include?('darwin') ? 'bundle' : 'so'}"
require 'numo/narray/extra'
