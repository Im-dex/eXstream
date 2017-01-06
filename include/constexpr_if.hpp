#pragma once

#include "config.hpp"
#include "utility.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <type_traits>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

template <bool>
class constexpr_if;

namespace detail {

template <typename Function>
class constexpr_if_result
{
public:

    constexpr_if_result(const constexpr_if_result&) = delete;
    constexpr_if_result& operator= (const constexpr_if_result&) = delete;

    template <typename T>
    const constexpr_if_result& then(const T&) const noexcept
    {
        return *this;
    }

    template <typename ElseFunction>
    const constexpr_if_result& else_(const ElseFunction&) const noexcept
    {
        return *this;
    }

    template <bool Condition>
    const constexpr_if_result& else_if() const noexcept
    {
        return *this;
    }

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) const noexcept(noexcept(std::declval<Function>()(std::declval<Args>()...)))
    {
        return function(std::forward<Args>(args)...);
    }

private:

    template <bool>
    friend class constexpr_if;

    explicit constexpr_if_result(const Function& function) noexcept
        : function(function)
    {
    }

    constexpr_if_result(constexpr_if_result&&) noexcept = default;

    const Function& function;
};

} // detail namespace

template <bool Condition>
class constexpr_if/*<true>*/
{
public:

    constexpr_if() noexcept = default;

    constexpr_if(const constexpr_if&) = delete;
    constexpr_if& operator= (const constexpr_if&) = delete;

    template <typename Function>
    auto then(const Function& function) const noexcept
    {
        return detail::constexpr_if_result<Function>(function);
    }

    template <typename Function>
    const constexpr_if& else_(const Function&) const noexcept
    {
        return *this;
    }

    template <bool ElseIfCondition>
    const constexpr_if& else_if() const noexcept
    {
        return *this;
    }

private:

    friend class constexpr_if<false>;

    constexpr_if(constexpr_if&&) noexcept = default;
};

template <>
class constexpr_if<false>
{
public:

    constexpr_if() noexcept = default;

    constexpr_if(const constexpr_if&) = delete;
    constexpr_if& operator= (const constexpr_if&) = delete;

    template <typename Function>
    const constexpr_if& then(const Function&) const noexcept
    {
        return *this;
    }

    template <typename Function>
    auto else_(const Function& function) const noexcept
    {
        return detail::constexpr_if_result<Function>(function);
    }

    template <bool ElseIfCondition>
    auto else_if() const noexcept
    {
        return constexpr_if<ElseIfCondition>();
    }

    template <typename... Args>
    void operator()(Args&&...) const noexcept
    {
    }

private:

    constexpr_if(constexpr_if&&) noexcept = default;
};

} // cppstream namespace
