/**
 * @file outcome.hpp
 * @brief main entry point for the outcome lib
 * @author Tommy Breslein (https://github.com/tbreslein)
 *
 * @copyright Copyright (c) 2022
 */

#pragma once

#include <variant>

namespace outcome {
    template<class T, class E>
    class Outcome
    {
        /// Holds either a value of type T, or an exception of type E
        const std::variant<T, E> _either;
    };
}

