# Numo::NArray Alternative

Numo::NArray Alternative is an experimental project forked from [Numo::NArray](https://github.com/ruby-numo/numo-narray).
Its goal is to adopt more modern Ruby features and development practices.
The project owner is also the developer of [Rumale](https://github.com/yoshoku/rumale), a machine learning library,
and may add new features to this project to support the needs of Rumale.
In addition, since Rumale depends on Numo::NArray, namespaces, class names, and method names will be kept unchanged
to allow this library to be used as a drop-in replacement.

The project owner has the utmost respect for Numo::NArray and its creator, Prof. Masahiro Tanaka.
This project is in no way intended to adversely affect the development of the original Numo::NArray.

## Installation

```shell
$ gem install specific_install
$ gem specific_install https://github.com/yoshoku/numo-narray-alt.git
```

## Development

preparation:

```shell
$ git clone https://github.com/yoshoku/numo-narray-alt
$ cd numo-narray-alt
$ bundle install
```

build and test:

```
$ bundle exec rake compile
$ bundle exec rake test
```

linter:

```shell
$ bundle exec rubocop
$ clang-format --dry-run --Werror --style=file ext/**/*.h ext/**/*.c
```

## Contributing

This project is in an early experimental stage.
Therefore, we are not accepting any Issues or Pull Requests at this time.
Thank you for your understanding.

## License

This project was originally forked from [Numo::NArray](https://github.com/ruby-numo/numo-narray).
It continues to be licensed under the [BSD-3-Clause License](https://github.com/yoshoku/numo-narray-alt/blob/main/LICENSE).
