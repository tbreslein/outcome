/**
 * @file outcome.hpp
 * @brief main entry point for the outcome lib
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

#pragma once

#include <optional>
#include <variant>

namespace outcome {

template <class T, class E>
struct [[nodiscard]] Outcome {
    std::variant<T, E> _either;
    Outcome()
        : _either{std::nullopt} {}
    // cppcheck-suppress noExplicitConstructor
    Outcome(const T value_in)
        : _either{value_in} {}
    // cppcheck-suppress noExplicitConstructor
    Outcome(const E error)
        : _either{error} {}
    auto has_error() const -> bool { return std::holds_alternative<E>(this->_either); }
    auto has_value() const -> bool { return !this->has_error(); }
    auto value() const -> T { return std::get<T>(this->_either); }
    auto error() const -> E { return std::get<E>(this->_either); }
};

template <class E>
struct [[nodiscard]] Outcome<void, E> {
    std::variant<std::nullopt_t, E> _either;
    Outcome()
        : _either{std::nullopt} {}
    // cppcheck-suppress noExplicitConstructor
    Outcome(const E error)
        : _either{error} {}
    auto has_error() const -> bool { return std::holds_alternative<E>(this->_either); }
    auto has_value() const -> bool { return !this->has_error(); }
    auto error() const -> E { return std::get<E>(this->_either); }
};

} // namespace outcome
