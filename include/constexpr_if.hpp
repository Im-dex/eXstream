#pragma once

#include "config.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
CPPSTREAM_RESTORE_ALL_WARNINGS

#define CPPSTREAM_CONSTEXPR_IF(condition, code)\
    CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4296)\
    ::cppstream::detail::constexpr_if<(condition)>([&](const auto) code);\
    CPPSTREAM_MSVC_WARNINGS_POP

#define CPPSTREAM_CONSTEXPR_IFELSE(condition, success, failure)\
    CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4296)\
    ::cppstream::detail::constexpr_branch<(condition)>([&](const auto) success, [&](const auto) failure);\
    CPPSTREAM_MSVC_WARNINGS_POP

namespace cppstream {
namespace detail {

template <bool Condition>
struct if_caller final
{
    template <typename Function>
    CPPSTREAM_FORCEINLINE if_caller(Function&&) noexcept
    {
    }
};

template <>
struct if_caller<true> final
{
    template <typename Function>
    CPPSTREAM_FORCEINLINE if_caller(Function&& function) noexcept(noexcept(function(true)))
    {
        const auto dummyArgument = true;
        function(dummyArgument);
    }
};

template <bool Condition, typename Function>
CPPSTREAM_FORCEINLINE void constexpr_if(Function&& function) noexcept(std::is_nothrow_constructible_v<if_caller<Condition>, Function>)
{
    if_caller<Condition>(std::forward<Function>(function));
}

//======================================================================================================================================

template <bool Condition>
struct ifelse_caller
{
    template <typename SuccessFunction, typename FailureFunction>
    CPPSTREAM_FORCEINLINE static decltype(auto) call(SuccessFunction&&, FailureFunction&& function) noexcept(noexcept(function(true)))
    {
        const auto dummyArgument = true;
        return function(dummyArgument);
    }
};

template <>
struct ifelse_caller<true>
{
    template <typename SuccessFunction, typename FailureFunction>
    CPPSTREAM_FORCEINLINE static decltype(auto) call(SuccessFunction&& function, FailureFunction&&) noexcept(noexcept(function(true)))
    {
        const auto dummyArgument = true;
        return function(dummyArgument);
    }
};

template <bool Condition, typename SuccessFunction, typename FailureFunction>
CPPSTREAM_FORCEINLINE decltype(auto) constexpr_branch(SuccessFunction&& success, FailureFunction&& failure)
    noexcept(noexcept(ifelse_caller<Condition>::call(std::declval<SuccessFunction>(), std::declval<FailureFunction>())))
{
    return ifelse_caller<Condition>::call(std::forward<SuccessFunction>(success), std::forward<FailureFunction>(failure));
}

} // detail namespace
} // cppstream namespace
