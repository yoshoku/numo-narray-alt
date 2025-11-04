# frozen_string_literal: true

require_relative 'test_helper'

def complex_type?(type)
  [Numo::DComplex, Numo::SComplex].include?(type)
end

class NArrayMathTest < NArrayTestBase
  def test_erfc
    FLOAT_TYPES.reject { |t| complex_type?(t) }.each do |dtype|
      a = dtype[-2, -1, 0, 1, 2]
      b = Numo::NMath.erfc(a)
      err = (dtype[Math.erfc(-2), Math.erfc(-1), 1, Math.erfc(1), Math.erfc(2)] - b).abs.max

      assert_kind_of(dtype, b)
      assert_operator(err, :<, 1e-6)
    end
  end

  def test_log1p
    FLOAT_TYPES.reject { |t| complex_type?(t) }.each do |dtype|
      a = dtype[-0.5, 0, 1, 2]
      b = Numo::NMath.log1p(a)
      err = (dtype[Math.log(0.5), Math.log(1), Math.log(2), Math.log(3)] - b).abs.max

      assert_kind_of(dtype, b)
      assert_operator(err, :<, 1e-6)
    end
  end

  def test_expm1
    FLOAT_TYPES.reject { |t| complex_type?(t) }.each do |dtype|
      a = dtype[-1.0, 0.0, 1.0, 2.0]
      b = Numo::NMath.expm1(a)
      err = (dtype[Math.exp(-1) - 1, 0, Math.exp(1) - 1, Math.exp(2) - 1] - b).abs.max

      assert_kind_of(dtype, b)
      assert_operator(err, :<, 1e-6)
    end
  end

  def test_ldexp
    FLOAT_TYPES.reject { |t| complex_type?(t) }.each do |dtype|
      a = dtype[0.0, 0.5, 0.5, 0.75, 0.5, 0.625]
      b = Numo::Int32[0, 1, 2, 2, 3, 3]
      c = Numo::NMath.ldexp(a, b)

      assert_kind_of(dtype, c)
      assert_equal(dtype[0.0, 1.0, 2.0, 3.0, 4.0, 5.0], c)
    end
  end

  def test_frexp
    FLOAT_TYPES.reject { |t| complex_type?(t) }.each do |dtype|
      a = dtype[0.0, 1.0, 2.0, 3.0, 4.0, 5.0]
      mant, expo = Numo::NMath.frexp(a)

      assert_kind_of(dtype, mant)
      assert_kind_of(Numo::Int32, expo)
      assert_equal(dtype[0.0, 0.5, 0.5, 0.75, 0.5, 0.625], mant)
      assert_equal(Numo::Int32[0, 1, 2, 2, 3, 3], expo)
    end
  end
end
