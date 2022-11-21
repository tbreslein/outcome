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

using namespace outcome;

TEST_CASE("Outcome works when used as return value") {
    SECTION("non-failing functions return a value") {
        auto success = []() -> outcome::Outcome<std::string, int> { return std::string("foo"); };
        REQUIRE(success().has_value());
        REQUIRE_FALSE(success().has_error());
        REQUIRE(success().value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<std::string, int> { return 1; };
        REQUIRE(failure().has_error());
        REQUIRE_FALSE(failure().has_value());
        REQUIRE(failure().error() == 1);
    }
}

TEST_CASE("Outcome<void> works when used as return value") {
    SECTION("non-failing functions return successfully") {
        auto success = []() -> outcome::Outcome<void, int> { return {}; };
        REQUIRE(success().has_value());
        REQUIRE_FALSE(success().has_error());
        // does not compile, since Outcome<void> does not have a .value() field; this is working as intended
        // REQUIRE(success().value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<void, int> { return 2; };
        REQUIRE(failure().has_error());
        REQUIRE_FALSE(failure().has_value());
        REQUIRE(failure().error() == 2);
    }
}

TEST_CASE("ErrorReport works when used as error value") {
    SECTION("non-failing functions return a value") {
        auto success = []() -> outcome::Outcome<std::string, outcome::ErrorReport> { return std::string("foo"); };
        REQUIRE(success().has_value());
        REQUIRE_FALSE(success().has_error());
        REQUIRE(success().value() == "foo");
    }
    SECTION("non-failing functions return an error") {
        auto failure = []() -> outcome::Outcome<std::string, outcome::ErrorReport> {
            return outcome::ErrorReport(5, "foobar", "/some/file", 42);
        };
        REQUIRE(failure().has_error());
        REQUIRE_FALSE(failure().has_value());
        REQUIRE(failure().error().code == 5);
        REQUIRE(failure().error().description == "foobar");
        REQUIRE(failure().error().file == "/some/file");
        REQUIRE(failure().error().line == 42);
        REQUIRE(failure().error().message == "Error Code 5\n  File: /some/file\n  Line: 42\n  Description: foobar");
    }
}
