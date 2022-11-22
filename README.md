# outcome

Small implementation of a template type similar to Rust's `Result`.

I developed this library on the side to use it in my thesis project later.
Because of that it's tiny, and does not have a lot of features, but it does exactly what I need it to do.

## Features

- Minimal API
- single header
- no dependencies (unless building tests, which requires `Catch2`, but that can be downloaded through CPM)
- works with non-copy types
- features a type `ErrorReport` which is a simple struct for containing information about the error

## Adding it to your project

Probably the easiest way to add it to a CMake project is using CPM (replace the `GIT_TAG` with the commit you want to use):

```cmake
#...

CPMAddPackage(
    NAME outcome
    GITHUB_REPOSITORY tbreslein/outcome
    GIT_TAG ff4106da889cd606c678a764849912bdddc7d871
)

add_executable(foo main.cpp)
target_include_directories(foo INTERFACE ${outcome_SOURCE_DIR})
target_link_libraries(foo PRIVATE outcome)

#...
```

Alternatively, if you are using meson, you can use a wrap with a wrap file like:

```wrap
[wrap-git]
url = https://github.com/tbreslein/outcome
revision = ff4106da889cd606c678a764849912bdddc7d871
depth = 1

[provide]
outcome = outcome_dep
```

And then include in your project with:

```meson
#...

cmake = import('cmake')
outcome = cmake.subproject('outcome')
executable('foo',
  'main.cpp',
  dependencies : outcome.dependency('outcome'))

#...
```

## Usage

## Compatibility

`outcome` so far has been tested with:

- `CMake >= 3.24.2`
- `8.5.0 <= g++ --version <= 12.2.0`
- `9.0.1 <= clang++ --version <= 14.0.6`

When including `outcome`, you need to have `C++17` features enabled for your targets.
