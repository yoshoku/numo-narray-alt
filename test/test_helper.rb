# frozen_string_literal: true

require 'simplecov'
SimpleCov.start

require_relative '../lib/numo/narray'

require 'minitest/autorun'

class NArrayTestBase < Minitest::Test
  FLOAT_TYPES = [
    Numo::SFloat,
    Numo::SComplex,
    Numo::DFloat,
    Numo::DComplex
  ].freeze

  TYPES = [
    *FLOAT_TYPES,
    Numo::Int64,
    Numo::Int32,
    Numo::Int16,
    Numo::Int8,
    Numo::UInt64,
    Numo::UInt32,
    Numo::UInt16,
    Numo::UInt8,
    Numo::RObject
  ].freeze
end
