# frozen_string_literal: true

require_relative 'test_helper'

class NArrayExtraTest < NArrayTestBase
  TYPES.each do |dtype|
    test "#{dtype}#new_narray" do
      a = dtype[[0, 1, 2], [3, 4, 5]]
      na = a.new_narray
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(3, na.shape[1])
    end

    test "#{dtype}#new_zeros" do
      a = dtype[[1], [2]]
      na = a.new_zeros
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(1, na.shape[1])
      assert_equal(0, na[0, 0])
      assert_equal(0, na[1, 0])
    end

    test "#{dtype}#new_ones" do
      a = dtype[[2, 3], [4, 5]]
      na = a.new_ones
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(2, na.shape[1])
      assert_equal(1, na[0, 0])
      assert_equal(1, na[0, 1])
      assert_equal(1, na[1, 0])
      assert_equal(1, na[1, 1])
    end

    test "#{dtype}#new_fill" do
      a = dtype[[1], [2]]
      na = a.new_fill(5)
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(1, na.shape[1])
      assert_equal(5, na[0, 0])
      assert_equal(5, na[1, 0])
    end

    test "#{dtype}#fliplr" do
      a = dtype[[2, 3], [4, 5]]
      na = a.fliplr
      assert_equal(3, na[0, 0])
      assert_equal(2, na[0, 1])
      assert_equal(5, na[1, 0])
      assert_equal(4, na[1, 1])
    end

    test "#{dtype}#flipud" do
      a = dtype[[2, 3], [4, 5]]
      na = a.flipud
      assert_equal(4, na[0, 0])
      assert_equal(5, na[0, 1])
      assert_equal(2, na[1, 0])
      assert_equal(3, na[1, 1])
    end

    test "#{dtype}#rot90" do
      a = dtype[[0, 1], [2, 3]]
      na = a.rot90(0)
      assert_equal(a[0, 0], na[0, 0])
      assert_equal(a[0, 1], na[0, 1])
      assert_equal(a[1, 0], na[1, 0])
      assert_equal(a[1, 1], na[1, 1])
      na = a.rot90
      assert_equal(a[0, 1], na[0, 0])
      assert_equal(a[1, 1], na[0, 1])
      assert_equal(a[0, 0], na[1, 0])
      assert_equal(a[1, 0], na[1, 1])
      na = a.rot90(2)
      assert_equal(a[1, 1], na[0, 0])
      assert_equal(a[1, 0], na[0, 1])
      assert_equal(a[0, 1], na[1, 0])
      assert_equal(a[0, 0], na[1, 1])
      na = a.rot90(3)
      assert_equal(a[1, 0], na[0, 0])
      assert_equal(a[0, 0], na[0, 1])
      assert_equal(a[1, 1], na[1, 0])
      assert_equal(a[0, 1], na[1, 1])
    end

    test "#{dtype}#to_i" do
      a = dtype[1, 2, 3]
      assert_raise(TypeError) { a.to_i }
      a = dtype[5]
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_raise(RangeError) { a.to_i }
      else
        ai = a.to_i
        assert_equal(ai, 5)
        assert { ai.is_a?(Integer) }
      end
    end

    test "#{dtype}#to_f" do
      a = dtype[1, 2, 3]
      assert_raise(TypeError) { a.to_f }
      a = dtype[5]
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_raise(RangeError) { a.to_f }
      else
        af = a.to_f
        assert_equal(5, af)
        assert { af.is_a?(Float) }
      end
    end

    test "#{dtype}#to_c" do
      a = dtype[1, 2, 3]
      assert_raise(TypeError) { a.to_c }
      a = dtype[5]
      ac = a.to_c
      assert_equal(5 + 0i, ac)
      assert { ac.is_a?(Complex) }
    end

    test "#{dtype}#each_over_axis" do
      a = dtype[[0, 1], [2, 3]]
      enm = a.each_over_axis
      assert_equal(Enumerator, enm.class)
      assert_equal(2, enm.to_a.size)
      assert_equal(dtype[0, 1], enm.next)
      assert_equal(dtype[2, 3], enm.next)
      enm = a.each_over_axis(1)
      assert_equal(Enumerator, enm.class)
      assert_equal(2, enm.to_a.size)
      assert_equal(dtype[0, 2], enm.next)
      assert_equal(dtype[1, 3], enm.next)
      a = dtype.new
      assert_raise(RuntimeError) { a.each_over_axis { |_v| nil } }
      assert_raise(ArgumentError) { a.each_over_axis(1) { |_v| nil } }
    end

    test "#{dtype}#append" do
      a = dtype[1, 2]
      na = a.append([[3, 4], [5, 6]])
      assert_equal(1, na.ndim)
      assert_equal(6, na.size)
      a = dtype[[1, 2]]
      na = a.append([[3, 4], [5, 6]], axis: 0)
      assert_equal(2, na.ndim)
      assert_equal(3, na.shape[0])
      assert_equal(2, na.shape[1])
      assert_raise(Numo::NArray::DimensionError) { a.append([3, 4], axis: 0) }
      a = dtype[[1], [2]]
      na = a.append([[3, 4], [5, 6]], axis: 1)
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(3, na.shape[1])
    end

    test "#{dtype}#delete" do
      a = dtype[[1, 2], [3, 4], [5, 6]]
      na = a.delete(1)
      assert_equal(1, na.ndim)
      assert_equal(5, na.size)
      na = a.delete(1, 0)
      assert_equal(2, na.ndim)
      assert_equal(2, na.shape[0])
      assert_equal(2, na.shape[1])
      na = a.delete(1, 1)
      assert_equal(2, na.ndim)
      assert_equal(3, na.shape[0])
      assert_equal(1, na.shape[1])
      na = a.delete([0, 2], 0)
      assert_equal(2, na.ndim)
      assert_equal(1, na.shape[0])
      assert_equal(2, na.shape[1])
    end
  end

  FLOAT_TYPES.each do |dtype|
    test "#{dtype}#rad2deg" do
      a = dtype[Math::PI / 2, Math::PI / 4]
      na = a.rad2deg
      assert { na[0] != a[0] }
      assert { na[1] != a[1] }
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_in_delta(90, na[0].real, 1e-6)
        assert_in_delta(0, na[0].imag, 1e-6)
        assert_in_delta(45, na[1].real, 1e-6)
        assert_in_delta(0, na[1].imag, 1e-6)
      else
        assert_in_delta(90, na[0], 1e-6)
        assert_in_delta(45, na[1], 1e-6)
      end
    end

    test "#{dtype}#deg2rad" do
      a = dtype[90, 45]
      na = a.deg2rad
      assert { na[0] != a[0] }
      assert { na[1] != a[1] }
      if [Numo::DComplex, Numo::SComplex].include?(dtype)
        assert_in_delta(Math::PI / 2, na[0].real, 1e-6)
        assert_in_delta(0, na[0].imag, 1e-6)
        assert_in_delta(Math::PI / 4, na[1].real, 1e-6)
        assert_in_delta(0, na[1].imag, 1e-6)
      else
        assert_in_delta(Math::PI / 2, na[0], 1e-6)
        assert_in_delta(Math::PI / 4, na[1], 1e-6)
      end
    end
  end
end
