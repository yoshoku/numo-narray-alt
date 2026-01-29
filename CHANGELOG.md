## [[0.9.14](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.13...v0.9.14)] - 2026-01-29

- fixed the `poly` method that intermittently failed due to unsigned integer underflow and uninitialized dim field: [c4755ac](https://github.com/yoshoku/numo-narray-alt/commit/c4755acabc6621f5e3302ebe1f535dceb0496938).
- refactored multiple methods to use macro templates: abs, copysign, signbit, modf, kahan_sum, conj, im, real, imag, arg, set_imag, set_real, each, map, each_with_index, map_with_index, extract, aref, and poly.

## [[0.9.13](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.12...v0.9.13)] - 2026-01-23

- fixed the `rand` method for `Numo::DComplex` to return the complex values when no arguments are given: [78423e5](https://github.com/yoshoku/numo-narray-alt/commit/78423e5e0c300374ea87affa1e5433d821862089)
- refactored multiple methods to use macro templates: pow, minus, reciprocal, sign, square, and rand_norm.

## [[0.9.12](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.11...v0.9.12)] - 2026-01-21

- fixed the `divmod` method for `Numo::RObject` to also return the modulo result: [687b3bf](https://github.com/yoshoku/numo-narray-alt/commit/687b3bf5588c46212b71d75d8a17487b4de7122e)
- refactored multiple methods to use macro templates: isnan, isinf, isposinf, isneginf, isfinite, floor, round,
  ceil, trunc, rint, gt, ge, lt, le, eq, ne, nearly_eq, to_a, coerce_cast, fill, format, format_to_a, inspect,
  add, sub, mul, div, mod, bincount, &, |, ^, ~, <<, >>, and divmod.

## [[0.9.11](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.10...v0.9.11)] - 2026-01-02

- fixed maximum value specification not working in `rand` method of `Numo::RObject`: [ea94492](https://github.com/yoshoku/numo-narray-alt/compare/9e9e4ea...ea94492)
- refactored multiple methods to use macro templates: minimum, maximum, cumsum, cumprod, sum, prod, min, max, ptp, minmax, max_index, min_index, argmax, argmin, mulsum, clip, seq, logseq, eye, and rand.

## [[0.9.10](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.9...v0.9.10)] - 2025-11-18

- fix to use require for compatibility with distributions installing extensions separately: [b6ceeb6](https://github.com/yoshoku/numo-narray-alt/commit/b6ceeb6ccee122c24cebeb2676d4abd742767fdb)
  - Fixed a `LoadError` on Rocky Linux where built native extensions are installed under `/usr/local/lib64/gems/`
    separately from Ruby code under `/usr/local/share/gems/`. Changed to use `require` instead of `require_relative`
    for loading native extensions to support this directory layout.

## [[0.9.9](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.8...v0.9.9)] - 2025-11-18

- change installation path for the native extension file: [a826564](https://github.com/yoshoku/numo-narray-alt/commit/a826564c18341bcc6e89ce6a2753b667929fd90e)
- change to use require_relative: [1f6a4be](https://github.com/yoshoku/numo-narray-alt/commit/1f6a4bed02978921fe4052f5c27bcbf1076358e3)
- add alt.rb for explicit numo-narray-alt loading: [762048a](https://github.com/yoshoku/numo-narray-alt/commit/762048afcecc6511caef049d3f666c96d610bb9a)
  ```ruby
  require 'numo/narray/alt'
  ```
- add post_install_message about conflict with numo-narray: [092e8b8](https://github.com/yoshoku/numo-narray-alt/commit/092e8b89cf2b4e9de38598524104df76febb22f1)
- add constant value and class method to indicate wheter it is numo-narray-alt: [d71f4ff](https://github.com/yoshoku/numo-narray-alt/commit/d71f4ff49820b4ba8d4732f283b842dca9e2016a)
  ```ruby
  > Numo::NArray::ALTERNATIVE
  => true
  > Numo::NArray.alternative?
  => true
  ```

## [[0.9.8](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.7...v0.9.8)] - 2025-11-13

- fix variable typo in `log2` and `log10` methods for complex array classes (`Numo::SComplex`, `Numo::DComplex`)
  causing incorrect calculations: [289b747](https://github.com/yoshoku/numo-narray-alt/commit/289b747f16adb9209835172db8024970326ad52c)
- refactor `Math` module methods for floating-point and complex array classes.

## [[0.9.7](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.6...v0.9.7)] - 2025-11-03

- add `argsort` method.

## [[0.9.6](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.5...v0.9.6)] - 2025-10-16

- add `var`, `stddev`, and `rms` methods to integer-typed narrays.
- refactor `mean` method's native extension code to improve yard documentation comment parsing.
- update API documentation: improve documentation coverage to 96.09% (1135/1186 methods documented).

## [[0.9.5](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.4...v0.9.5)] - 2025-10-09

- implement `cov` method: [8fb42d9](https://github.com/yoshoku/numo-narray-alt/commit/8fb42d967fd6280059cf4093a9abbd9d989bb904)
- add `mean` method to integer-typed narrays: [fc7b6a0](https://github.com/yoshoku/numo-narray-alt/commit/fc7b6a09abe50b363f3da7d3be28bfbc95e2e408)
- minor fix to workflows, configuration files, and documentation.

## [[0.9.4](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.3...v0.9.4)] - 2025-09-18

- fix a potential segmentation fault that could occur when ndim is 0, such as in Numo::SFloat.cast(42): [5820a54](https://github.com/yoshoku/numo-narray-alt/commit/5820a54c8cd6dfcd3bfc1c86051ec85138db27f9)
- fix a potential segmentation fault or warnings when passing a Range object to the [] method on Ruby 2.x: [036fcb5](https://github.com/yoshoku/numo-narray-alt/commit/036fcb5b6d69aa2030a068abcdec9604777ea816)
- migrate test framework from test-unit to minitest to use ruby_memcheck.
- add GitHub Actions workflow for ruby_memcheck.
- add tests for methods defined in lib/numo/narray/extra.rb

## [[0.9.3](https://github.com/yoshoku/numo-narray-alt/compare/95c0525...725e090)] - 2025-09-13

- fork from [Numo::NArray master branch](https://github.com/ruby-numo/numo-narray/tree/95c05257349b954c027c2284834408736244662a).
- move list of develop dependencies to Gemfile.
- decouple code generation with ERB.
- introduce linters such as RuboCop, clang-format, and commitlint.
- simplify the repository file structure for maintainability.
- simplify GitHub workflows to save cost.
- solve some warnings and issues.
- rename default branch name from master to main.
- detach forked repository to avoid operational errors.

**Note**:

The version numbering rule of Numo::NArray Alternative is not compatible with that of original Numo::NArray.
