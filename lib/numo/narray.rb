# frozen_string_literal: true

require_relative "narray.#{RUBY_PLATFORM.include?('darwin') ? 'bundle' : 'so'}"
require_relative 'narray/extra'

module Numo
  class NArray
    # Constant to indicate whether it is numo-narray-alt.
    ALTERNATIVE = true

    # Returns true if this is numo-narray-alt.
    # @return [Boolean]
    def self.alternative?
      ALTERNATIVE
    end
  end
end
