// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <utility>
#include <type_traits>

namespace utility
{
    using Guard = std::function<bool()>;

    /// Invokes a function if specified conditions are met.
    /// \tparam F       Function signature
    /// \tparam Args    Function arguments
    /// \param guard    Conditions to be met
    /// \param func     Callable object
    /// \param args     Function arguments
    /// \return If the function returns void - a flag indicating whether the conditions were met and the function has
    /// been called. If the function returns a type - a pair of: a flag indicating whether the conditions were met, and
    /// a function's return value.
    template <typename F, typename... Args>
    auto conditionally_invoke(const Guard &guard,
                              const F &func,
                              Args... args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
    {
        using ResultType              = typename std::invoke_result_t<F, Args...>;
        constexpr auto isVoidFunction = std::is_void_v<ResultType>;
        if (!guard()) {
            if constexpr (isVoidFunction) {
                return false;
            }
            else {
                return std::pair(false, ResultType{});
            }
        }

        if constexpr (isVoidFunction) {
            std::invoke(func, std::forward<Args>(args)...);
            return true;
        }
        else {
            return std::pair(true, std::invoke(func, std::forward<Args>(args)...));
        }
    }

    /// Invokes a member function if specified conditions are met.
    /// \tparam T       Class which contains the member function F
    /// \tparam F       Function signature
    /// \tparam Args    Function arguments
    /// \param guard    Conditions to be met
    /// \param self     Pointer to the class T instance
    /// \param func     Callable object
    /// \param args     Function arguments
    /// \return If the function returns void - a flag indicating whether the conditions were met and the function has
    /// been called. If the function returns a type - a pair of: a flag indicating whether the conditions were met, and
    /// a function's return value.
    template <class T,
              typename F,
              typename... Args,
              typename std::enable_if_t<std::is_invocable_v<F, T, Args...>, bool> = false>
    auto conditionally_invoke(const Guard &guard,
                              T &&self,
                              const F &func,
                              Args... args) noexcept(std::is_nothrow_invocable_v<F, T, Args...>)
    {
        using ResultType              = typename std::invoke_result_t<F, T, Args...>;
        constexpr auto isVoidFunction = std::is_void_v<ResultType>;
        if (!guard()) {
            if constexpr (isVoidFunction) {
                return false;
            }
            else {
                return std::pair(false, ResultType{});
            }
        }

        if constexpr (isVoidFunction) {
            std::invoke(func, std::forward<T>(self), std::forward<Args>(args)...);
            return true;
        }
        else {
            return std::pair(true, std::invoke(func, std::forward<T>(self), std::forward<Args>(args)...));
        }
    }
} // namespace utility
