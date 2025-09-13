# frozen_string_literal: true

require 'simplecov'
SimpleCov.start

$LOAD_PATH.unshift File.expand_path('../lib', __dir__)
require 'numo/narray'
require 'test/unit'
require 'stringio'

class NArrayTestBase < Test::Unit::TestCase
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
