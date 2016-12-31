#pragma once

#include "constexpr_if.hpp"
#include "detail/traits.hpp"

namespace cppstream {

//========================map=======================

template <typename T, typename Source, typename Function>
class map_transformation;

template <typename T, typename Self>
class with_map
{
public:

    template <typename Function>
    auto map(const Function& function) && noexcept(noexcept(function(std::declval<T>())))
    {
        return CPPSTREAM_CONSTEXPR_IFELSE((is_invokable_v<Function, T>),
            noexcept(noexcept(function(std::declval<T>()))) {
                using result = std::result_of_t<Function(T)>;
                return (map_transformation<result, Self, Function>(*this, function));
            },
            noexcept {
                static_assert(false, "Illegal function argument type");
            }
        );
    }
};

//========================flat_map=======================

template <typename T, typename Source, typename Function>
class flat_map_transformation;

template <typename T, typename Self>
class with_flat_map
{
public:

    template <typename Function>
    auto flat_map(const Function& function) && noexcept(noexcept(function(std::declval<T>())))
    {
        return CPPSTREAM_CONSTEXPR_IFELSE((is_invokable_v<Function, T>),
            noexcept(noexcept(function(std::declval<T>()))) {
                using result = remove_cvr_t<std::result_of_t<Function(T)>>;
                return flat_map_impl<result>(function);
            },
            noexcept {
                static_assert(false, "Illegal function argument type");
                return error_t();
            }
        );
    }

private:

    template <typename Result, typename Function>
    auto flat_map_impl(const Function& function) &&
    {
        return CPPSTREAM_CONSTEXPR_IFELSE(is_iterable_v<Result>,
            noexcept {
                using value_type = remove_cvr_t<decltype(*std::declval<Result>().begin())>;
                return (flat_map_transformation<value_type&, Self, Function>(*this, function));
            },
            noexcept {
                static_assert(false, "Function return type should be iterable.");
                return error_t();
            }
        );
    }
};

} // cppstream namespace
