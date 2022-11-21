/**
 * @file tests.cpp
 * @brief Unit tests
 * @author Tommy Breslein (https://github.com/tbreslein)
 *
 * @copyright Copyright (c) 2022 Tommy Breslein <https://github.com/tbreslein>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "outcome/outcome.hpp"
#include <catch2/catch_all.hpp>
#include <memory>
#include <iostream>

using namespace outcome;

TEST_CASE("Regular value types") {
    SECTION("non-failing functions return a value") {
        auto success = []() -> outcome::Outcome<std::string, int> { return std::string("foo"); }();
        REQUIRE(success.has_value());
        REQUIRE_FALSE(success.has_error());
        REQUIRE(success.value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<std::string, int> { return 1; }();
        REQUIRE(failure.has_error());
        REQUIRE_FALSE(failure.has_value());
        REQUIRE(failure.error() == 1);
    }
}

TEST_CASE("void as value type") {
    SECTION("non-failing functions return successfully") {
        auto success = []() -> outcome::Outcome<void, int> { return {}; }();
        REQUIRE(success.has_value());
        REQUIRE_FALSE(success.has_error());
        // does not compile, since Outcome<void> does not have a .value() field; this is working as intended
        // REQUIRE(success().value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<void, int> { return 2; }();
        REQUIRE(failure.has_error());
        REQUIRE_FALSE(failure.has_value());
        REQUIRE(failure.error() == 2);
    }
}

TEST_CASE("ErrorReport as error value") {
    SECTION("non-failing functions return a value") {
        auto success = []() -> outcome::Outcome<std::string, outcome::ErrorReport> { return std::string("foo"); }();
        REQUIRE(success.has_value());
        REQUIRE_FALSE(success.has_error());
        REQUIRE(success.value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<std::string, outcome::ErrorReport> {
            return outcome::ErrorReport(5, "foobar", "/some/file", 42);
        }();
        REQUIRE(failure.has_error());
        REQUIRE_FALSE(failure.has_value());
        REQUIRE(failure.error().code == 5);
        REQUIRE(failure.error().description == "foobar");
        REQUIRE(failure.error().file == "/some/file");
        REQUIRE(failure.error().line == 42);
        REQUIRE(failure.error().message == "Error Code 5\n  File: /some/file\n  Line: 42\n  Description: foobar");
    }
}

TEST_CASE("Value types that are not copy-constructible, but move-constructible") {
    auto success = []() -> outcome::Outcome<std::unique_ptr<double>, int> {return std::make_unique<double>(2.0); }();
    auto failure = []() -> outcome::Outcome<std::unique_ptr<double>, int> {return 10; }();
    SECTION("has_value and has_error still work as expected") {
        REQUIRE(success.has_value());
        REQUIRE_FALSE(success.has_error());
        REQUIRE(failure.has_error());
        REQUIRE_FALSE(failure.has_value());
    }
    SECTION("retrieving the error value still works") {
        REQUIRE(failure.error() == 10);
    }
    SECTION("must use .move() or .ptr() instead of .value()") {
        // v This does not compile since you cannot get the raw value behind an object that is not copy_constructible.
        // REQUIRE(success.value());

        // Instead you can get a pointer to the underlying object, which is especially useful for containers like
        // std::unique_ptr
        static_assert(std::is_same_v<std::unique_ptr<double>*, decltype(success.ptr())>);
        REQUIRE(*(success.ptr())->get() == 2.0);

        // ... or you use move semantics
        auto moved = success.move();
        static_assert(std::is_same_v<std::unique_ptr<double>, decltype(moved)>);
        REQUIRE(moved); // moved_foo does contain a value

        // and access the value inside the unique_ptr as usual
        REQUIRE(*moved.get() == 2.0);
    }
}
