#pragma once

#include "preprocessor.hpp"
#include "uncaught_exceptions_counter.hpp"
#include "constexpr_if.hpp"

#define CPPSTREAM_SCOPE_EXIT\
    const auto CPPSTREAM_ANONYMOUS_VAR(scope_exit) = cppstream::ScopeExit() + [&]()

#define CPPSTREAM_SCOPE_SUCCESS\
    const auto CPPSTREAM_ANONYMOUS_VAR(scope_success) = cppstream::ScopeSuccess() + [&]()

#define CPPSTREAM_SCOPE_FAIL\
    const auto CPPSTREAM_ANONYMOUS_VAR(scope_fail) = cppstream::ScopeFailure() + [&]() noexcept

namespace cppstream {

enum class ScopeExit;
enum class ScopeSuccess;
enum class ScopeFailure;

template <typename Function>
class scope_guard final
{
public:

    explicit scope_guard(Function&& function) noexcept(std::is_nothrow_move_constructible_v<Function>)
        : function(std::move(function))
    {
    }

    scope_guard(scope_guard&& that) noexcept(std::is_nothrow_move_constructible_v<Function>)
        : function(std::move(that.function))
    {
    }

    scope_guard(const scope_guard&) = delete;

    scope_guard& operator= (const scope_guard&) = delete;
    scope_guard& operator= (scope_guard&&) = delete;

    ~scope_guard() noexcept(noexcept(std::declval<Function>()()))
    {
        function();
    }

private:

    Function function;
};

namespace detail {

template <typename Function, bool ExecuteOnException>
constexpr bool is_nothrow_destructor() noexcept
{
    return ExecuteOnException ? true : noexcept(std::declval<Function>()());
}

} // detail namespace

template <typename Function, bool ExecuteOnException>
class scope_guard_for_new_excepotion final
{
public:

    explicit scope_guard_for_new_excepotion(Function&& function) noexcept(std::is_nothrow_move_constructible_v<Function>)
        : function(std::move(function)),
          counter()
    {
    }

    scope_guard_for_new_excepotion(scope_guard_for_new_excepotion&& that) noexcept(std::is_nothrow_move_constructible_v<Function>)
        : function(std::move(that.function)),
          counter(std::move(that.counter))
    {
    }

    scope_guard_for_new_excepotion(const scope_guard_for_new_excepotion&) = delete;

    scope_guard_for_new_excepotion& operator= (const scope_guard_for_new_excepotion&) = delete;
    scope_guard_for_new_excepotion& operator= (scope_guard_for_new_excepotion&&) = delete;

    ~scope_guard_for_new_excepotion() noexcept(detail::is_nothrow_destructor<Function, ExecuteOnException>())
    {
        constexpr_if<ExecuteOnException>()
            .then([this](auto) noexcept
            {
                if (counter.is_new_uncaught_exception())
                    function();
            })
            .else_([this](auto)
            {
                if (!counter.is_new_uncaught_exception())
                    function();
            })(nothing);
    }

private:

    Function function;
    uncaught_exceptions_counter counter;
};

template <typename Function>
scope_guard<std::decay_t<Function>> operator+ (ScopeExit, Function&& function)
    noexcept(std::is_nothrow_move_constructible_v<std::decay_t<Function>>)
{
    return scope_guard<std::decay_t<Function>>(std::forward<Function>(function));
}

template <typename Function>
scope_guard_for_new_excepotion<std::decay_t<Function>, true> operator+ (ScopeFailure, Function&& function)
    noexcept(std::is_nothrow_move_constructible_v<std::decay_t<Function>>)
{
    return scope_guard_for_new_excepotion<std::decay_t<Function>, true>(std::forward<Function>(function));
}

template <typename Function>
scope_guard_for_new_excepotion<std::decay_t<Function>, false> operator+ (ScopeSuccess, Function&& function)
    noexcept(std::is_nothrow_move_constructible_v<std::decay_t<Function>>)
{
    return scope_guard_for_new_excepotion<std::decay_t<Function>, false>(std::forward<Function>(function));
}

} // cppstream namespace
