# frozen_string_literal: true

require_relative 'test_helper'

class NArrayExtraTest < NArrayTestBase
  TYPES.each do |dtype|
    test "#{dtype}#new_narray" do
      a = dtype[[0, 1, 2], [3, 4, 5]]
      na = a.new_narray
      assert_equal(na.ndim, 2)
      assert_equal(na.shape[0], 2)
      assert_equal(na.shape[1], 3)
    end

    test "#{dtype}#new_zeros" do
      a = dtype[[1], [2]]
      na = a.new_zeros
      assert_equal(na.ndim, 2)
      assert_equal(na.shape[0], 2)
      assert_equal(na.shape[1], 1)
      assert_equal(na[0, 0], 0)
      assert_equal(na[1, 0], 0)
    end

    test "#{dtype}#new_ones" do
      a = dtype[[2, 3], [4, 5]]
      na = a.new_ones
      assert_equal(na.ndim, 2)
      assert_equal(na.shape[0], 2)
      assert_equal(na.shape[1], 2)
      assert_equal(na[0, 0], 1)
      assert_equal(na[0, 1], 1)
      assert_equal(na[1, 0], 1)
      assert_equal(na[1, 1], 1)
    end

    test "#{dtype}#new_fill" do
      a = dtype[[1], [2]]
      na = a.new_fill(5)
      assert_equal(na.ndim, 2)
      assert_equal(na.shape[0], 2)
      assert_equal(na.shape[1], 1)
      assert_equal(na[0, 0], 5)
      assert_equal(na[1, 0], 5)
    end

    test "#{dtype}#fliplr" do
      a = dtype[[2, 3], [4, 5]]
      na = a.fliplr
      assert_equal(na[0, 0], 3)
      assert_equal(na[0, 1], 2)
      assert_equal(na[1, 0], 5)
      assert_equal(na[1, 1], 4)
    end
  end

  FLOAT_TYPES.each do |dtype|
    test "#{dtype}#rad2deg" do
      a = dtype[Math::PI / 2, Math::PI / 4]
      na = a.rad2deg
      assert { na[0] != a[0] }
      assert { na[1] != a[1] }
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_in_delta(na[0].real, 90, 1e-6)
        assert_in_delta(na[0].imag, 0, 1e-6)
        assert_in_delta(na[1].real, 45, 1e-6)
        assert_in_delta(na[1].imag, 0, 1e-6)
      else
        assert_in_delta(na[0], 90, 1e-6)
        assert_in_delta(na[1], 45, 1e-6)
      end
    end

    test "#{dtype}#deg2rad" do
      a = dtype[90, 45]
      na = a.deg2rad
      assert { na[0] != a[0] }
      assert { na[1] != a[1] }
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_in_delta(na[0].real, Math::PI / 2, 1e-6)
        assert_in_delta(na[0].imag, 0, 1e-6)
        assert_in_delta(na[1].real, Math::PI / 4, 1e-6)
        assert_in_delta(na[1].imag, 0, 1e-6)
      else
        assert_in_delta(na[0], Math::PI / 2, 1e-6)
        assert_in_delta(na[1], Math::PI / 4, 1e-6)
      end
    end
  end
end
