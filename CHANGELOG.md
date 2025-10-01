## [[0.9.4](https://github.com/yoshoku/numo-narray-alt/compare/v0.9.3...v0.9.4)] - 2025-09-18

- fix a potential segmentation fault that could occur when ndim is 0, such as in Numo::SFloat.cast(42): [5820a54](https://github.com/yoshoku/numo-narray-alt/commit/5820a54c8cd6dfcd3bfc1c86051ec85138db27f9)
- fix a potential segmentation fault or warnings when passing a Range object to the [] method on Ruby 2.x: [036fcb5](https://github.com/yoshoku/numo-narray-alt/commit/036fcb5b6d69aa2030a068abcdec9604777ea816)
- migrate test framework from test-unit to minitest to use ruby_memcheck.
- add GitHub Actions workflow for ruby_memcheck.
- add tets for methods defined in lib/numo/narray/extra.rb

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
