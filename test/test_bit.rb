# frozen_string_literal: true

require_relative 'test_helper'

class NArrayBitTest < NArrayTestBase
  def test_numo_bit # rubocop:disable Metrics/AbcSize, Metrics/MethodLength, Minitest/MultipleAssertions
    assert_operator(Numo::Bit, :<, Numo::NArray)

    [[proc { |tp, a| tp[*a] }, ''],
     [proc { |tp, a| tp[*a][true] }, '[true]'],
     [proc { |tp, a| tp[*a] }, '[0..-1]']].each do |init, _ref|
      # test "Numo::Bit,[0,1,1,0,1,0,0,1]#{ref}" do
      src = [0, 1, 1, 0, 1, 0, 0, 1]
      a = init.call(Numo::Bit, src)
      assert_equal(Numo::Bit.asarray(src), a)
      assert_equal(Numo::Bit[0, 0, 0, 0, 0, 0, 0, 0], a & 0)
      assert_equal(Numo::Bit.asarray(src), a & 1)
      assert_equal(Numo::Bit.asarray(src), a | 0)
      assert_equal(Numo::Bit[1, 1, 1, 1, 1, 1, 1, 1], a | 1)
      assert_equal(Numo::Bit.asarray(src.map { |x| x ^ 0 }), a ^ 0)
      assert_equal(Numo::Bit.asarray(src.map { |x| x ^ 1 }), a ^ 1)
      assert_equal(Numo::Bit.asarray(src.map { |x| 1 - x }), ~a)
      assert_equal(4, a.count_true)
      assert_equal(4, a.count_false)
      assert_equal(Numo::Int32[1, 2, 4, 7], a.where)
      assert_equal(Numo::Int32[[1, 2, 4, 7], [0, 3, 5, 6]], a.where2)
      assert_equal(Numo::Int32[2, 3, 5, 8], a.mask(Numo::DFloat[1, 2, 3, 4, 5, 6, 7, 8]))
      refute_predicate(a, :all?)
      assert_predicate(a, :any?)
      assert_predicate(a, :any?)
    end

    [[proc { |tp, a| tp[*a] }, ''],
     [proc { |tp, a| tp[*a][true, 0..-1] }, '[true,true]']].each do |init, _ref|
      # test "Numo::Bit,[[0,1,1,0],[1,0,0,1]]#{ref}" do
      src = [[0, 1, 1, 0], [1, 0, 0, 1]]
      src.size
      a = init.call(Numo::Bit, src)
      assert_equal(0, a[5])
      assert_equal(1, a[-1])
      assert_equal(src[1][0], a[1, 0])
      assert_equal(src[1][1], a[1, 1])
      assert_equal(src[1][2], a[1, 2])
      assert_equal(Numo::Bit[0, 1], a[3..4])
      assert_equal(Numo::Bit[1, 1], a[0, 1..2])
      assert_equal(Numo::Bit.asarray(src[0]), a[0, :*])
      assert_equal(Numo::Bit.asarray(src[1]), a[1, :*])
      assert_equal(Numo::Bit[src[0][1], src[1][1]], a[:*, 1])
      assert_equal(4, a.count_true)
      assert_equal(4, a.count_false)
      assert_equal(Numo::Int32[1, 2, 4, 7], a.where)
      assert_equal(Numo::Int32[[1, 2, 4, 7], [0, 3, 5, 6]], a.where2)
      assert_equal(Numo::DFloat[2, 3, 5, 8], a.mask(Numo::DFloat[[1, 2, 3, 4], [5, 6, 7, 8]]))
      refute_predicate(a, :all?)
      assert_predicate(a, :any?)
      refute_predicate(a, :none?)
    end

    [[proc { |tp, a| tp[*a] }, '']].each do |init, _ref|
      # test "Numo::Bit,[]#{ref}" do
      src = []
      a = init.call(Numo::Bit, src)
      assert_equal(Numo::Bit.asarray(src), a)
      assert_equal(Numo::Bit[], a & 0)
      assert_equal(Numo::Bit.asarray(src), a & 1)
      assert_equal(Numo::Bit.asarray(src), a | 0)
      assert_equal(Numo::Bit[], a | 1)
      assert_equal(Numo::Bit.asarray(src.map { |x| x ^ 0 }), a ^ 0)
      assert_equal(Numo::Bit.asarray(src.map { |x| x ^ 1 }), a ^ 1)
      assert_equal(Numo::Bit.asarray(src.map { |x| 1 - x }), ~a)
      assert_equal(0, a.count_true)
      assert_equal(0, a.count_false)
      assert_empty(a.where)
      assert_equal([Numo::Int32[], Numo::Int32[]], a.where2)
      assert_empty(a.mask(Numo::DFloat[]))
      refute_predicate(a, :all?)
      refute_predicate(a, :any?)
      assert_predicate(a, :none?)
    end
  end

  def test_store_to_view
    n = 14
    x = Numo::Bit.zeros(n + 2, n + 2, 3)
    ~x[1..-2, 1..-2, 0].inplace # rubocop:disable Lint/Void
    assert_equal(n * n, x.where.size)

    x1 = Numo::Bit.ones(n, n)
    x0 = Numo::Bit.zeros(n, n)
    y0 = Numo::Bit.zeros(n + 2, n + 2)
    x = Numo::NArray.dstack([x1, x0, x0])
    y = Numo::NArray.dstack([y0, y0, y0])
    y[1..-2, 1..-2, true] = x
    assert_empty((~y[1..-2, 1..-2, 0]).where)
    assert_empty(y[true, true, 1].where)
  end

  def test_assign_nil
    x = Numo::RObject.cast([1, 2, 3])
    x[Numo::Bit.cast([0, 1, 0])] = nil
    assert_equal([1, nil, 3], x.to_a)
  end

  def test_flipud
    m = Numo::Bit.zeros(7, 7)
    m = m.flipud
    m[true, 1] = 1
    assert_equal(
      Numo::Bit[
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0]
      ],
      m
    )
  end
end
