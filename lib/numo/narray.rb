# frozen_string_literal: true

# On distributions like Rocky Linux, native extensions are installed in a separate
# directory from Ruby code, so use require to load them.
require 'numo/narray/narray'

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
