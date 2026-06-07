#!/usr/bin/env ruby
require 'bundler/setup'
require 'numo/narray/alt'

if ARGV.include?('--debug')
  # Shorter test with debugging output
  Numo::NArray.debug = true
  classes = [Numo::SFloat, Numo::DFloat, Numo::SComplex]
else
  # Full test
  names = Numo.constants - [:NMath, :Struct, :RObject, :NArray, :Bit]
  classes = names.sort.map { |n| Numo.const_get(n) }.uniq
end

classes.each do |klass|
  puts "\n\n\e[1m#{klass}:\e[0m"
  count = 0
  result = klass.zeros(5).map { |_| STDOUT.write(" #{count}") ; count += 1 }
  puts "\n  Block called #{count} times: #{result.inspect.sub("\n", ": ")} -- #{count == 5 ? "\e[32mPASS\e[0m" : "\e[31mFAIL\e[0m"}"
end
