#pragma once

#include "constexpr_if.hpp"
#include "type_traits.hpp"

namespace exstream {

template <typename Signature>
class function_view;

template <typename Ret, typename... Args>
class function_view<Ret(Args...)> final
{
public:

    template <typename Function>
    explicit function_view(Function&& function) noexcept
        : function(std::addressof(function)),
          caller(nullptr)
    {
        constexpr_if<is_callable_v<Function, Ret, Args...>>()
            .then([&](auto) noexcept
            {
                caller = [](void* func, Args... args) noexcept(noexcept(function(std::declval<Args>()...))) -> Ret
                {
                    return (*static_cast<Function*>(func))(args);
                };
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Invalid function signature");
            })(nothing);
    }

    function_view(const function_view&) = default;
    function_view(function_view&&) = default;

    function_view& operator= (const function_view&) = default;
    function_view& operator= (function_view&&) = default;

    Ret operator() (Args... args) const //noexcept
    {
        return caller(function, std::forward<Args>(args)...);
    }

private:

    void* function;
    Ret(*caller)(void*, Args...);
};

} // exstream namespace
