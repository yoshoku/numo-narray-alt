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

        assert_nil(a.coerce_cast(Numo::Bit))
        assert_kind_of(dtype, a)

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
          assert_equal(dtype[1, 2, 3, 5, 7], a[0.step(4)])
          assert_equal(dtype[2, 3, 5, 7, 11], a[-5.step(-1)])
          assert_equal(dtype[1, 3, 7], a[0.step(-1, 2)])
          assert_equal(dtype[1, 3, 7], a[0.step(4, 2)])
          assert_equal(dtype[2, 5, 11], a[-5.step(-1, 2)])

          assert_equal(dtype[1, 2, 3, 5, 7, 11], a[0.step])
          assert_equal(dtype[2, 3, 5, 7, 11], a[-5.step])
          assert_equal(dtype[1, 3, 7], a[(0..).step(2)])
          assert_equal(dtype[1, 3, 7], a[(0..).step(2)])
          assert_equal(dtype[1, 3, 7], a[(0...).step(2)])
          assert_equal(dtype[2, 5, 11], a[(-5..).step(2)])
          assert_equal(dtype[2, 5, 11], a[(-5...).step(2)])
          assert_equal(dtype[1, 3, 7], a[(0..) % 2])
          assert_equal(dtype[1, 3, 7], a[(0...) % 2])
          assert_equal(dtype[2, 5, 11], a[(-5..) % 2])
          assert_equal(dtype[2, 5, 11], a[(-5...) % 2])
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
        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          formatted_a = a.format
          assert_kind_of(Numo::RObject, formatted_a)
          assert_equal(Numo::RObject['1+0i', '2+0i', '3+0i', '5+0i', '7+0i', '11+0i'], formatted_a)
          formatted_a = a.format_to_a
          assert_kind_of(Array, formatted_a)
          assert_equal(['1+0i', '2+0i', '3+0i', '5+0i', '7+0i', '11+0i'], formatted_a)
          assert_match(/#{a.class}(\(view\))?#shape=\[6\]\n\[1\+0i, 2\+0i, 3\+0i, 5\+0i, 7\+0i, 11\+0i\]/, a.inspect)
        else
          formatted_a = a.format('%.1f')
          assert_kind_of(Numo::RObject, formatted_a)
          assert_equal(Numo::RObject['1.0', '2.0', '3.0', '5.0', '7.0', '11.0'], formatted_a)
          formatted_a = a.format_to_a('%.1f')
          assert_kind_of(Array, formatted_a)
          assert_equal(['1.0', '2.0', '3.0', '5.0', '7.0', '11.0'], formatted_a)
          assert_match(/#{a.class}(\(view\))?#shape=\[6\]\n\[1, 2, 3, 5, 7, 11\]/, a.inspect)
        end
        assert_equal(dtype[2, 3, 4, 6, 8, 12], a + 1)
        assert_equal(dtype[0, 1, 2, 4, 6, 10], a - 1)
        assert_equal(dtype[3, 6, 9, 15, 21, 33], a * 3)
        assert_equal(dtype[2, 4, 6, 10, 14, 22], a / 0.5)
        unless FLOAT_TYPES.include?(dtype)
          assert_raises(ZeroDivisionError) { a / 0 }
          assert_raises(ZeroDivisionError) { a / [0] }
          assert_raises(ZeroDivisionError) { a / [1, 1, 1, 1, 1, 0] }
        end
        assert_equal(dtype[1, 2, 0, 2, 1, 2], a % 3) unless [Numo::DComplex, Numo::SComplex].include?(dtype)
        unless FLOAT_TYPES.include?(dtype)
          assert_raises(ZeroDivisionError) { a % 0 }
          assert_raises(ZeroDivisionError) { a % [0] } # rubocop:disable Style/FormatString
          assert_raises(ZeroDivisionError) { a % [1, 1, 1, 1, 1, 0] } # rubocop:disable Style/FormatString
        end
        unless [Numo::DComplex, Numo::SComplex].include?(dtype)
          actual = a.divmod(2)
          assert_kind_of(Array, actual)
          assert_equal(2, actual.size)
          if FLOAT_TYPES.include?(dtype)
            assert_equal(dtype[0.5, 1, 1.5, 2.5, 3.5, 5.5], actual[0])
            assert_equal(dtype[1, 0, 1, 1, 1, 1], actual[1])
          else
            assert_equal(dtype[0, 1, 1, 2, 3, 5], actual[0])
            assert_equal(dtype[1, 0, 1, 1, 1, 1], actual[1])
            assert_raises(ZeroDivisionError) { a.divmod(0) }
          end
          actual = a.divmod([2])
          assert_kind_of(Array, actual)
          assert_equal(2, actual.size)
          if FLOAT_TYPES.include?(dtype)
            assert_equal(dtype[0.5, 1, 1.5, 2.5, 3.5, 5.5], actual[0])
            assert_equal(dtype[1, 0, 1, 1, 1, 1], actual[1])
          else
            assert_equal(dtype[0, 1, 1, 2, 3, 5], actual[0])
            assert_equal(dtype[1, 0, 1, 1, 1, 1], actual[1])
            assert_raises(ZeroDivisionError) { a.divmod([0]) }
          end
        end
        assert_equal(dtype[-1, -2, -3, -5, -7, -11], -a)
        assert_equal(dtype[-1, -2, -3, -5, -7, -11], a.-@)
        assert_equal(dtype[1, 4, 9, 25, 49, 121], a**2)
        assert_equal(dtype[1, 4, 9, 25, 49, 121], a.square)
        assert_equal(Numo::SFloat[1, 2, 3, 5, 7, 11], a.swap_byte.swap_byte) if dtype != Numo::RObject

        assert_predicate(a, :contiguous?)
        assert_predicate(a.transpose, :contiguous?)

        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          assert_equal(Numo::SFloat.asarray(src), a.real)
          assert_equal(Numo::SFloat[0, 0, 0, 0, 0, 0], a.imag)
          assert_equal(Numo::SFloat.asarray(src), a.conj)
          assert_equal(Numo::SFloat[0, 0, 0, 0, 0, 0], a.angle)
          b = dtype[1 + 2i, 3 + 4i, 5 + 6i, 7 + 8i, 9 + 10i, 11 + 12i]
          actual = b.conj
          assert_kind_of(dtype, actual)
          assert_equal(dtype[1 - 2i, 3 - 4i, 5 - 6i, 7 - 8i, 9 - 10i, 11 - 12i], actual)
          assert_equal(b, actual.conj)
        else
          assert_equal(dtype[2, 2, 3, 5, 7, 7], a.clip(2, 7))
          assert_equal(1, a.min)
          assert_equal(11, a.max)
          assert_equal(0, a.min_index)
          assert_equal(5, a.max_index)
          assert_equal(0, a.argmin)
          assert_equal(5, a.argmax)
          assert_equal([1, 11], a.minmax)
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
          assert_equal(39, a[0...4].mulsum(a[0...4]))
          assert_equal(32, a[0...4].mulsum(dtype[5, 4, 3, 2]))
        end
      end
    end
  end

  def test_abs
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[3.5, -2.1, 0.0, -0.7, -0.9].abs
      assert_kind_of(dtype, actual)
      assert_equal(dtype[3.5, 2.1, 0.0, 0.7, 0.9], actual)
    end
    INTEGER_TYPES.each do |dtype|
      actual = dtype[3, -2, 0, -7, -9].abs
      assert_kind_of(dtype, actual)
      assert_equal(dtype[3, 2, 0, 7, 9], actual)
    end
    UNSIGNED_INTEGER_TYPES.each do |dtype|
      actual = dtype[3, 0, 0, 7, 9].abs
      assert_kind_of(dtype, actual)
      assert_equal(dtype[3, 0, 0, 7, 9], actual)
    end
    actual = Numo::DComplex[3 + 4i, -8 - 6i, 0 + 0i, 0 - 4i, -5 + 12i].abs
    assert_kind_of(Numo::DFloat, actual)
    assert_equal(Numo::DFloat[5, 10, 0, 4, 13], actual)
    actual = Numo::SComplex[3 + 4i, -8 - 6i, 0 + 0i, 0 - 4i, -5 + 12i].abs
    assert_kind_of(Numo::SFloat, actual)
    assert_equal(Numo::SFloat[5, 10, 0, 4, 13], actual)
  end

  def test_reciprocal
    FLOAT_TYPES.each do |dtype|
      assert_equal(dtype[0.5, 0.25, 0.125, 0.1], dtype[2, 4, 8, 10].reciprocal)
    end
    INTEGER_TYPES.each do |dtype|
      assert_equal(dtype[1, -1, 0, 0], dtype[1, -1, 2, -2].reciprocal)
      assert_raises(ZeroDivisionError) { dtype[1, 0].reciprocal }
    end
    UNSIGNED_INTEGER_TYPES.each do |dtype|
      assert_equal(dtype[1, 0, 0], dtype[1, 2, 4].reciprocal)
      assert_raises(ZeroDivisionError) { dtype[1, 0].reciprocal }
    end
    Float.class_eval %{
      def reciprocal
        1.fdiv(self)
      end
    }, __FILE__, __LINE__ - 4
    assert_equal(Numo::RObject[0.5, 0.25, 0.125, 0.1], Numo::RObject[2.0, 4.0, 8.0, 10.0].reciprocal)
  end

  def test_sign
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      assert_equal(dtype[1, -1, 0, 1, -1], dtype[3.5, -2.1, 0.0, 0.7, -0.9].sign)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      assert_equal(dtype[1 + 1i, -1 - 1i, 0 + 0i, 0 - 1i, -1 + 0i],
                   dtype[3 + 2i, -2 - 3i, 0 + 0i, 0 - 4i, -5 + 0i].sign)
    end
    INTEGER_TYPES.each do |dtype|
      assert_equal(dtype[1, -1, 0, 1, -1], dtype[3, -2, 0, 7, -9].sign)
    end
    UNSIGNED_INTEGER_TYPES.each do |dtype|
      assert_equal(dtype[1, 0, 0, 1, 0], dtype[3, 0, 0, 7, 0].sign)
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

  def test_nearly_eq # rubocop:disable Metrics/AbcSize
    a = Numo::SFloat[1.0, 1.0, 1.0, 1.0]
    b = Numo::SFloat[1.0 + 1e-7, 1.0 - 1e-7, 1.0 + 1e-6, 1.0 - 1e-6]
    actual = a.nearly_eq(b)
    assert_kind_of(Numo::Bit, actual)
    assert_equal(Numo::Bit[1, 1, 0, 0], actual)
    a = Numo::DFloat[1.0, 1.0, 1.0, 1.0]
    b = Numo::DFloat[1.0 + 1e-16, 1.0 - 1e-16, 1.0 + 1e-15, 1.0 - 1e-15]
    actual = a.nearly_eq(b)
    assert_kind_of(Numo::Bit, actual)
    assert_equal(Numo::Bit[1, 1, 0, 0], actual)
    a = Numo::RObject[1.0, 1.0, 1.0, 1.0]
    b = Numo::RObject[1.0 + 1e-16, 1.0 - 1e-16, 1.0 + 1e-15, 1.0 - 1e-15]
    actual = a.nearly_eq(b)
    assert_kind_of(Numo::Bit, actual)
    assert_equal(Numo::Bit[1, 1, 0, 0], actual)
    a = Numo::SComplex[1.0 + 1.0i, 1.0 + 1.0i, 1.0 + 1.0i, 1.0 + 1.0i]
    b = Numo::SComplex[1.0 + 1.0i + (1e-7 + 1e-7i), 1.0 + 1.0i - (1e-7 + 1e-7i),
                       1.0 + 1.0i + (1e-6 + 1e-6i), 1.0 + 1.0i - (1e-6 + 1e-6i)]
    actual = a.nearly_eq(b)
    assert_kind_of(Numo::Bit, actual)
    assert_equal(Numo::Bit[1, 1, 0, 0], actual)
    a = Numo::DComplex[1.0 + 1.0i, 1.0 + 1.0i, 1.0 + 1.0i, 1.0 + 1.0i]
    b = Numo::DComplex[1.0 + 1.0i + (1e-16 + 1e-16i), 1.0 + 1.0i - (1e-16 + 1e-16i),
                       1.0 + 1.0i + (1e-15 + 1e-15i), 1.0 + 1.0i - (1e-15 + 1e-15i)]
    actual = a.nearly_eq(b)
    assert_kind_of(Numo::Bit, actual)
    assert_equal(Numo::Bit[1, 1, 0, 0], actual)
  end

  def test_floor
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.5, -2.3, 3.0, -4.7, 5.9].floor
      assert_equal(dtype[1.0, -3.0, 3.0, -5.0, 5.0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.5 + 2.3i, -2.3 - 3.7i, 3.0 + 4.0i].floor
      assert_equal(dtype[1.0 + 2.0i, -3.0 - 4.0i, 3.0 + 4.0i], actual)
    end
  end

  def test_bit_operations # rubocop:disable Metrics/AbcSize, Minitest/MultipleAssertions
    INTEGER_TYPES.each do |dtype|
      x = dtype.new(5).seq
      y = dtype[0, 1, 2, 0, 1]
      assert_equal(dtype[0, 1, 2, 0, 0], x & y)
      assert_equal(dtype[0, 1, 2, 3, 5], x | y)
      assert_equal(dtype[0, 0, 0, 3, 5], x ^ y)
      y = Numo::UInt32[0, 1, 2, 0, 1]
      assert_equal(dtype[0, 1, 2, 0, 0], x & y)
      assert_equal(dtype[0, 1, 2, 3, 5], x | y)
      assert_equal(dtype[0, 0, 0, 3, 5], x ^ y)
      assert_equal(-1 - x, ~x)
      assert_equal(dtype[0, 2, 4, 6, 8], x << 1)
      assert_equal(dtype[0, 2, 4, 6, 8], (x << 2) >> 1)
    end
    UNSIGNED_INTEGER_TYPES.each do |dtype|
      x = dtype.new(5).seq
      y = dtype[0, 1, 2, 0, 1]
      assert_equal(dtype[0, 1, 2, 0, 0], x & y)
      assert_equal(dtype[0, 1, 2, 3, 5], x | y)
      assert_equal(dtype[0, 0, 0, 3, 5], x ^ y)
      y = Numo::Int32[0, 1, 2, 0, 1]
      assert_equal(dtype[0, 1, 2, 0, 0], x & y)
      assert_equal(dtype[0, 1, 2, 3, 5], x | y)
      assert_equal(dtype[0, 0, 0, 3, 5], x ^ y)
      assert_equal(dtype::MAX - x, ~x)
      assert_equal(dtype[0, 2, 4, 6, 8], x << 1)
      assert_equal(dtype[0, 2, 4, 6, 8], (x << 2) >> 1)
    end
    Integer.class_eval %{
      def bit_and(other)
        self & other
      end

      def bit_or(other)
        self | other
      end

      def bit_xor(other)
        self ^ other
      end

      def bit_not
        ~self
      end
    }, __FILE__, __LINE__ - 16
    x = Numo::RObject[0, 1, 2]
    assert_equal(Numo::RObject[0, 0, 2], x & 2)
    assert_equal(Numo::RObject[2, 3, 2], x | 2)
    assert_equal(Numo::RObject[2, 3, 0], x ^ 2)
    assert_equal(Numo::RObject[-1, -2, -3], ~x)
    assert_equal(Numo::RObject[0, 2, 4], x << 1)
    assert_equal(Numo::RObject[0, 2, 4], (x << 2) >> 1)
  end

  def test_round
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.5, -2.3, 3.0, -4.7, 5.9].round
      assert_equal(dtype[2.0, -2.0, 3.0, -5.0, 6.0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.5 + 2.3i, -2.3 - 3.7i, 3.0 + 4.0i].round
      assert_equal(dtype[2.0 + 2.0i, -2.0 - 4.0i, 3.0 + 4.0i], actual)
    end
  end

  def test_ceil
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.5, -2.3, 3.0, -4.7, 5.9].ceil
      assert_equal(dtype[2.0, -2.0, 3.0, -4.0, 6.0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.5 + 2.3i, -2.3 - 3.7i, 3.0 + 4.0i].ceil
      assert_equal(dtype[2.0 + 3.0i, -2.0 - 3.0i, 3.0 + 4.0i], actual)
    end
  end

  def test_trunc
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.5, -2.3, 3.0, -4.7, 5.9].trunc
      assert_equal(dtype[1.0, -2.0, 3.0, -4.0, 5.0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.5 + 2.3i, -2.3 - 3.7i, 3.0 + 4.0i].trunc
      assert_equal(dtype[1.0 + 2.0i, -2.0 - 3.0i, 3.0 + 4.0i], actual)
    end
  end

  def test_rint
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      actual = dtype[1.5, -2.5, 3.0, -4.5, 5.9].rint
      assert_equal(dtype[2.0, -2.0, 3.0, -4.0, 6.0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.5 + 2.5i, -2.5 - 3.5i, 3.0 + 4.0i].rint
      assert_equal(dtype[2.0 + 2.0i, -2.0 - 4.0i, 3.0 + 4.0i], actual)
    end
  end

  def test_copysign
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      actual = dtype[1, -2, 3, -4, 5].copysign(dtype[-1, 1, -1, 1, 0])
      assert_kind_of(dtype, actual)
      assert_equal(dtype[-1, 2, -3, 4, 5], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1 + 2i, -2 - 3i, 3 + 4i].copysign(dtype[-1 + 1i, 1 - 1i, -1 + 0i])
      assert_kind_of(dtype, actual)
      assert_equal(dtype[-1 + 2i, 2 - 3i, -3 + 4i], actual)
    end
  end

  def test_signbit
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      actual = dtype[1.0, -2.0, 0.0, -0.0, 3.5].signbit
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 0, 1, 0], actual)
    end
  end

  def test_modf
    # rubocop:disable Performance/CollectionLiteralInLoop
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      actual = dtype[1.5, -2.1, 3.0, -4.7, 5.9].modf
      assert_kind_of(Array, actual)
      assert_equal(2, actual.size)
      assert_kind_of(dtype, actual[0])
      [0.5, -0.1, 0, -0.7, 0.9].each_with_index do |expected, i|
        assert_in_delta(expected, actual[0][i], 1e-6)
      end
      assert_kind_of(dtype, actual[1])
      assert_equal(dtype[1, -2, 3, -4, 5], actual[1])
    end
    # rubocop:enable Performance/CollectionLiteralInLoop
  end

  def test_kahan_sum
    a = Numo::DFloat.new(1_000_000).fill(1e-6)
    assert_in_delta(1, a.kahan_sum, 1e-10)
    a = Numo::DComplex.new(1_000_000).fill(1e-6 + 1e-6i)
    assert_in_delta(1 + 1i, a.kahan_sum, 1e-10)
  end

  def test_seq
    TYPES.each do |dtype|
      assert_equal(dtype[0, 1, 2, 3, 4], dtype.new(5).seq)
      assert_equal(dtype[2, 3, 4, 5, 6], dtype.new(5).seq(2))
      assert_equal(dtype[5, 8, 11, 14, 17], dtype.new(5).seq(5, 3))
    end
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      assert_equal(dtype[1, 10, 100, 1000], dtype.new(4).logseq(0, 1))
      assert_equal(dtype[16, 8, 4, 2, 1], dtype.new(5).logseq(4, -1, 2))
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype.new(3).logseq(2, 1, 1 + 2i)
      assert_equal(3, actual.size)
      [-3 + 4i, -11 - 2i, -7 - 24i].each_with_index do |expected, i|
        assert_in_delta(expected, actual[i], 1e-5)
      end
    end
  end

  def test_rand # rubocop:disable Metrics/AbcSize, Metrics/CyclomaticComplexity, Metrics/MethodLength, Minitest/MultipleAssertions, Metrics/PerceivedComplexity
    INTEGER_TYPES.each do |dtype|
      Numo::NArray.srand(1_234_567_890)
      if dtype.byte_size < 8
        assert_equal(dtype[3, 4, 1, 4, 0, -2, -2, 2, -2, -3], dtype.new(10).rand(-3, 5))
        assert_equal(dtype[[6, 5, 8, 2, 4], [1, 0, 4, 9, 2]], dtype.new(2, 5).rand(10))
      else
        assert_equal(dtype[3, 1, 4, -2, 2, -2, -3, 2, 1, -1], dtype.new(10).rand(-3, 5))
        assert_equal(dtype[[3, 6, 1, 5, 7], [9, 9, 6, 4, 0]], dtype.new(2, 5).rand(10))
      end
    end
    UNSIGNED_INTEGER_TYPES.each do |dtype|
      Numo::NArray.srand(1_234_567_890)
      if dtype.byte_size < 8
        assert_equal(dtype[6, 7, 4, 7, 3, 1, 1, 5, 1, 0], dtype.new(10).rand(10))
        assert_equal(dtype[[6, 5, 8, 2, 4], [1, 0, 4, 9, 2]], dtype.new(2, 5).rand(10))
      else
        assert_equal(dtype[6, 4, 7, 1, 5, 1, 0, 5, 8, 4], dtype.new(10).rand(10))
        assert_equal(dtype[[2, 3, 6, 1, 5], [7, 9, 9, 6, 4]], dtype.new(2, 5).rand(10))
      end
    end
    # rubocop:disable Performance/CollectionLiteralInLoop
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      Numo::NArray.srand(1_234_567_890)
      actual1d = dtype.new(5).rand(-1, 1)
      actual2d = dtype.new(2, 3).rand(-1, 1)
      assert_equal(2, actual2d.ndim)
      assert_equal(2, actual2d.shape[0])
      assert_equal(3, actual2d.shape[1])
      if dtype == Numo::SFloat
        [-0.128008, -0.0780624, -0.470543, 0.808037, -0.00503415].each_with_index do |expected, i|
          assert_in_delta(expected, actual1d[i], 1e-6)
        end
        [0.279245, 0.838221, -0.621664].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[0, i], 1e-6)
        end
        [-0.860478, 0.933182, 0.592224].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[1, i], 1e-6)
        end
      elsif dtype == Numo::DFloat
        [-0.0780623, 0.808037, 0.279245, -0.621664, 0.933182].each_with_index do |expected, i|
          assert_in_delta(expected, actual1d[i], 1e-6)
        end
        [-0.87189, 0.359531, 0.258884].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[0, i], 1e-6)
        end
        [-0.248656, 0.766334, -0.625959].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[1, i], 1e-6)
        end
      elsif dtype == Numo::RObject
        [-0.078062, 0.808036, 0.279245, -0.621664, 0.933181].each_with_index do |expected, i|
          assert_in_delta(expected, actual1d[i], 1e-6)
        end
        [-0.871889, 0.359530, 0.258884].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[0, i], 1e-6)
        end
        [-0.248655, 0.766333, -0.625958].each_with_index do |expected, i|
          assert_in_delta(expected, actual2d[1, i], 1e-6)
        end
      end
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      Numo::NArray.srand(1_234_567_890)
      actual1d = dtype.new(5).rand(2 + 3i)
      actual2d = dtype.new(2, 3).rand(-2 - 3i, 1 + 2i)
      assert_equal(2, actual2d.ndim)
      assert_equal(2, actual2d.shape[0])
      assert_equal(3, actual2d.shape[1])
      if dtype == Numo::SComplex
        [0.871991 + 1.382906i, 0.529456 + 2.712055i, 0.994965 + 1.918868i, 1.838220 + 0.567503i,
         0.139521 + 2.899772i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual1d[i].real, 1e-6)
          assert_in_delta(expected.imag, actual1d[i].imag, 1e-6)
        end
        [0.388335 - 2.679724i, -1.060235 + 0.398827i, -1.702621 + 0.147211i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual2d[0, i].real, 1e-6)
          assert_in_delta(expected.imag, actual2d[0, i].imag, 1e-6)
        end
        [-1.871673 - 1.121639i, -0.888626 + 1.415834i, -0.451813 - 2.064896i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual2d[1, i].real, 1e-6)
          assert_in_delta(expected.imag, actual2d[1, i].imag, 1e-6)
        end
      elsif dtype == Numo::DComplex
        [0.921937 + 2.712055i, 1.279245 + 0.567503i, 1.933181 + 0.192165i, 1.359530 + 1.888326i,
         0.751344 + 2.649500i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual1d[i].real, 1e-6)
          assert_in_delta(expected.imag, actual1d[i].imag, 1e-6)
        end
        [-1.438938 - 1.474032i, -1.687581 + 0.280124i, -1.992894 - 0.0294957i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual2d[0, i].real, 1e-6)
          assert_in_delta(expected.imag, actual2d[0, i].imag, 1e-6)
        end
        [0.795879 - 1.566802i, 0.835083 - 1.379133i, 0.245460 + 1.054923i].each_with_index do |expected, i|
          assert_in_delta(expected.real, actual2d[1, i].real, 1e-6)
          assert_in_delta(expected.imag, actual2d[1, i].imag, 1e-6)
        end
      end
      actual = Numo::SComplex.new(5).rand
      assert(actual.real.to_a.any? { |v| v != 0.0 })
      assert(actual.imag.to_a.any? { |v| v != 0.0 })
      actual = Numo::DComplex.new(5).rand
      assert(actual.real.to_a.any? { |v| v != 0.0 })
      assert(actual.imag.to_a.any? { |v| v != 0.0 })
    end
    # rubocop:enable Performance/CollectionLiteralInLoop
  end

  def test_rand_norm
    [Numo::DFloat, Numo::SFloat].each do |dtype|
      Numo::NArray.srand(1_234_567)
      actual = dtype.new(100_000).rand_norm
      assert_in_delta(0.0, actual.mean, 0.01)
      assert_in_delta(1.0, actual.var, 0.01)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      Numo::NArray.srand(1_234_567)
      actual = dtype.new(100_000).rand_norm
      assert_in_delta(0.0, actual.mean.real, 0.01)
      assert_in_delta(0.0, actual.mean.imag, 0.01)
      assert_in_delta(1.0, actual.real.var, 0.01)
      assert_in_delta(1.0, actual.imag.var, 0.01)
    end
  end

  def test_isnan
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.0, Float::NAN, 2.0, Float::NAN, 3.0].isnan
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 0, 1, 0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.0 + 2.0i, Float::NAN - 2.0i, 4.0 + (Float::NAN * 1i), Float::NAN + (Float::NAN * 1i), 3.0 + 2.0i].isnan
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 1, 1, 0], actual)
    end
  end

  def test_isinf
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.0, Float::INFINITY, -Float::INFINITY, 2.0, Float::NAN].isinf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 1, 0, 0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.0 + 2.0i, Float::INFINITY - 2.0i, 4.0 - (Float::INFINITY * 1i), Float::NAN + (Float::NAN * 1i),
                     3.0 + 2.0i].isinf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 1, 0, 0], actual)
    end
  end

  def test_isposinf
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.0, Float::INFINITY, -Float::INFINITY, 2.0, Float::NAN].isposinf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 0, 0, 0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.0 + 2.0i, Float::INFINITY - 2.0i, 4.0 - (Float::INFINITY * 1i), Float::NAN + (Float::NAN * 1i),
                     3.0 + 2.0i].isposinf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 1, 0, 0, 0], actual)
    end
  end

  def test_isneginf
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.0, Float::INFINITY, -Float::INFINITY, 2.0, Float::NAN].isneginf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 0, 1, 0, 0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.0 + 2.0i, Float::INFINITY - 2.0i, 4.0 - (Float::INFINITY * 1i), Float::NAN + (Float::NAN * 1i),
                     3.0 + 2.0i].isneginf
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[0, 0, 1, 0, 0], actual)
    end
  end

  def test_isfinite
    [Numo::DFloat, Numo::SFloat, Numo::RObject].each do |dtype|
      actual = dtype[1.0, Float::INFINITY, -Float::INFINITY, 2.0, Float::NAN].isfinite
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[1, 0, 0, 1, 0], actual)
    end
    [Numo::DComplex, Numo::SComplex].each do |dtype|
      actual = dtype[1.0 + 2.0i, Float::INFINITY - 2.0i, 4.0 - (Float::INFINITY * 1i), Float::NAN + (Float::NAN * 1i),
                     3.0 + 2.0i].isfinite
      assert_kind_of(Numo::Bit, actual)
      assert_equal(Numo::Bit[1, 0, 0, 0, 1], actual)
    end
  end

  def test_bincount
    [*INTEGER_TYPES, *UNSIGNED_INTEGER_TYPES].each do |dtype|
      actual = dtype[0, 1, 1, 3, 2, 1, 7].bincount
      assert_kind_of(Numo::UInt32, actual)
      assert_equal(Numo::UInt32[1, 3, 1, 1, 0, 0, 0, 1], actual)
      x = dtype[0, 1, 1, 3, 2, 1, 7, 23]
      assert_equal(x.max + 1, x.bincount.size)
      w = Numo::DFloat[0.3, 0.5, 0.2, 0.7, 1.0, -0.6]
      x = dtype[0, 1, 1, 2, 2, 2]
      actual = x.bincount(w)
      assert_kind_of(Numo::DFloat, actual)
      assert_equal(Numo::DFloat[0.3, 0.7, 1.1], actual)
      w = Numo::SFloat[0.3, 0.5, 0.2, 0.7, 1.0, -0.6]
      actual = x.bincount(w)
      assert_kind_of(Numo::SFloat, actual)
      assert_equal(Numo::SFloat[0.3, 0.7, 1.1], actual)
    end
  end

  def test_2d_narray # rubocop:disable Metrics/AbcSize, Metrics/MethodLength, Metrics/PerceivedComplexity, Minitest/MultipleAssertions
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
        assert_equal(Numo::Bit[[0, 1, 0], [1, 0, 1]], a.ne([[1, 1, 3], [3, 7, 7]]))
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
        sub_a = a[true, 0...2]
        assert_equal(dtype[[1, 2], [10, 70]], sub_a.cumprod)
        assert_equal(dtype[[1, 2], [5, 14]], sub_a.cumprod(0))
        assert_equal(dtype[[1, 2], [5, 35]], sub_a.cumprod(1))
        assert_equal(79, sub_a.mulsum(sub_a))
        assert_equal(dtype[26, 53], sub_a.mulsum(sub_a, 0))
        assert_equal(dtype[5, 74], sub_a.mulsum(sub_a, 1))
        b = dtype[[3, 1, 2], [2, 1, 0]]
        assert_equal(28, a.mulsum(b))
        assert_equal(dtype[13, 9, 6], a.mulsum(b, 0))
        assert_equal(dtype[11, 17], a.mulsum(b, 1))

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
          assert_equal(dtype[[2, 2, 3], [5, 7, 7]], a.clip(2, 7))
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
          assert_equal(5, a.max_index)
          assert_equal(dtype[2, 5], a.max_index(axis: 1))
          assert_equal(dtype[3, 4, 5], a.max_index(axis: 0))
          assert_equal(0, a.argmin)
          assert_equal(Numo::Int32[0, 0], a.argmin(axis: 1))
          assert_equal(Numo::Int32[0, 0, 0], a.argmin(axis: 0))
          assert_equal(5, a.argmax)
          assert_equal(Numo::Int32[2, 2], a.argmax(axis: 1))
          assert_equal(Numo::Int32[1, 1, 1], a.argmax(axis: 0))
          assert_equal([1, 11], a.minmax)
          assert_equal(dtype[[1, 5], [3, 11]], a.minmax(axis: 1))
          assert_equal(dtype[[1, 2, 3], [5, 7, 11]], a.minmax(axis: 0))
          assert_equal(Numo::Bit[[0, 0, 1], [1, 1, 1]], a >= 3)
          assert_equal(Numo::Bit[[0, 0, 0], [1, 1, 1]], a > 3)
          assert_equal(Numo::Bit[[1, 1, 1], [0, 0, 0]], a <= 3)
          assert_equal(Numo::Bit[[1, 1, 0], [0, 0, 0]], a < 3)
          assert_equal(Numo::Bit[[0, 0, 1], [0, 0, 0]], a.eq(3))
          assert_equal(Numo::Bit[[1, 1, 0], [1, 1, 1]], a.ne(3))
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
        if [Numo::DComplex, Numo::SComplex].include?(dtype)
          formatted_a = a.format
          assert_kind_of(Numo::RObject, formatted_a)
          assert_equal(Numo::RObject[['1+0i', '2+0i', '3+0i'],
                                     ['5+0i', '7+0i', '11+0i']], formatted_a)
          formatted_a = a.format_to_a
          assert_kind_of(Array, formatted_a)
          assert_equal([['1+0i', '2+0i', '3+0i'], ['5+0i', '7+0i', '11+0i']], formatted_a)
          assert_match(/#{a.class}(\(view\))?#shape=\[2,3\]\n\[\[1\+0i, 2\+0i, 3\+0i\], \n \[5\+0i, 7\+0i, 11\+0i\]\]/,
                       a.inspect)
        else
          formatted_a = a.format('%.1f')
          assert_kind_of(Numo::RObject, formatted_a)
          assert_equal(Numo::RObject[['1.0', '2.0', '3.0'], ['5.0', '7.0', '11.0']], formatted_a)
          formatted_a = a.format_to_a('%.1f')
          assert_kind_of(Array, formatted_a)
          assert_equal([['1.0', '2.0', '3.0'], ['5.0', '7.0', '11.0']], formatted_a)
          assert_match(/#{a.class}(\(view\))?#shape=\[2,3\]\n\[\[1, 2, 3\], \n \[5, 7, 11\]\]/, a.inspect)
        end
        assert_equal(dtype[[2, 3, 4], [6, 8, 12]], a + 1)
        assert_equal(dtype[[2, 4, 6], [6, 9, 14]], a + [1, 2, 3])
        assert_equal(dtype[[0, 1, 2], [4, 6, 10]], a - 1)
        assert_equal(dtype[[0, 0, 0], [4, 5, 8]], a - [1, 2, 3])
        assert_equal(dtype[[3, 6, 9], [15, 21, 33]], a * 3)
        assert_equal(dtype[[1, 4, 9], [5, 14, 33]], a * [1, 2, 3])
        assert_equal(dtype[[2, 4, 6], [10, 14, 22]], a / 0.5)
        unless FLOAT_TYPES.include?(dtype)
          assert_raises(ZeroDivisionError) { a / 0 }
          assert_raises(ZeroDivisionError) { a / [0] }
          assert_raises(ZeroDivisionError) { a / dtype[[1, 1, 1], [1, 1, 0]] }
        end
        unless [Numo::DComplex, Numo::SComplex].include?(dtype)
          assert_equal(dtype[[1, 2, 0], [2, 1, 2]], a % 3)
          assert_equal(dtype[[1, 0, 3], [2, 1, 1]], a % [3, 2, 5]) # rubocop:disable Style/FormatString
        end
        unless FLOAT_TYPES.include?(dtype)
          assert_raises(ZeroDivisionError) { a % 0 }
          assert_raises(ZeroDivisionError) { a % [0] } # rubocop:disable Style/FormatString
          assert_raises(ZeroDivisionError) { a % dtype[[1, 1, 1], [1, 1, 0]] }
        end
        assert_equal(dtype[[-1, -2, -3], [-5, -7, -11]], -a)
        assert_equal(dtype[[1, 4, 9], [25, 49, 121]], a**2)
        assert_equal(dtype[[1, 4, 9], [25, 49, 121]], a.square)
      end
    end
  end

  def test_eye
    TYPES.each do |dtype|
      assert_equal(dtype[[1, 0, 0], [0, 1, 0], [0, 0, 1]], dtype.new(3, 3).eye)
      assert_equal(dtype[[1, 0], [0, 1], [0, 0]], dtype.new(3, 2).eye)
      assert_equal(dtype[[1, 0, 0], [0, 1, 0]], dtype.new(2, 3).eye)
      assert_equal(dtype[[2, 0, 0], [0, 2, 0], [0, 0, 2]], dtype.new(3, 3).eye(2))
      assert_equal(dtype[[0, 1, 0], [0, 0, 1], [0, 0, 0]], dtype.new(3, 3).eye(1, 1))
      assert_equal(dtype[[0, 0, 1], [0, 0, 0], [0, 0, 0]], dtype.new(3, 3).eye(1, 2))
      assert_equal(dtype[[0, 0, 0], [1, 0, 0], [0, 1, 0]], dtype.new(3, 3).eye(1, -1))
      assert_equal(dtype[[0, 0, 0], [0, 0, 0], [1, 0, 0]], dtype.new(3, 3).eye(1, -2))
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
      a = dtype.new(*shape).rand(0, 10)
      original_data = a.to_binary
      data = original_data.dup.freeze
      restored_a = dtype.from_binary(data, shape)
      assert_equal(a, restored_a)

      restored_a[0, 0] += 1
      refute_equal(restored_a, a)
      assert_equal(original_data, data)

      # test 'not frozen string' do
      shape = [2, 5]
      a = dtype.new(*shape).rand(0, 10)
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
      a = dtype.new(*shape).rand(0, 10)
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
      a = dtype.new(*shape).rand(0, 10)
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
