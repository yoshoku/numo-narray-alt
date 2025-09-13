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
  end
end
