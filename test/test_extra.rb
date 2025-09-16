# frozen_string_literal: true

require_relative 'test_helper'

class NArrayExtraTest < NArrayTestBase
  sub_test_case 'instance methods' do
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

      test "#{dtype}#insert" do
        a = dtype[[1, 2], [3, 4], [5, 6]]
        na = a.insert(1, 9)
        assert_equal(1, na.ndim)
        assert_equal(7, na.size)
        assert_equal(9, na[1])
        na = a.insert(1, 9, axis: 1)
        assert_equal(2, na.ndim)
        assert_equal(3, na.shape[0])
        assert_equal(3, na.shape[1])
        assert_equal(dtype[9, 9, 9], na[true, 1])
        na = a.insert(1, [7, 8], axis: 0)
        assert_equal(2, na.ndim)
        assert_equal(4, na.shape[0])
        assert_equal(2, na.shape[1])
        assert_equal(dtype[7, 8], na[1, true])
        na = a.insert([0, 2], [8, 9], axis: 1)
        assert_equal(2, na.ndim)
        assert_equal(3, na.shape[0])
        assert_equal(4, na.shape[1])
        assert_equal(dtype[8, 8, 8], na[true, 0])
        assert_equal(dtype[9, 9, 9], na[true, 3])
      end

      test "#{dtype}#concatenate" do
        a = dtype[[1, 2], [3, 4]]
        b = dtype[[5, 6]]
        na = a.concatenate(b, axis: 0)
        assert_equal(2, na.ndim)
        assert_equal(3, na.shape[0])
        assert_equal(2, na.shape[1])
        assert_equal(na[2, true], b[0, true])
        na = a.concatenate(b.transpose, axis: 1)
        assert_equal(2, na.ndim)
        assert_equal(2, na.shape[0])
        assert_equal(3, na.shape[1])
        assert_equal(na[true, 2], b[0, true])
        a = dtype[1, 2]
        na = a.concatenate([3, 4])
        assert_equal(1, na.ndim)
        assert_equal(4, na.size)
        assert_equal(3, na[2])
        assert_equal(4, na[3])
        na = a.concatenate(3)
        assert_equal(1, na.ndim)
        assert_equal(3, na.size)
        assert_equal(3, na[2])
        assert_raise(TypeError) { a.concatenate('3') }
      end

      test "#{dtype}#split" do
        a = dtype.new(6).seq
        na = a.split(2)
        assert_equal(2, na.size)
        assert_equal(dtype[0, 1, 2], na[0])
        assert_equal(dtype[3, 4, 5], na[1])
        na = a.split([2, 5])
        assert_equal(3, na.size)
        assert_equal(dtype[0, 1], na[0])
        assert_equal(dtype[2, 3, 4], na[1])
        assert_equal(dtype[5], na[2])
        na = a.split(Numo::UInt8[2, 5])
        assert_equal(3, na.size)
        assert_equal(dtype[0, 1], na[0])
        assert_equal(dtype[2, 3, 4], na[1])
        assert_equal(dtype[5], na[2])
        a = dtype.new(2, 3).seq
        na = a.split(2, axis: 1)
        assert_equal(2, na.size)
        assert_equal(dtype[[0, 1], [3, 4]], na[0])
        assert_equal(dtype[[2], [5]], na[1])
        assert_raise(TypeError) { a.split('2') }
      end

      test "#{dtype}#vsplit" do
        a = dtype.new(3, 2).seq
        na = a.vsplit(2)
        assert_equal(2, na.size)
        assert_equal(dtype[[0, 1], [2, 3]], na[0])
        assert_equal(dtype[[4, 5]], na[1])
      end

      test "#{dtype}#hsplit" do
        a = dtype.new(3, 2).seq
        na = a.hsplit(2)
        assert_equal(2, na.size)
        assert_equal(dtype[[0], [2], [4]], na[0])
        assert_equal(dtype[[1], [3], [5]], na[1])
      end

      test "#{dtype}#dsplit" do
        a = dtype.new(3, 2, 2).seq
        na = a.dsplit(2)
        assert_equal(2, na.size)
        assert_equal(dtype[[[0], [2]], [[4], [6]], [[8], [10]]], na[0])
        assert_equal(dtype[[[1], [3]], [[5], [7]], [[9], [11]]], na[1])
      end

      test "#{dtype}#tile" do
        a = dtype.new(3).seq
        na = a.tile(2)
        assert_equal(dtype[0, 1, 2, 0, 1, 2], na)
        na = a.tile(2, 2)
        assert_equal(dtype[[0, 1, 2, 0, 1, 2], [0, 1, 2, 0, 1, 2]], na)
        na = a.tile(2, 1, 2)
        assert_equal(dtype[[[0, 1, 2, 0, 1, 2]], [[0, 1, 2, 0, 1, 2]]], na)
        a = dtype.new(2, 2).seq
        na = a.tile(2)
        assert_equal(dtype[[0, 1, 0, 1], [2, 3, 2, 3]], na)
        na = a.tile(2, 2)
        assert_equal(dtype[[0, 1, 0, 1], [2, 3, 2, 3], [0, 1, 0, 1], [2, 3, 2, 3]], na)
      end

      test "#{dtype}#repeat" do
        a = dtype.new(2, 3).seq
        na = a.repeat(2)
        assert_equal(dtype[0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5], na)
        na = a.repeat(2, axis: 0)
        assert_equal(dtype[[0, 1, 2], [0, 1, 2], [3, 4, 5], [3, 4, 5]], na)
        na = a.repeat(2, axis: 1)
        assert_equal(dtype[[0, 0, 1, 1, 2, 2], [3, 3, 4, 4, 5, 5]], na)
        assert_raise(ArgumentError) { a.repeat(2, axis: '0') }
        na = a.repeat([1, 2], axis: 0)
        assert_equal(dtype[[0, 1, 2], [3, 4, 5], [3, 4, 5]], na)
        assert_raise(ArgumentError) { a.repeat([1, 2], axis: 1) }
      end

      test "#{dtype}#diff" do
        a = dtype[1, 3, 5, 7, 9]
        na = a.diff
        assert_equal(dtype[2, 2, 2, 2], na)
        a = dtype[1, 3, 5, 9]
        na = a.diff(2)
        assert_equal(dtype[0, 2], na)
        a = dtype[[0, 3, 6, 8], [1, 5, 6, 10]]
        na = a.diff(axis: 0)
        assert_equal(dtype[[1, 2, 0, 2]], na)
        na = a.diff(axis: 1)
        assert_equal(dtype[[3, 3, 2], [4, 1, 4]], na)
      end

      test "#{dtype}#triu/triu!" do
        a = dtype.new(3, 3).seq + 1
        na = a.triu
        assert_equal(dtype[[1, 2, 3], [0, 5, 6], [0, 0, 9]], na)
        na = a.triu(1)
        assert_equal(dtype[[0, 2, 3], [0, 0, 6], [0, 0, 0]], na)
        na = a.triu(-1)
        assert_equal(dtype[[1, 2, 3], [4, 5, 6], [0, 8, 9]], na)
        av = a[0...2, 0...2]
        assert_equal(false, av.contiguous?)
        av.triu!
        assert_equal(dtype[[1, 2, 3], [0, 5, 6], [7, 8, 9]], a)
      end

      test "#{dtype}#tril/tril!" do
        a = dtype.new(3, 3).seq + 1
        na = a.tril
        assert_equal(dtype[[1, 0, 0], [4, 5, 0], [7, 8, 9]], na)
        na = a.tril(1)
        assert_equal(dtype[[1, 2, 0], [4, 5, 6], [7, 8, 9]], na)
        na = a.tril(-1)
        assert_equal(dtype[[0, 0, 0], [4, 0, 0], [7, 8, 0]], na)
        av = a[1...3, 1...3]
        assert_equal(false, av.contiguous?)
        av.tril!
        assert_equal(dtype[[1, 2, 3], [4, 5, 0], [7, 8, 9]], a)
      end

      test "#{dtype}#diag_indices" do
        a = dtype[1, 2, 3]
        assert_raise(Numo::NArray::ShapeError) { a.diag_indices }
        a = dtype[[1, 2], [3, 4]]
        assert_equal(Numo::Int32[0, 3], a.diag_indices)
        a = dtype[[1, 2, 3], [4, 5, 6]]
        assert_equal(Numo::Int32[0, 4], a.diag_indices)
        a = dtype[[1, 2], [3, 4], [5, 6]]
        assert_equal(Numo::Int32[0, 3], a.diag_indices)
      end

      test "#{dtype}#diag" do
        a = dtype[1, 2]
        na = a.diag
        assert_equal(dtype[[1, 0], [0, 2]], na)
        na = a.diag(1)
        assert_equal(dtype[[0, 1, 0], [0, 0, 2], [0, 0, 0]], na)
        na = a.diag(-1)
        assert_equal(dtype[[0, 0, 0], [1, 0, 0], [0, 2, 0]], na)
      end

      test "#{dtype}#trace" do
        a = dtype.new(3, 3).seq + 1
        tr = a.trace
        assert_equal(15, tr)
      end

      sub_test_case "#{dtype}#dot" do
        test 'vector.dot(vector)' do
          a = dtype[1..3]
          b = dtype[2..4]
          assert { a.dot(b) == ((1 * 2) + (2 * 3) + (3 * 4)) }
        end
        test 'matrix.dot(vector)' do
          a = dtype[1..6].reshape(3, 2)
          b = dtype[1..2]
          assert { a.dot(b) == [5, 11, 17] }
        end
        test 'vector.dot(matrix)' do
          a = dtype[1..2]
          b = dtype[1..6].reshape(2, 3)
          assert { a.dot(b) == [9, 12, 15] }
        end
        test 'matrix.dot(matrix)' do
          a = dtype[1..6].reshape(3, 2)
          b = dtype[1..6].reshape(2, 3)
          assert { a.dot(b) == [[9, 12, 15], [19, 26, 33], [29, 40, 51]] }
          assert { b.dot(a) == [[22, 28], [49, 64]] }
        end
        test 'matrix.dot(matrix) with incorrect shape' do
          a = dtype[1..6].reshape(3, 2)
          b = dtype[1..9].reshape(3, 3)
          assert_raise(Numo::NArray::ShapeError) { a.dot(b) }
        end
      end

      test "#{dtype}#inner" do
        a = dtype[1, 2, 3]
        b = dtype[4, 5, 6]
        assert_equal(32, a.inner(b))
      end

      test "#{dtype}#outer" do
        a = dtype[1, 2]
        b = dtype[3, 4]
        assert_equal(dtype[[3, 4], [6, 8]], a.outer(b))
        assert_equal(dtype[[3, 6], [4, 8]], b.outer(a, axis: 0))
        assert_raise(ArgumentError) { a.outer(b, axis: 1) }
      end

      test "#{dtype}#percentile" do
        unless [Numo::DComplex, Numo::SComplex, Numo::RObject].include?(dtype)
          a = dtype[[1, 2, 3], [5, 7, 11]]
          assert_in_delta(a.percentile(0), 1.0, 1e-14)
          assert_in_delta(a.percentile(50), 4.0, 1e-14)
          assert_in_delta(a.percentile(90), 9.0, 1e-14)
          assert_in_delta(a.percentile(100), 11.0, 1e-14)
          assert { a.percentile(0, axis: 0) == [1, 2, 3] }
          assert { a.percentile(50, axis: 0) == [3, 4.5, 7] }
          assert { a.percentile(90, axis: 0) == [4.6, 6.5, 10.2] }
          assert { a.percentile(100, axis: 0) == [5, 7, 11] }
          assert { a.percentile(0, axis: 1) == [1, 5] }
          assert { a.percentile(50, axis: 1) == [2, 7] }
          assert { a.percentile(90, axis: 1) == [2.8, 10.2] }
          assert { a.percentile(100, axis: 1) == [3, 11] }
        end
      end

      test "#{dtype}#kron" do
        a = dtype[1, 10, 100]
        assert_equal(dtype[5, 6, 7, 50, 60, 70, 500, 600, 700], a.kron(dtype[5, 6, 7]))
        assert_equal(dtype[[1, 1, 0, 0], [1, 1, 0, 0], [0, 0, 1, 1], [0, 0, 1, 1]],
                     dtype.eye(2).kron(dtype.ones(2, 2)))
      end
    end

    test '#dot with different type arrays' do
      $stderr = StringIO.new
      a = Numo::Int32[1, 2, 3]
      b = Numo::DFloat[[4], [5], [6]]
      assert { a.dot(b).is_a?(Numo::DFloat) }
      assert { a.dot(b) == [32] }
      # no warning message
      assert { $stderr.string == '' }
      $stderr = STDERR
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

      test "#{dtype}#cov" do
        a = dtype[1, 2, 3]
        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          assert_equal(1.0 + 0.0i, a.cov)
        else
          assert_equal(1.0, a.cov)
        end
        a = dtype[[1, 2], [3, 4]]
        assert_equal(dtype[[0.5, 0.5], [0.5, 0.5]], a.cov)
      end
    end
  end

  sub_test_case 'class methods' do
    TYPES.each do |dtype|
      test "#{dtype}.cast" do
        a = dtype.cast(dtype[1, 2, 3])
        assert { a.is_a?(dtype) }
        assert_equal(1, a.ndim)
        assert_equal(3, a.size)
        a = dtype.cast([1, 2, 3])
        assert { a.is_a?(dtype) }
        assert_equal(1, a.ndim)
        assert_equal(3, a.size)
        a = dtype.cast(1)
        assert { a.is_a?(dtype) }
        assert_equal(0, a.ndim)
        assert_equal(1, a.size)
        if [Numo::RObject].include?(dtype)
          a = dtype.cast('a')
          assert { a.is_a?(dtype) }
          assert_equal(0, a.ndim)
          assert_equal(1, a.size)
        else
          assert_raise(Numo::NArray::CastError) { dtype.cast('a') }
        end
      end

      test "#{dtype}.asarray" do
        a = dtype.asarray(dtype[1, 2, 3])
        assert_equal(dtype[1, 2, 3], a)
        a = dtype.asarray([1, 2, 3])
        assert_equal(dtype[1, 2, 3], a)
        a = dtype.asarray(dtype.cast(1))
        assert_equal(dtype[1], a)
        a = dtype.asarray(1)
        assert_equal(dtype[1], a)
        a = dtype.asarray(1..3)
        assert_equal(dtype[1, 2, 3], a)
        if [Numo::RObject].include?(dtype)
          a = dtype.asarray('a')
          assert_equal(dtype.cast('a'), a)
        end
      end

      test "#{dtype}.parse" do
        a = dtype.parse('1 2 3')
        assert_equal(dtype[[1, 2, 3]], a)
        a = dtype.parse('1 2 3; 4 5 6')
        assert_equal(dtype[[1, 2, 3], [4, 5, 6]], a)
        a = dtype.parse('1 2; 3 4 & 5 6; 7 8', split3d: '&')
        assert_equal(dtype[[[1, 2], [3, 4]], [[5, 6], [7, 8]]], a)
      end

      test "#{dtype}.concatenate" do
        a = dtype[[1, 2], [3, 4]]
        b = dtype[[5, 6]]
        na = dtype.concatenate([a, b], axis: 0)
        assert_equal(dtype[[1, 2], [3, 4], [5, 6]], na)
        na = dtype.concatenate([a, b.transpose], axis: 1)
        assert_equal(dtype[[1, 2, 5], [3, 4, 6]], na)
        a = dtype[1, 2]
        na = dtype.concatenate([a, [3, 4]])
        assert_equal(dtype[1, 2, 3, 4], na)
        na = dtype.concatenate([a, 3])
        assert_equal(dtype[1, 2, 3], na)
        assert_raise(TypeError) { dtype.concatenate([a, '3']) }
      end

      test "#{dtype}.vstack" do
        a = dtype[1, 2]
        b = dtype[3, 4]
        na = dtype.vstack([a, b])
        assert_equal(dtype[[1, 2], [3, 4]], na)
      end

      test "#{dtype}.hstack" do
        a = dtype[1, 2]
        b = dtype[3, 4]
        na = dtype.hstack([a, b])
        assert_equal(dtype[1, 2, 3, 4], na)
      end

      test "#{dtype}.dstack" do
        a = dtype[1, 2]
        b = dtype[3, 4]
        na = dtype.dstack([a, b])
        assert_equal(dtype[[[1, 3], [2, 4]]], na)
      end

      test "#{dtype}.column_stack" do
        a = dtype.column_stack([dtype[1, 2], dtype[3, 4]])
        assert_equal(dtype[[1, 3], [2, 4]], a)
        a = dtype.column_stack([dtype.cast(1), dtype.cast(2)])
        assert_equal(dtype[[1, 2]], a)
        a = dtype.column_stack([dtype[[1, 2], [3, 4]], dtype[[5, 6], [7, 8]]])
        assert_equal(dtype[[1, 2, 5, 6], [3, 4, 7, 8]], a)
      end
    end

    test 'Numo::NArray.cast' do
      a = Numo::NArray.cast([1, 2, 3])
      assert { a.is_a?(Numo::Int32) }
      assert_equal(1, a.ndim)
      assert_equal(3, a.size)
      a = Numo::NArray.cast(1)
      assert { a.is_a?(Numo::Int32) }
      assert_equal(0, a.ndim)
      assert_equal(1, a.size)
      assert_raise(TypeError) { Numo::NArray.cast('1') }
    end
  end
end
