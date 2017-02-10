#pragma once

#include "detail/traits.hpp"
#include "detail/partial_application.hpp"
#include "detail/result_traits.hpp"
#include "detail/constexpr_if.hpp"
#include "error_transformation.hpp"

#include "map_iterator.hpp"
#include "flat_map_iterator.hpp"
#include "filter_iterator.hpp"
#include "distinct_iterator.hpp"

namespace exstream {

template <typename T,
          typename Source,
          typename Function,
          typename TransformRange,
          typename Allocator,
          typename Meta>
class transformation;

template <typename T,
          typename Source,
          typename TransformRange,
          typename Allocator,
          typename Meta>
class independent_transformation;

template <typename T, typename Self>
class with_transformations
{
public:

    template <typename Function>
    auto map(const Function& function) const noexcept
    {
        return constexpr_if<(is_invokable_v<const Function&, T>)>()
            .then([&](auto) noexcept
            {
                return make_transformation<map_iterator>(function);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function signature");
                return error_transformation();
            })(nothing);
    }

    template <typename Function>
    auto flat_map(const Function& function) const noexcept
    {
        using arg_type = typename Self::iterator_type::result_type; // TODO: simplify

        return constexpr_if<is_invokable_v<const Function&, arg_type>>()
            .then([&](auto) noexcept
            {
                using function_result = std::result_of_t<const Function&(arg_type)>;

                return constexpr_if<is_iterable<std::decay_t<function_result>>::value>()
                    .then([&](auto) noexcept
                    {
                        using allocator = typename Self::allocator;

                        return make_transformation<
                            partial_apply4<flat_map_iterator, allocator>::bind_4
                        >(function);
                    })
                    .else_([](auto) noexcept
                    {
                        static_assert(false, "Function return type needs to be iterable");
                        return error_transformation();
                    })(nothing);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function signature");
                return error_transformation();
            })(nothing);
    }

    template <typename Function>
    auto filter(const Function& function) const noexcept
    {
        return constexpr_if<is_callable_v<const Function&, bool, T>>()
            .then([&](auto) noexcept
            {
                return make_transformation<filter_iterator>(function);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function signature");
                return error_transformation();
            })(nothing);
    }

    auto distinct() const noexcept
    {
        using allocator = typename Self::allocator;

        return make_transformation<partial_apply3<distinct_iterator, allocator>::bind_3>();
    }

private:

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }

    template <template <typename, typename, typename> class TransformIterator, typename Function>
    auto make_transformation(const Function& function) const noexcept
    {
        using self_iterator_type = typename Self::iterator_type;
        using allocator = typename Self::allocator;
        using meta = typename Self::meta;

        using iterator_type = TransformIterator<self_iterator_type, Function, meta>;
        using new_meta = typename iterator_type::meta;
        using value_type = typename iterator_type::value_type;

        return transformation<value_type, Self, Function, iterator_type, allocator, new_meta>(self(), function, self().get_allocator());
    }

    template <template <typename, typename> class TransformIterator>
    auto make_transformation() const noexcept
    {
        using self_iterator_type = typename Self::iterator_type;
        using allocator = typename Self::allocator;
        using meta = typename Self::meta;

        using iterator_type = TransformIterator<self_iterator_type, meta>;
        using new_meta = typename iterator_type::meta;
        using value_type = typename iterator_type::value_type;

        return independent_transformation<value_type, Self, iterator_type, allocator, new_meta>(self(), self().get_allocator());
    }
};

} // exstream namespace
