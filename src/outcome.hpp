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

/// @brief Namespace for this lib
namespace outcome {

/**
 * @brief Minimal implementation of Rust's `Result` type aiming to be at least usable for most instances with a minimal
 * API.
 * @tparam T Value type
 * @tparam E Error type
 *
 * Under the hood, this is just a small wrapper around std::variant.
 */
template <class T, class E>
class [[nodiscard]] Outcome {
    /// @brief The std::variant<T,E> that stores either a value or an error.
    std::variant<T, E> _either;

  public:
    /// @brief Constructs an outcome::Outcome<std::nullopt_t, E>.
    Outcome()
        : _either{std::nullopt} {}

    // cppcheck-suppress noExplicitConstructor
    /// @brief Constructs an outcome::Outcome<T, E> containing the value @p value.
    Outcome(const T value)
        : _either{value} {}

    // cppcheck-suppress noExplicitConstructor
    /// @brief Constructs an outcome::Outcome<_, E> containing the error @p error.
    Outcome(const E error)
        : _either{error} {}

    /// @brief Checks whether the object contains an error of type E
    auto has_error() const noexcept -> bool { return std::holds_alternative<E>(this->_either); }

    /// @brief Checks whether the object contains a value of type T
    auto has_value() const noexcept -> bool { return !this->has_error(); }

    /// @brief Retrieves the value
    auto value() const noexcept -> T { return std::get<T>(this->_either); }

    /// @brief Retrieves the error
    auto error() const noexcept -> E { return std::get<E>(this->_either); }
};

/**
 * @brief Specialisation of outcome::Outcome for functions that would otherwise return a void
 * @tparam E Error type
 *
 * This is realised by storing a value of type std::nullopt_t as the value type.
 */
template <class E>
class [[nodiscard]] Outcome<void, E> {
    /// @brief The std::variant<std::nullopt_t,E> that stores either a std::nullopt or an error.
    std::variant<std::nullopt_t, E> _either;

  public:
    /// @brief Constructs an outcome::Outcome<std::nullopt_t, E>.
    Outcome()
        : _either{std::nullopt} {}

    // cppcheck-suppress noExplicitConstructor
    /// @brief Constructs an outcome::Outcome<std::nullopt_t, E> from the error @p error.
    Outcome(const E error)
        : _either{error} {}

    /// @brief Checks whether the object contains an error of type E
    auto has_error() const noexcept -> bool { return std::holds_alternative<E>(this->_either); }

    /// @brief Checks whether the object contains a std::nullopt
    auto has_value() const noexcept -> bool { return !this->has_error(); }

    /// @brief Retrieves the error
    auto error() const noexcept -> E { return std::get<E>(this->_either); }
};

} // namespace outcome
