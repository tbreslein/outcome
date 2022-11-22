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
#include <string>
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
    std::variant<T, E> _either; ///< @brief The std::variant<T,E> that stores either a value or an error.

  public:
    /// @brief Constructs an outcome::Outcome<std::nullopt_t, E>.
    Outcome()
        : _either{std::nullopt} {}

    // cppcheck-suppress noExplicitConstructor
    /// @brief Constructs an outcome::Outcome<T, E> containing the value @p value.
    Outcome(T value)
        : _either{[&]() {
            if constexpr (std::is_copy_constructible_v<T>) {
                return value;
            } else {
                auto raw_ptr = value.release();
                return T(std::move(raw_ptr));
            }
        }()} {}

    // cppcheck-suppress noExplicitConstructor
    /// @brief Constructs an outcome::Outcome<_, E> containing the error @p error.
    Outcome(const E error)
        : _either{error} {}

    /// @brief Checks whether the object contains an error of type E
    constexpr auto has_error() const noexcept -> bool { return std::holds_alternative<E>(this->_either); }

    /// @brief Checks whether the object contains a value of type T
    constexpr auto has_value() const noexcept -> bool { return !this->has_error(); }

    /// @brief Retrieves a copy of the value; needs the value to be copy constructible
    constexpr auto value() const {
        static_assert(std::is_copy_constructible_v<T>, ".value() requires T to be copy constructible!");
        return std::get<T>(this->_either);
    }

    /// @brief Retrieves a non-const pointer to the underlying object
    constexpr auto ptr() noexcept { return &std::get<T>(this->_either); }

    /// @brief Retrives a const pointer to the underlying object
    constexpr auto ptr() const noexcept { return &std::get<T>(this->_either); }

    /// @brief Moves the underlying object out of the outcome::Outcome container
    constexpr auto move() noexcept -> T {
        static_assert(std::is_move_constructible_v<T>, ".move() requires T to be move constructible!");
        auto *raw_ptr = std::get<T>(this->_either).release();
        return T(raw_ptr);
    }

    /// @brief Retrieves a copy of the error
    constexpr auto error() const -> E { return std::get<E>(this->_either); }
};

/**
 * @brief Specialisation of outcome::Outcome for functions that would otherwise return a void
 * @tparam E Error type
 *
 * This is realised by storing a value of type std::nullopt_t as the value type.
 */
template <class E>
class [[nodiscard]] Outcome<void, E> {
    std::variant<std::nullopt_t, E>
        _either; ///< @brief The std::variant<std::nullopt_t,E> that stores either a std::nullopt or an error.

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

#ifdef OUTCOME_USE_ERRORREPORT
/**
 * @brief Wrapper around the kind of error that happened
 */
struct ErrorReport {
    const int code;                ///< Integer error code that should identify the kind of error that happened
    const std::string description; ///< Description of the error
    const std::string file;        ///< The source code file where the error occurred
    const int line;                ///< The line in the source code file where the error occurred
    const std::string message;     ///< The error message that can be displayed to the user

    /**
     * @brief Constructs an outcome::ErrorReport
     * @param code The error code
     * @param description Description of the error that occured
     * @param file The source file where the error occurred
     * @param line The line in that file where the error occurred
     */
    explicit ErrorReport(const int code_in, const std::string &description_in, const std::string &file_in,
                         const int line_in)
        : code{code_in}
        , description{description_in}
        , file{file_in}
        , line{line_in}
        , message{"\n** Error! **\n  Code: " + std::to_string(this->code) + "\n  File: " + this->file +
                  "\n  Line: " + std::to_string(this->line) + "\n  Description: " + this->description} {}
};

#endif // OUTCOME_USE_ERRORREPORT

#ifdef OUTCOME_USE_MACROS

/**
 * @brief Checks the condition @p cond, and returns the enclosing scope with @p err if the condition does not hold.
 * @param condition The condition that is going to be checked
 * @param error The error that is going to be returned
 *
 * NOTE: Your condition should be written such that not passing the check is unlikely, since the code path where the
 * condition does NOT hold, is annotated with an `[[unlikely]]`.
 *
 * Example:
 *     // Assume that we use outcome::ErrorReport as the error type.
 *
 *     auto foo() noexcept -> outcome::Outcome<int, outcome::ErrorReport> {
 *         // This would pass and this not disrupt the function flow
 *         const int i {2};
 *         OUTCOME_ENSURE(i > 1, outcome::ErrorReport(101, "i must be greater than 1!", __FILE__, __LINE__));
 *
 *         // This condition would not pass, so the outcome::ErrorReport is returned.
 *         OUTCOME_ENSURE(i % 2 != 0, outcome::ErrorReport(102, "i must be uneven!", __FILE__, __LINE__));
 *
 *         // This part of the code is never reached, because the previous OUTCOME_ENSURE call made the function return
 *         // early.
 *
 *         return i+1;
 *     }
 */
#define OUTCOME_ENSURE(condition, error)                                                                               \
    if (!(condition)) [[unlikely]] {                                                                                   \
        return {error};                                                                                                \
    }

/**
 * @brief Takes the value @p val, and returns the error from the enclosing scope of this macro call, if the value
 * contains an error.
 * @p val The value to check
 *
 * NOTE: The code path where the value DOES contain an error, is annotated with an `[[unlikely]]`.
 *
 * Example:
 *     // Assume that we use outcome::ErrorReport as the error type.
 *
 *     // This function prints its argument, but only if that argument is even; otherwise it returns an error.
 *     auto print_if_even(const int i) noexcept -> outcome::Outcome<void, outcome::ErrorReport> {
 *         OUTCOME_ENSURE(i % 2 == 0, outcome::ErrorReport(101, "i must be even!", __FILE__, __LINE__));
 *         printf("i = %d\n", i);
 *         return {};
 *     }
 *
 *     // This function prints its argument, but only if that argument is uneven; otherwise it returns an error.
 *     auto print_if_uneven(const int i) noexcept -> outcome::Outcome<void, outcome::ErrorReport> {
 *         OUTCOME_ENSURE(i % 2 != 0, outcome::ErrorReport(102, "i must be uneven!", __FILE__, __LINE__));
 *         printf("i = %d\n", i);
 *         return {};
 *     }
 *
 *     auto foo() -> outcome::Outcome<void, outcome::ErrorReport> {
 *         const int i {2};
 *
 *         // This function call would just pass since i is indeed even, so we see the terminal output.
 *         OUTCOME_UNWRAP(print_if_even(2));
 *
 *         // This function call would NOT pass, so the macro returns the outcome::ErrorReport described in the
 *         // function.
 *         OUTCOME_UNWRAP(print_if_uneven(2));
 *
 *         // This call is never reached, since the previous OUTCOME_UNWRAP call had the function return early.
 *         printf("I am never reached\n");
 *         return {};
 *     }
 */
#define OUTCOME_UNWRAP(val)                                                                                            \
    if (const auto err{val}; err.has_error()) [[unlikely]] {                                                           \
        return {err.error()};                                                                                          \
    }

#endif // OUTCOME_USE_MACROS

} // namespace outcome
