# frozen_string_literal: true

require_relative 'test_helper'

class NArrayRactorTest < NArrayTestBase
  if respond_to?(:ractor)
    ractor keep: true
    data(:dtype, TYPES, keep: true)
    def test_non_frozen(data)
      dtype = data.fetch(:dtype)
      ary = random_array(dtype)
      r = Ractor.new(ary) { |x| x }
      ary2 = defined_value? ? r.value : r.take
      assert_equal(ary, ary2)
      assert_not_same(ary, ary2)
    end

    def test_frozen(data)
      dtype = data.fetch(:dtype)
      ary1 = random_array(dtype)
      ary1.freeze
      r = Ractor.new(ary1) do |ary2|
        [ary2, ary2 * 10]
      end
      ary2, res = defined_value? ? r.value : r.take
      assert_equal(dtype != Numo::RObject,
                   ary1.equal?(ary2))
      assert_equal(ary1 * 10, res)
    end

    def test_parallel(data)
      dtype = data.fetch(:dtype)
      ary1 = random_array(dtype, 100_000)
      r1 = Ractor.new(ary1) do |ary2|
        ary2 * 10
      end
      r2 = Ractor.new(ary1) do |ary4|
        ary4 * 10
      end
      if defined_value?
        assert_equal(r1.value, r2.value)
      else
        assert_equal(r1.take, r2.take)
      end
    end

    def random_array(dtype, n = 1000)
      case dtype
      when Numo::DFloat, Numo::SFloat, Numo::DComplex, Numo::SComplex
        dtype.new(n).rand_norm
      else
        dtype.new(n).rand(10)
      end
    end

    def defined_value?
      return @defined_value if defined? @defined_value

      @defined_value = Ractor.method_defined?(:value)
    end
  end
end
