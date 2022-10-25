/**
 * @file outcome.hpp
 * @brief main entry point for the outcome lib
 * @author Tommy Breslein (https://github.com/tbreslein)
 *
 * @copyright Copyright (c) 2022
 */

#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <variant>

namespace outcome {

template<class T, class E>
class Outcome
{
    /// Holds either a value of type T, or an exception of type E
    const std::variant<T, E> _either;

public:
    Outcome()
        : _either {std::nullopt}
    {
    }

    explicit Outcome(const T& value_in)
        : _either {value_in}
    {
    }

    explicit Outcome(const E exception_in)
        : _either {exception_in}
    {
    }

    auto has_exception() const -> bool
    {
        return std::holds_alternative<E>(this->_either);
    }

    auto has_value() const -> bool
    {
        return !this->has_exception();
    }

    auto value() const -> T
    {
        return std::get<T>(this->_either);
    }

    auto exception() const -> E
    {
        return std::get<E>(this->_either);
    }
};

template<class T, class E>
class OutcomeUniquePtr
{
    /// Holds either a value of type T, or an exception of type E
    const std::variant<std::unique_ptr<T>, E> _either;

public:
    OutcomeUniquePtr()
        : _either {std::nullopt}
    {
    }

    explicit OutcomeUniquePtr(std::unique_ptr<T> value_ptr_in)
        : _either {std::move(value_ptr_in)}
    {
    }

    explicit OutcomeUniquePtr(const E exception_in)
        : _either {exception_in}
    {
    }

    auto has_exception() const -> bool
    {
        return std::holds_alternative<E>(this->_either);
    }

    auto has_value() const -> bool
    {
        return !this->has_exception();
    }

    auto value() const -> T
    {
        return std::get<T>(this->_either);
    }

    std::unique_ptr<T> move()
    {
        T* raw_ptr = std::get<std::unique_ptr<T>>(this->_either).release();
        return std::unique_ptr<T>(raw_ptr);
    }

    auto exception() const -> E
    {
        return std::get<E>(this->_either);
    }
};

}
