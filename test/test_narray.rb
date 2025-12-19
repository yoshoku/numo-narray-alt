# frozen_string_literal: true

require_relative 'test_helper'

class NArrayTest < NArrayTestBase
  def test_inheritance_relationship
    TYPES.each do |dtype|
      assert_operator(dtype, :<, Numo::NArray)
    end
  end

  def test_altertive
    assert(Numo::NArray::ALTERNATIVE)
    assert_predicate(Numo::NArray, :alternative?)
  end

  def test_empty_narray # rubocop:disable Minitest/MultipleAssertions
    TYPES.each do |dtype|
      a = dtype[]
      assert_raises(Numo::NArray::ShapeError) { a[true] }
      assert_raises(Numo::NArray::ShapeError) { a[1..] }
      assert_empty(a)
      assert_equal(1, a.ndim)
      assert_equal([0], a.shape)
      refute_predicate(a, :inplace?)
      assert_predicate(a, :row_major?)
      refute_predicate(a, :column_major?)
      assert_predicate(a, :host_order?)
      refute_predicate(a, :byte_swapped?)
      assert_empty(a.to_a)
      assert_kind_of(Array, a.to_a)
      assert_equal(a, a.dup)
      assert_equal(a, a.clone)
      refute_same(a.dup, a)
      refute_same(a.clone, a)
      TYPES.each do |other_dtype|
        next if dtype == other_dtype

        assert_equal(other_dtype[], dtype[])
      end
    end
  end

  def test_1d_narray # rubocop:disable Metrics/AbcSize, Metrics/MethodLength, Minitest/MultipleAssertions
    TYPES.each do |dtype|
      [[proc { |tp, a| tp[*a] }, ''],
       [proc { |tp, a| tp[*a][true] }, '[true]'],
       [proc { |tp, a| tp[*a] }, '[0..-1]']].each do |init, _ref|
        # test "#{dtype},[1,2,3,5,7,11]#{ref}" do
        src = [1, 2, 3, 5, 7, 11]
        a = init.call(dtype, src)

        assert_kind_of(dtype, a)
        assert_equal(6, a.size)
        assert_equal(1, a.ndim)
        assert_equal([6], a.shape)
        refute_predicate(a, :inplace?)
        assert_predicate(a, :row_major?)
        refute_predicate(a, :column_major?)
        assert_predicate(a, :host_order?)
        refute_predicate(a, :byte_swapped?)
        assert_equal(dtype[1, 2, 3, 5, 7, 11], a)
        assert_equal([1, 2, 3, 5, 7, 11], a.to_a)
        assert_kind_of(Array, a.to_a)
        assert_equal(a, a.dup)
        assert_equal(a, a.clone)
        refute_same(a.dup, a)
        refute_same(a.clone, a)

        assert_equal(Numo::Bit[1, 0, 1, 0, 1, 0], a.eq([1, 1, 3, 3, 7, 7]))
        assert_equal(dtype[5, 7], a[3..4])
        assert_equal(11, a[5])
        assert_equal(11, a[-1])

        assert_equal(dtype[5, 7], a.at([3, 4]))
        assert_equal(dtype[5, 7], a.view.at([3, 4]))
        assert_equal(dtype[5, 7], a[2..].at([1, 2]))
        assert_equal(dtype[5, 7], a.at(Numo::Int32.cast([3, 4])))
        assert_equal(dtype[5, 7], a.view.at(Numo::Int32.cast([3, 4])))
        assert_equal(dtype[5, 7], a.at(3..4))
        assert_equal(dtype[5, 7], a.view.at(3..4))
        assert_equal(dtype[11], a.at([5]))
        assert_equal(dtype[11], a.view.at([5]))
        assert_equal(dtype[11], a.at([-1]))
        assert_equal(dtype[11], a.view.at([-1]))

        assert_equal(dtype[1, 2, 3, 5, 7, 11], a[(0..-1).each])
        assert_equal(dtype[1, 2, 3, 5, 7], a[(0...-1).each])

        if Enumerator.const_defined?(:ArithmeticSequence)
          assert_equal(dtype[1, 2, 3, 5, 7, 11], a[0.step(-1)])
          assert_equal(dtype[1, 2, 3, 5, 7],    a[0.step(4)])
          assert_equal(dtype[2, 3, 5, 7, 11],   a[-5.step(-1)])
          assert_equal(dtype[1, 3, 7],          a[0.step(-1, 2)])
          assert_equal(dtype[1, 3, 7],          a[0.step(4, 2)])
          assert_equal(dtype[2, 5, 11],         a[-5.step(-1, 2)])

          assert_equal(dtype[1, 2, 3, 5, 7, 11], a[0.step])
          assert_equal(dtype[2, 3, 5, 7, 11], a[-5.step])
          # rubocop:disable Style/EvalWithLocation
          assert_equal(dtype[1, 3, 7], eval('a[(0..).step(2)]'))
          assert_equal(dtype[1, 3, 7], eval('a[(0...).step(2)]'))
          assert_equal(dtype[2, 5, 11], eval('a[(-5..).step(2)]'))
          assert_equal(dtype[2, 5, 11], eval('a[(-5...).step(2)]'))
          assert_equal(dtype[1, 3, 7], eval('a[(0..) % 2]'))
          assert_equal(dtype[1, 3, 7], eval('a[(0...) % 2]'))
          assert_equal(dtype[2, 5, 11], eval('a[(-5..) % 2]'))
          assert_equal(dtype[2, 5, 11], eval('a[(-5...) % 2]'))
          # rubocop:enable Style/EvalWithLocation
        end

        assert_equal(dtype[1, 3, 7], a[(0..-1).step(2)])
        assert_equal(dtype[1, 3, 7], a[(0...-1).step(2)])
        assert_equal(dtype[1, 3, 7], a[(0..4).step(2)])
        assert_equal(dtype[1, 3], a[(0...4).step(2)])
        assert_equal(dtype[2, 5, 11], a[(-5..-1).step(2)])
        assert_equal(dtype[2, 5], a[(-5...-1).step(2)])
        assert_equal(dtype[1, 3, 7], a[(0..-1) % 2])
        assert_equal(dtype[1, 3, 7], a[(0...-1) % 2])
        assert_equal(dtype[1, 3, 7], a[(0..4) % 2])
        assert_equal(dtype[1, 3], a[(0...4) % 2])
        assert_equal(dtype[2, 5, 11], a[(-5..-1) % 2])
        assert_equal(dtype[2, 5], a[(-5...-1) % 2])
        assert_equal(dtype[7, 5, 1, 2, 11, 3], a[[4, 3, 0, 1, 5, 2]]) #  Numo::NArray::CastError: cannot convert to NArray
        assert_equal(dtype[11, 7, 5, 3, 2, 1], a.reverse)
        assert_equal(29, a.sum)
        assert_in_delta(4.833333, a.mean, 1e-6)
        assert_in_delta(13.766666, a.var, 1e-5)
        assert_in_delta(3.710345, a.stddev, 1e-6)
        assert_in_delta(5.901977, a.rms, 1e-6)
        assert_equal(dtype[12, 12, 12, 12, 12, 12], a.dup.fill(12))
        assert_equal(dtype[2, 3, 4, 6, 8, 12], a + 1)
        assert_equal(dtype[0, 1, 2, 4, 6, 10], a - 1)
        assert_equal(dtype[3, 6, 9, 15, 21, 33], a * 3)
        assert_equal(dtype[2, 4, 6, 10, 14, 22], a / 0.5)
        assert_equal(dtype[-1, -2, -3, -5, -7, -11], -a)
        assert_equal(dtype[1, 4, 9, 25, 49, 121], a**2)
        assert_equal(Numo::SFloat[1, 2, 3, 5, 7, 11], a.swap_byte.swap_byte) if dtype != Numo::RObject

        assert_predicate(a, :contiguous?)
        assert_predicate(a.transpose, :contiguous?)

        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          assert_equal(Numo::SFloat.asarray(src), a.real)
          assert_equal(Numo::SFloat[0, 0, 0, 0, 0, 0], a.imag)
          assert_equal(Numo::SFloat.asarray(src), a.conj)
          assert_equal(Numo::SFloat[0, 0, 0, 0, 0, 0], a.angle)
        else
          assert_equal(1, a.min)
          assert_equal(11, a.max)
          assert_equal(0, a.min_index)
          assert_equal(5, a.max_index)
          assert_equal(Numo::Bit[0, 0, 1, 1, 1, 1], a >= 3)
          assert_equal(Numo::Bit[0, 0, 0, 1, 1, 1], a > 3)
          assert_equal(Numo::Bit[1, 1, 1, 0, 0, 0], a <= 3)
          assert_equal(Numo::Bit[1, 1, 0, 0, 0, 0], a < 3)
          assert_equal(Numo::Bit[0, 0, 1, 0, 0, 0], a.eq(3))
          if dtype != Numo::RObject
            assert_equal(Numo::SFloat.asarray(src), a.sort)
            assert_equal(Numo::Int32[0, 1, 2, 3, 4, 5], a.sort_index)
            assert_equal(4, a.median)
          end
          assert_equal(dtype[11, 10, 9, 7, 7, 11], dtype.maximum(a, 12 - a))
          assert_equal(dtype[1, 2, 3, 5, 5, 1], dtype.minimum(a, 12 - a))
          assert_equal(dtype[5, 5, 5, 5, 7, 11], dtype.maximum(a, 5))
          assert_equal(dtype[1, 2, 3, 5, 5, 5], dtype.minimum(a, 5))
          assert_equal(dtype[1, 3, 6, 11, 18, 29], a.cumsum)
          assert_equal(dtype[1, 2, 6, 30], a[0...4].cumprod)
        end
      end
    end
  end

  def test_array_indexing
    TYPES.each do |dtype|
      assert_equal(dtype[1, 2, 3, 4], dtype[1..4])
      assert_equal(dtype[-4, -3, -2, -1], dtype[-4..-1]) # rubocop:disable Style/SlicingWithRange

      if Enumerator.const_defined?(:ArithmeticSequence)
        assert_equal(dtype[1, 2, 3, 4], dtype[1.step(4)])
        assert_equal(dtype[-4, -3, -2, -1], dtype[-4.step(-1)])
        assert_equal(dtype[1, 3], dtype[1.step(4, 2)])
        assert_equal(dtype[-4, -2], dtype[-4.step(-1, 2)])
        assert_equal(dtype[-4, -2], dtype[(-4..-1).step(2)])
      end

      assert_equal(dtype[1, 3], dtype[(1..4) % 2])
      assert_equal(dtype[-4, -2], dtype[(-4..-1) % 2])
    end
  end

  def test_seq
    TYPES.each do |dtype|
      assert_equal(dtype[0, 1, 2, 3, 4], dtype.new(5).seq)
    end
  end

  def test_2d_narray # rubocop:disable Metrics/AbcSize, Metrics/MethodLength, Minitest/MultipleAssertions
    TYPES.each do |dtype|
      [[proc { |tp, src| tp[*src] }, ''],
       [proc { |tp, src| tp[*src][true, true] }, '[true,true]'],
       [proc { |tp, src| tp[*src][0..-1, 0..-1] }, '[0..-1,0..-1]']].each do |init, _ref|
        src = [[1, 2, 3], [5, 7, 11]]
        a = init.call(dtype, src)

        assert_kind_of(dtype, a)
        assert_equal(6, a.size)
        assert_equal(2, a.ndim)
        assert_equal([2, 3], a.shape)
        refute_predicate(a, :inplace?)
        assert_predicate(a, :row_major?)
        refute_predicate(a, :column_major?)
        assert_predicate(a, :host_order?)
        refute_predicate(a, :byte_swapped?)
        assert_equal(dtype.asarray(src), a)
        assert_equal(src, a.to_a)
        assert_kind_of(Array, a.to_a)

        assert_equal(Numo::Bit[[1, 0, 1], [0, 1, 0]], a.eq([[1, 1, 3], [3, 7, 7]]))
        assert_equal(11, a[5])
        assert_equal(11, a[-1])
        assert_equal(src[1][0], a[1, 0])
        assert_equal(src[1][1], a[1, 1])
        assert_equal(src[1][2], a[1, 2])
        assert_equal(dtype[5, 7], a[3..4])
        assert_equal(dtype[2, 3], a[0, 1..2])

        assert_equal(dtype[2, 11], a.at([0, 1], [1, 2]))
        assert_equal(dtype[2, 11], a.view.at([0, 1], [1, 2]))
        assert_equal(dtype[1, 11], a.at([0, 1], (0..2) % 2))
        assert_equal(dtype[1, 11], a.view.at([0, 1], (0..2) % 2))
        assert_equal(dtype[1, 11], a.at((0..1) % 1, [0, 2]))
        assert_equal(dtype[1, 11], a.view.at((0..1) % 1, [0, 2]))
        assert_equal(dtype[2, 11], a.at(Numo::Int32.cast([0, 1]), Numo::Int32.cast([1, 2])))
        assert_equal(dtype[2, 11], a.view.at(Numo::Int32.cast([0, 1]), Numo::Int32.cast([1, 2])))
        assert_equal(dtype[1, 11], a[[0, 1], [0, 2]].at([0, 1], [0, 1]))
        assert_equal(dtype[1, 11], a[[0, 1], (0..2) % 2].at([0, 1], [0, 1]))
        assert_equal(dtype[1, 11], a[(0..1) % 1, [0, 2]].at([0, 1], [0, 1]))
        assert_equal(dtype[1, 11], a[(0..1) % 1, (0..2) % 2].at([0, 1], [0, 1]))

        assert_equal(dtype.asarray(src[0]), a[0, :*])
        assert_equal(dtype.asarray(src[1]), a[1, :*])
        assert_equal(dtype[src[0][1], src[1][1]], a[:*, 1])
        assert_equal(dtype[[3, 1, 2], [11, 5, 7]], a[true, [2, 0, 1]])
        assert_equal(dtype[[1, 2], [3, 5], [7, 11]], a.reshape(3, 2))
        assert_equal(dtype[[1, 2], [3, 5], [7, 11]], a.reshape(3, nil))
        assert_equal(dtype[[1, 2], [3, 5], [7, 11]], a.reshape(nil, 2))
        assert_equal(dtype[[1, 5], [2, 7], [3, 11]], a.transpose)
        assert_equal(dtype[[1, 5], [2, 7], [3, 11]], a.transpose(1, 0))
        assert_equal(dtype[[11, 7, 5], [3, 2, 1]], a.reverse)
        assert_equal(dtype[[11, 7, 5], [3, 2, 1]], a.reverse(0, 1))
        assert_equal(dtype[[11, 7, 5], [3, 2, 1]], a.reverse(1, 0))
        assert_equal(dtype[[5, 7, 11], [1, 2, 3]], a.reverse(0))
        assert_equal(dtype[[3, 2, 1], [11, 7, 5]], a.reverse(1))

        assert_equal(29, a.sum)
        a_sum_axis0 = a.sum(axis: 0)
        a_sum_axis1 = a.sum(axis: 1)
        if INTEGER_TYPES.include?(dtype)
          assert_equal(Numo::Int64[6, 9, 14], a_sum_axis0)
          assert_kind_of(Numo::Int64, a_sum_axis0)
          assert_equal(Numo::Int64[6, 23], a_sum_axis1)
          assert_kind_of(Numo::Int64, a_sum_axis1)
        elsif UNSIGNED_INTEGER_TYPES.include?(dtype)
          assert_equal(Numo::UInt64[6, 9, 14], a_sum_axis0)
          assert_kind_of(Numo::UInt64, a_sum_axis0)
          assert_equal(Numo::UInt64[6, 23], a_sum_axis1)
          assert_kind_of(Numo::UInt64, a_sum_axis1)
        else
          assert_equal(dtype[6, 9, 14], a_sum_axis0)
          assert_kind_of(dtype, a_sum_axis0)
          assert_equal(dtype[6, 23], a_sum_axis1)
          assert_kind_of(dtype, a_sum_axis1)
        end

        assert_equal(2310, a.prod)
        a_prod_axis0 = a.prod(axis: 0)
        a_prod_axis1 = a.prod(axis: 1)
        if INTEGER_TYPES.include?(dtype)
          assert_equal(Numo::Int64[5, 14, 33], a_prod_axis0)
          assert_kind_of(Numo::Int64, a_prod_axis0)
          assert_equal(Numo::Int64[6, 385], a_prod_axis1)
          assert_kind_of(Numo::Int64, a_prod_axis1)
        elsif UNSIGNED_INTEGER_TYPES.include?(dtype)
          assert_equal(Numo::UInt64[5, 14, 33], a_prod_axis0)
          assert_kind_of(Numo::UInt64, a_prod_axis0)
          assert_equal(Numo::UInt64[6, 385], a_prod_axis1)
          assert_kind_of(Numo::UInt64, a_prod_axis1)
        else
          assert_equal(dtype[5, 14, 33], a_prod_axis0)
          assert_kind_of(dtype, a_prod_axis0)
          assert_equal(dtype[6, 385], a_prod_axis1)
          assert_kind_of(dtype, a_prod_axis1)
        end

        assert_in_delta(4.833333, a.mean, 1e-6)
        if FLOAT_TYPES.include?(dtype)
          assert_equal(dtype[3, 4.5, 7], a.mean(0))
          assert_equal(dtype[2, 23.0 / 3], a.mean(1))
        else
          assert_equal(Numo::DFloat[3, 4.5, 7], a.mean(0))
          assert_equal(Numo::DFloat[2, 23.0 / 3], a.mean(1))
        end

        assert_equal(dtype[[1, 3, 6], [11, 18, 29]], a.cumsum)
        assert_equal(dtype[[1, 2, 3], [6, 9, 14]], a.cumsum(0))
        assert_equal(dtype[[1, 3, 6], [5, 12, 23]], a.cumsum(1))
        assert_equal(dtype[[1, 2], [10, 70]], a[true, 0...2].cumprod)
        assert_equal(dtype[[1, 2], [5, 14]], a[true, 0...2].cumprod(0))
        assert_equal(dtype[[1, 2], [5, 35]], a[true, 0...2].cumprod(1))

        assert_predicate(a, :contiguous?)
        assert_predicate(a.reshape(3, 2), :contiguous?)
        refute_predicate(a[true, 1..2], :contiguous?)
        refute_predicate(a.transpose, :contiguous?)
        refute_predicate(a, :fortran_contiguous?)
        assert_predicate(a.transpose, :fortran_contiguous?)
        refute_predicate(a.transpose.transpose, :fortran_contiguous?)
        refute_predicate(a.reshape(3, 2), :fortran_contiguous?)
        assert_predicate(a.reshape(3, 2).transpose, :fortran_contiguous?)
        refute_predicate(a[true, 1..2], :fortran_contiguous?)
        refute_predicate(a[true, 1..2].transpose, :fortran_contiguous?)

        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          assert_equal(Numo::SFloat.asarray(src), a.real)
          assert_equal(Numo::SFloat[[0, 0, 0], [0, 0, 0]], a.imag)
          assert_equal(Numo::SFloat.asarray(src), a.conj)
          assert_equal(Numo::SFloat[[0, 0, 0], [0, 0, 0]], a.angle)
        else
          assert_equal(1, a.min)
          assert_equal(dtype[1, 5], a.min(axis: 1))
          assert_equal(dtype[1, 2, 3], a.min(axis: 0))
          assert_equal(11, a.max)
          assert_equal(dtype[3, 11], a.max(axis: 1))
          assert_equal(dtype[5, 7, 11], a.max(axis: 0))
          assert_equal(10, a.ptp)
          assert_equal(dtype[2, 6], a.ptp(axis: 1))
          assert_equal(dtype[4, 5, 8], a.ptp(axis: 0))
          assert_equal(0, a.min_index)
          assert_equal(dtype[0, 3], a.min_index(axis: 1))
          assert_equal(dtype[0, 1, 2], a.min_index(axis: 0))
          assert_equal(dtype[2, 5], a.max_index(axis: 1))
          assert_equal(dtype[3, 4, 5], a.max_index(axis: 0))
          assert_equal(Numo::Bit[[0, 0, 1], [1, 1, 1]], a >= 3)
          assert_equal(Numo::Bit[[0, 0, 0], [1, 1, 1]], a > 3)
          assert_equal(Numo::Bit[[1, 1, 1], [0, 0, 0]], a <= 3)
          assert_equal(Numo::Bit[[1, 1, 0], [0, 0, 0]], a < 3)
          assert_equal(Numo::Bit[[0, 0, 1], [0, 0, 0]], a.eq(3))
          assert_equal(dtype[1, 2, 5, 7, 11], a[a.ne(3)])
          assert_equal(dtype[[1, 2, 3]], a[a[true, 2] < 5, true])
          assert_equal(dtype[[2, 3], [7, 11]], a[true, a[1, true] > 5])
          assert_equal(dtype[[1, 3], [5, 11]], a[:*, (a[0, :*] % 2).eq(1)])
          if dtype != Numo::RObject
            assert_equal(Numo::SFloat.asarray(src), a.sort)
            assert_equal(Numo::SFloat[[0, 1, 2], [3, 4, 5]], a.sort_index)
          end
        end
        assert_equal(dtype[[12, 12, 12], [12, 12, 12]], a.dup.fill(12))
        assert_equal(dtype[[2, 3, 4], [6, 8, 12]], a + 1)
        assert_equal(dtype[[2, 4, 6], [6, 9, 14]], a + [1, 2, 3])
        assert_equal(dtype[[0, 1, 2], [4, 6, 10]], a - 1)
        assert_equal(dtype[[0, 0, 0], [4, 5, 8]], a - [1, 2, 3])
        assert_equal(dtype[[3, 6, 9], [15, 21, 33]], a * 3)
        assert_equal(dtype[[1, 4, 9], [5, 14, 33]], a * [1, 2, 3])
        assert_equal(dtype[[2, 4, 6], [10, 14, 22]], a / 0.5)
        assert_equal(dtype[[-1, -2, -3], [-5, -7, -11]], -a)
        assert_equal(dtype[[1, 4, 9], [25, 49, 121]], a**2)
        assert_equal(Numo::SFloat.asarray(src), a.swap_byte.swap_byte) if dtype != Numo::RObject
      end
    end
  end

  def test_3d_narray # rubocop:disable Metrics/AbcSize, Minitest/MultipleAssertions
    TYPES.each do |dtype|
      a = dtype[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]

      assert_equal(4, a[0, 1, 1])
      assert_equal(a, a[:rest])
      assert_equal(dtype[[1, 2], [3, 4]], a[0, :rest])
      assert_equal(dtype[[1, 2], [3, 4]], a[0, false])
      assert_equal(dtype[3, 4], a[0, 1, :rest])
      assert_equal(dtype[3, 4], a[0, 1, false])
      assert_equal(dtype[[1, 3], [5, 7]], a[:rest, 0])
      assert_equal(dtype[2, 6], a[:rest, 0, 1])
      assert_equal(dtype[5, 7], a[1, :rest, 0])
      assert_equal(7, a[1, 1, :rest, 0])
      assert_raises(IndexError) { a[1, 1, 1, 1, :rest] }
      assert_raises(IndexError) { a[1, 1, 1, :rest, 1] }
      assert_raises(IndexError) { a[:rest, 1, :rest, 0] }

      assert_equal(dtype[3, 6], a.at([0, 1], [1, 0], [0, 1]))
      assert_equal(dtype[3, 6], a.view.at([0, 1], [1, 0], [0, 1]))

      assert_equal(dtype[[[1, 5], [3, 7]], [[2, 6], [4, 8]]], a.transpose)
      assert_equal(dtype[[[1, 5], [3, 7]], [[2, 6], [4, 8]]], a.transpose(2, 1, 0))
      assert_equal(dtype[[[1, 3], [2, 4]], [[5, 7], [6, 8]]], a.transpose(0, 2, 1))

      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse)
      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse(0, 1, 2))
      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse(-3, -2, -1))
      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse(0..2))
      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse(-3..-1))
      assert_equal(dtype[[[8, 7], [6, 5]], [[4, 3], [2, 1]]], a.reverse(0...3))
      assert_equal(dtype[[[5, 6], [7, 8]], [[1, 2], [3, 4]]], a.reverse(0))
      assert_equal(dtype[[[3, 4], [1, 2]], [[7, 8], [5, 6]]], a.reverse(1))
      assert_equal(dtype[[[2, 1], [4, 3]], [[6, 5], [8, 7]]], a.reverse(2))
      assert_equal(dtype[[[7, 8], [5, 6]], [[3, 4], [1, 2]]], a.reverse(0, 1))
      assert_equal(dtype[[[7, 8], [5, 6]], [[3, 4], [1, 2]]], a.reverse(0..1))
      assert_equal(dtype[[[7, 8], [5, 6]], [[3, 4], [1, 2]]], a.reverse(0...2))
      assert_equal(dtype[[[6, 5], [8, 7]], [[2, 1], [4, 3]]], a.reverse(0, 2))
      assert_equal(dtype[[[6, 5], [8, 7]], [[2, 1], [4, 3]]], a.reverse((0..2) % 2))
      assert_equal(dtype[[[6, 5], [8, 7]], [[2, 1], [4, 3]]], a.reverse((0..2).step(2)))

      assert_predicate(a, :contiguous?)
      refute_predicate(a.transpose, :contiguous?)
      refute_predicate(a, :fortran_contiguous?)
      assert_predicate(a.transpose, :fortran_contiguous?)
      refute_predicate(a.transpose.transpose, :fortran_contiguous?)
      refute_predicate(a.transpose(0, 2, 1), :fortran_contiguous?)
      refute_predicate(a.reshape(2, 4), :fortran_contiguous?)
      assert_predicate(a.reshape(2, 4).transpose, :fortran_contiguous?)
    end
  end

  def test_simd # rubocop:disable Metrics/AbcSize, Metrics/MethodLength, Minitest/MultipleAssertions
    TYPES.each do |dtype|
      # test 'no simd add' do
      a = dtype[4..6]
      b = dtype[1..3]
      assert_equal(dtype[5, 7, 9], a + b)
      assert_equal(dtype[5, 7, 9], a.inplace + b)
      # test 'no simd sub' do
      a = dtype[4..6]
      b = dtype[1..3]
      assert_equal(dtype[3, 3, 3], a - b)
      assert_equal(dtype[3, 3, 3], a.inplace - b)
      # test 'no simd mul' do
      a = dtype[4..6]
      b = dtype[1..3]
      assert_equal(dtype[4, 10, 18], a * b)
      assert_equal(dtype[4, 10, 18], a.inplace * b)
      # test 'no simd div' do
      a = dtype[4..6]
      b = dtype[1..3]
      assert_equal(dtype[4, 2.5, 2], a / b)
      assert_equal(dtype[4, 2.5, 2], a.inplace / b)
      if dtype != Numo::RObject
        # test 'no simd sqrt' do
        a = dtype[4, 9, 16]
        assert_equal(Numo::SFloat[2, 3, 4], Numo::NMath.sqrt(a))
        assert_equal(Numo::SFloat[2, 3, 4], Numo::NMath.sqrt(a.inplace))
      end

      # test 'simd add' do
      a = dtype[11..19]
      b = dtype.ones(9)
      assert_equal(dtype[12, 13, 14, 15, 16, 17, 18, 19, 20], a + b)
      assert_equal(dtype[12, 13, 14, 15, 16, 17, 18, 19, 20], a.inplace + b)
      # test 'simd sub' do
      a = dtype[11..19]
      b = dtype.ones(9)
      assert_equal(dtype[10, 11, 12, 13, 14, 15, 16, 17, 18], a - b)
      assert_equal(dtype[10, 11, 12, 13, 14, 15, 16, 17, 18], a.inplace - b)
      # test 'simd mul' do
      a = dtype[11..19]
      b = dtype.ones(9) * 2
      assert_equal(dtype[22, 24, 26, 28, 30, 32, 34, 36, 38], a * b)
      assert_equal(dtype[22, 24, 26, 28, 30, 32, 34, 36, 38], a.inplace * b)
      # test 'simd div' do
      a = dtype[11..19]
      b = dtype.ones(9) * 2
      assert_equal(dtype[5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5], a / b)
      assert_equal(dtype[5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5], a.inplace / b)
      if dtype != Numo::RObject
        # test 'simd sqrt' do
        a = dtype[4, 9, 16, 25, 36, 49, 64, 81, 100]
        assert_equal(Numo::SFloat[2, 3, 4, 5, 6, 7, 8, 9, 10], Numo::NMath.sqrt(a))
        assert_equal(Numo::SFloat[2, 3, 4, 5, 6, 7, 8, 9, 10], Numo::NMath.sqrt(a.inplace))
      end

      # test 'simd broadcast scalar add' do
      a = dtype[1..9]
      assert_equal(dtype[2, 3, 4, 5, 6, 7, 8, 9, 10], a + 1)
      assert_equal(dtype[2, 3, 4, 5, 6, 7, 8, 9, 10], a.inplace + 1)

      # test 'simd broadcast not scalar add' do
      a = dtype[10..19].reshape(2, 5)
      b = dtype[10..14]
      assert_equal(dtype[[20, 22, 24, 26, 28], [25, 27, 29, 31, 33]], a + b)
      assert_equal(dtype[[20, 22, 24, 26, 28], [25, 27, 29, 31, 33]], a.inplace + b)

      # test 'simd view add' do
      a = dtype[10..19][1..9]
      b = dtype[10..19][1..9]
      assert_equal(dtype[22, 24, 26, 28, 30, 32, 34, 36, 38], a + b)
      assert_equal(dtype[22, 24, 26, 28, 30, 32, 34, 36, 38], a.inplace + b)

      next unless dtype != Numo::RObject

      # test 'simd view sqrt' do
      a = dtype[1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
      assert_equal(Numo::SFloat[2, 3, 4, 5, 6, 7, 8, 9, 10], Numo::NMath.sqrt(a[1..9]))
      assert_equal(Numo::SFloat[2, 3, 4, 5, 6, 7, 8, 9, 10], Numo::NMath.sqrt(a[1..9].inplace))
    end
  end

  def test_advanced_indexing # rubocop:disable Metrics/AbcSize, Minitest/MultipleAssertions
    TYPES.each do |dtype|
      # test "#{dtype},advanced indexing" do
      a = dtype[[1, 2, 3], [4, 5, 6]]
      assert_equal(dtype[[1, 2], [4, 5]], a[[0, 1], [0, 1]].dup)
      assert_equal(12, a[[0, 1], [0, 1]].sum)
      assert_equal(dtype[1, 5], a[[0, 1], [0, 1]].diagonal)

      diag = a.dup[[0, 1], [0, 1]].diagonal
      diag.inplace - 1
      assert_equal(dtype[0, 4], diag)
      assert_equal(dtype[1, 5], a.at([0, 1], [0, 1]).dup)

      at = a.dup
      at.at([0, 1], [0, 1]).inplace - 1
      assert_equal(dtype[[0, 2, 3], [4, 4, 6]], at)

      next unless dtype != Numo::RObject

      # sub_test_case "#{dtype}.from_binary" do
      # test 'frozen string' do
      shape = [2, 5]
      a = dtype.new(*shape)
      a.rand(0, 10)
      original_data = a.to_binary
      data = original_data.dup.freeze
      restored_a = dtype.from_binary(data, shape)
      assert_equal(a, restored_a)

      restored_a[0, 0] += 1
      refute_equal(restored_a, a)
      assert_equal(original_data, data)

      # test 'not frozen string' do
      shape = [2, 5]
      a = dtype.new(*shape)
      a.rand(0, 10)
      original_data = a.to_binary
      data = original_data.dup
      restored_a = dtype.from_binary(data, shape)
      assert_equal(a, restored_a)

      restored_a[0, 0] += 1
      refute_equal(restored_a, a)
      assert_equal(original_data, data)

      # sub_test_case "#{dtype}#store_binary" do
      # test 'frozen string' do
      shape = [2, 5]
      a = dtype.new(*shape)
      a.rand(0, 10)
      original_data = a.to_binary
      data = original_data.dup.freeze
      restored_a = dtype.new(*shape)
      restored_a.store_binary(data)
      assert_equal(a, restored_a)

      restored_a[0, 0] += 1
      refute_equal(restored_a, a)
      assert_equal(original_data, data)

      # test 'not frozen string' do
      shape = [2, 5]
      a = dtype.new(*shape)
      a.rand(0, 10)
      original_data = a.to_binary
      data = original_data.dup
      restored_a = dtype.new(*shape)
      restored_a.store_binary(data)
      assert_equal(a, restored_a)

      restored_a[0, 0] += 1
      refute_equal(restored_a, a)
      assert_equal(original_data, data)
    end
  end

  def test_cast_any_object_that_responds_to_to_ary
    obj = Struct.new(:to_a).new([1, 2, 3]) # rubocop:disable Lint/StructNewOverride
    assert_equal(Numo::Int32[1, 2, 3], Numo::NArray.cast(obj))
  end

  def test_zero_dimensional_binary_string
    assert_equal(0x17, Numo::UInt8.from_binary("\x17", []))
  end

  def test_robject_sum
    assert_equal(6, Numo::RObject.cast([1.0, 2.0, 3.0]).sum)
  end

  def test_dfloat_cast_robject
    assert_equal(Numo::DFloat[1, Float::NAN, 3].format_to_a,
                 Numo::DFloat.cast(Numo::RObject[1, nil, 3]).format_to_a)
  end

  def test_sort_index
    rng = Random.new(42)
    arr = Array.new(100) { rng.rand(-1.0...1.0) }
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      a = dtype.asarray(arr)
      idx = a.sort_index
      assert_equal(a.to_a.sort, a[idx].to_a)
    end

    arr = Array.new(100) { rng.rand(-100...100) }
    [Numo::Int64, Numo::Int32, Numo::Int16, Numo::Int8].each do |dtype|
      a = dtype.asarray(arr)
      idx = a.sort_index
      assert_equal(a.to_a.sort, a[idx].to_a)
    end

    arr = Array.new(100) { rng.rand(0...100) }
    [Numo::UInt64, Numo::UInt32, Numo::UInt16, Numo::UInt8].each do |dtype|
      a = dtype.asarray(arr)
      idx = a.sort_index
      assert_equal(a.to_a.sort, a[idx].to_a)
    end
  end
end
