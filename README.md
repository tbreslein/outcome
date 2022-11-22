# outcome

Small implementation of a template type similar to Rust's `Result`.

I developed this library on the side to use it in my thesis project later.
Because of that it's tiny, and does not have a lot of features, but it does exactly what I need it to do.

## Features

- Minimal API
- single header
- no dependencies (unless building tests, which requires `Catch2`, but that can be downloaded through CPM)
- works with non-copy types
- works with functions that would otherwise return `void`
- optionally features a type `outcome::ErrorReport` which is a simple struct for containing information about the error; you can activate this feature by compiling your targets with the `OUTCOME_USE_ERRORREPORT` compiler definition.
- optionally features a couple of convenience macros, which you can compile with the `OUTCOME_USE_MACROS` compiler definition.

## Limitations:

- Value and error types HAVE to be different types

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

Generally, I would always recommend having a global `using` statement in your codebase that nails down the type for your error values.
For example, I have this in my projects:

```cpp
template<class T>
using Outcome = outcome::Outcome<T, outcome::ErrorReport>
```

This slims down the return types of functions significantly.

### Regular types for values and errors

```cpp
// Let's look at functions that return a std::string on success or an int on
// failure.

// The first template parameter of outcome::Outcome represents your success type,
// and the second one represents your failure type.
auto get_foo_success() noexcept -> outcome::Outcome<std::string, int> {
    // This function just succeeds, and you only need to return the std::string
    // to construct the correct variant.
    return std::string("foo");
}

auto get_foo_failure() noexcept -> outcome::Outcome<std::string, int> {
    // If the function fails, you can in turn just return a value of your error
    // type.
    return 1;
}

int main() {
    auto success_outcome {get_foo_success()};

    // you can check if an outcome::Outcome has a value instead of an error, by
    // calling the .value() method.Since get_foo_success returned the success
    // type, std::string, this assert passes.
    assert(success_outcome.has_value();

    // In turn, the .has_error() method returns whether the container carries an
    // error.
    assert(!success_outcome.has_error());

    // We can reach into the container to the value and acquire a copy with
    // the .value() method
    assert(success_outcome.value() == "foo");

    // You can also just get a pointer, if you do not want to copy the value
    static_assert(
      std::is_same_v<std::string *, decltype(success_outcome.ptr())>);
    assert(*(success_outcome.ptr()) == "foo");

    // Similarly, we can do the .has_value() and .has_error() checks on errors
    // too.
    auto failure_outcome {get_foo_failure()};
    assert(failure_outcome.has_error());
    assert(!failure_outcome.has_value());

    // The equivalent to .value() for errors is .error()
    assert(failure_outcome.error() == 1);

    // Note that .value() and .error() will throw with a bad variant access if
    // the container does not contain the type you expected.
}
```

### The `outcome::Outcome<void, E>` specialisation

```cpp
// The container features a special overload for functions that typically do
// not return anything.
// For that purpose, just set the payload type to void.
auto do_stuff() noexcept -> outcome::Outcome<void, int> {
    printf("hey!");

    // In order to trigger the correct constructor though, you need to return
    // empty braces.
    return {};
}

int main() {
    // Note that the outcome::Outcome struct is [[nodiscard]], so even for these
    // cases you need to handle the error, otherwise your compiler will probably
    // complain.

    do_stuff(); // compiler will probably warn you about discarding the return
                // value here.

    auto possibly_err {do_stuff()}; // This is fine
    if (possibly_err.has_error()) {
        return possibly_err.error();
    }
    return 0;
}
```

### Non-copy types

```cpp
// When you have a function that returns something that is not
// copy-constructible, the semantics change slightly.
// The case that I ran into most often for this, is std::unique_ptr.

auto success() noexcept -> outcome::Outcome<std::unique_ptr<double>, int> {
    return std::make_unique<double>(2.0);
}

auto failure() noexcept -> outcome::Outcome<std::unique_ptr<double>, int> {
    return 10;
}

int main() {
    auto success_outcome {success()};
    auto failure_outcome {failure()};

    assert(success_outcome.has_value());
    assert(!success_outcome.has_error());

    assert(!failure_outcome.has_value());
    assert(failure_outcome.has_error());

    // Trying to retrieve the value directly will trip a static_assert on types
    // that do not satisfy std::is_copy_constructible_v.
    // success.value() // trips static assertion

    // Instead, you can either get a pointer to the object
    static_assert(
      std::is_same_v<std::unique_ptr<double>*, decltype(success_outcome.ptr())>);
    assert(*(success_outcome.ptr()) == 2.0);

    // And you can also move the value out of the container
    std::unique_ptr<double> x {success_outcome.move()};
    static_assert(std::is_same_v<std::unique_ptr<double>*, decltype(x)>);
    assert(x);
    assert(*x.get() == 2.0);
}
```

### ErrorReport

```cpp
// When compiling the library with the OUTCOME_USE_ERRORREPORT compiler
// definition you get access to the outcome::ErrorReport struct.
// This is a small struct for carrying a bit of info about the error that
// occured.

auto failure() -> outcome::Outcome<int, outcome::ErrorReport> {
    // The constructor takes four arguments:
    // - An int representing the error code
    // - A string describing the error
    // - A string describing where the error was caught (like __FILE__)
    // - An int for the line in that file (like __LINE__);
    return outcome::ErrorReport(5, "here's what happened", "/some/source/file", 42);
}

int main() {
    auto foo {failure()};
    assert(foo.has_error());

    // You can access the fields you set through the constructor as expected:
    assert(foo.code == 5);
    assert(foo.description == "here's what happened");
    assert(foo.file == "/some/source/file");
    assert(foo.line == 42);

    // It also, upon construction, crafts an error message for you that you can
    // dump to stderr.
    std::cerr << foo.message << std::endl;
    // Prints:
    //
    // ** Error! **
    //   Code: 5
    //   File: /some/source/file
    //   Line: 42
    //   Description: here's what happened

    // ... and return the error code
    return foo.code;
}
```

### Macros

```cpp
// There are also some macros that you can add to your target by compiling it
// with the OUTCOME_USE_MACROS compiler definition

// OUTCOME_ENSURE checks a condition, and, if the condition does not hold,
// it returns from the enclosing scope with your error type.

// Assume that we use outcome::ErrorReport as the error type.
// The first argument of the macro is just the condition that you want to be
// true. The second is the error value the function should return in case the
// condition did not hold.

// Important note: The if branch where the error occurs is annotated with
// [[unlikely]], so you should use these macros on conditions that should
// actually be true.
auto foo() noexcept -> outcome::Outcome<int, outcome::ErrorReport> {
    // This would pass and this not disrupt the function flow
    const int i {2};
    OUTCOME_ENSURE(i > 1,
      outcome::ErrorReport(1, "i must be greater than 1!", __FILE__, __LINE__));

    // This condition would not pass, so the outcome::ErrorReport is returned.
    OUTCOME_ENSURE(i % 2 != 0,
      outcome::ErrorReport(2, "i must be uneven!", __FILE__, __LINE__));

    // This part of the code is never reached, because the previous
    // OUTCOME_ENSURE call made the function return early.
    printf("I am never reached!");
    return i+1;
}


// OUTCOME_UNWRAP runs a function call, and returns from the enclosing scope in
// case it returns an error.
// This is obviously only really useful for functions that return
// outcome::Outcome<void, E>.


// This function prints its argument, but only if that argument is even.
// Otherwise it returns an error.
auto print_if_even(const int i) noexcept
        -> outcome::Outcome<void, outcome::ErrorReport> {

    OUTCOME_ENSURE(i % 2 == 0,
      outcome::ErrorReport(101, "i must be even!", __FILE__, __LINE__));
    printf("i = %d\n", i);
    return {};
}

// This function prints its argument, but only if that argument is uneven.
// Otherwise it returns an error.
auto print_if_uneven(const int i) noexcept
        -> outcome::Outcome<void, outcome::ErrorReport> {

    OUTCOME_ENSURE(i % 2 != 0,
      outcome::ErrorReport(102, "i must be uneven!", __FILE__, __LINE__));
    printf("i = %d\n", i);
    return {};
}

auto foo() -> outcome::Outcome<void, outcome::ErrorReport> {
    const int i {2};

    // This function call would just pass since i is indeed even, so we see the
    // terminal output and the function flow continues.
    OUTCOME_UNWRAP(print_if_even(2));

    // This function call would NOT pass, so the macro returns the
    // outcome::ErrorReport described in the function.
    OUTCOME_UNWRAP(print_if_uneven(2));

    // This call is never reached, since the previous OUTCOME_UNWRAP call had
    // the function return early.
    printf("I am never reached\n");
    return {};
}
```

## Compatibility

`outcome` so far has been tested with:

- `CMake >= 3.24.2`
- `8.5.0 <= g++ --version <= 12.2.0`
- `9.0.1 <= clang++ --version <= 14.0.6`

When including `outcome`, you need to have `C++17` features enabled for your targets.

## License

`Outcome` is licensed under the [MIT License](https://opensource.org/licenses/MIT):

> Copyright (c) 2022 Tommy Breslein
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

## References

- [std::Result in Rust](https://doc.rust-lang.org/std/result/) + [Data.Either in Haskell](https://hackage.haskell.org/package/base-4.17.0.0/docs/Data-Either.html): What tought me how important types like these are
- [eyre::Result](https://docs.rs/eyre/latest/eyre/type.Result.html) + [color_eyre::eyre::Result](https://github.com/yaahc/color-eyre): Incredible UX
- [`bitwizeshift`'s implementation'](https://github.com/bitwizeshift/result): I got the idea for `Outcome<void, E>` from there.
