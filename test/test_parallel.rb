# frozen_string_literal: true

require_relative 'test_helper'

class ParallelTest < NArrayTestBase
  def setup
    @original_threads = ENV.fetch('NUMO_NARRAY_NUM_THREADS', nil)
  end

  def teardown
    if @original_threads
      ENV['NUMO_NARRAY_NUM_THREADS'] = @original_threads
    else
      ENV.delete('NUMO_NARRAY_NUM_THREADS')
    end
  end

  def test_thread_count_consistency
    n = 100_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    results = {}
    [1, 2, 4, 8].each do |threads|
      ENV['NUMO_NARRAY_NUM_THREADS'] = threads.to_s
      results[threads] = a + b
    end

    baseline = results[1]
    [2, 4, 8].each do |threads|
      assert_equal baseline, results[threads], "Thread count #{threads} produced different result"
    end
  end

  def test_sfloat_thread_consistency
    n = 100_000
    a = Numo::SFloat.new(n).rand
    b = Numo::SFloat.new(n).rand

    results = {}
    [1, 2, 4].each do |threads|
      ENV['NUMO_NARRAY_NUM_THREADS'] = threads.to_s
      results[threads] = a + b
    end

    baseline = results[1]
    [2, 4].each do |threads|
      assert_equal baseline, results[threads], "SFloat thread count #{threads} produced different result"
    end
  end

  def test_environment_variable
    ENV['NUMO_NARRAY_NUM_THREADS'] = '2'
    n = 50_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    result1 = a + b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result2 = a + b

    assert_equal result1, result2, 'Different thread counts should produce same result'
  end

  def test_large_array_parallel
    n = 1_000_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    result_single = a + b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result_parallel = a + b

    assert_equal result_single, result_parallel, 'Parallel execution should produce same result as single thread'
  end

  def test_inplace_operation
    n = 100_000
    a1 = Numo::DFloat.new(n).rand
    a2 = a1.dup
    b = Numo::DFloat.new(n).rand

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    a1 += b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    a2 += b

    assert_equal a1, a2, 'Inplace operation should produce same result with different thread counts'
  end

  def test_subtraction_parallel
    n = 100_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    result_single = a - b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result_parallel = a - b

    assert_equal result_single, result_parallel, 'Subtraction should produce same result with different thread counts'
  end

  def test_multiplication_parallel
    n = 100_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    result_single = a * b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result_parallel = a * b

    assert_equal result_single, result_parallel, 'Multiplication should produce same result with different thread counts'
  end

  def test_division_parallel
    n = 100_000
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand
    b += 0.1

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    result_single = a / b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result_parallel = a / b

    assert_equal result_single, result_parallel, 'Division should produce same result with different thread counts'
  end

  def test_small_array_below_threshold
    n = 100
    a = Numo::DFloat.new(n).rand
    b = Numo::DFloat.new(n).rand

    ENV['NUMO_NARRAY_NUM_THREADS'] = '1'
    result_single = a + b

    ENV['NUMO_NARRAY_NUM_THREADS'] = '4'
    result_parallel = a + b

    assert_equal result_single, result_parallel, 'Small arrays should produce same result regardless of thread count'
  end
end
