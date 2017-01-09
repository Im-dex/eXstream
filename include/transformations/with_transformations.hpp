#pragma once

#include "constexpr_if.hpp"
#include "detail/traits.hpp"
#include "detail/partial_application.hpp"
#include "error_transformation.hpp"

#include "map_range.hpp"
#include "flat_map_range.hpp"
#include "filter_range.hpp"
#include "distinct_range.hpp"

namespace cppstream {

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

template <typename T,
          typename Self>
class with_transformations
{
public:

    template <typename Function>
    auto map(const Function& function) const noexcept
    {
        return constexpr_if<(is_invokable_v<const Function&, T>)>()
            .then([&](auto) noexcept
            {
                using result = std::result_of_t<const Function&(T)>;
                return make_transformation<map_range, result>(function);
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
        return constexpr_if<is_invokable_v<const Function&, T>>()
            .then([&](auto) noexcept
            {
                using result = remove_cvr_t<std::result_of_t<const Function&(T)>>;
                return constexpr_if<is_iterable_v<result>>()
                    .then([&](auto) noexcept
                    {
                        using allocator = typename Self::allocator;
                        using value_type = remove_cvr_t<decltype(*std::begin(std::declval<result>()))>;

                        return make_transformation<
                            partial_apply4<flat_map_range, allocator>::bind_4,
                            value_type&
                        >(function);
                    })
                    .else_([](auto) noexcept
                    {
                        static_assert(false, "Function return type should be iterable");
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
                return make_transformation<filter_range, T>(function);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function signature");
                return error_transformation();
            })(nothing);
    }

    // TODO: allocator is not neccessary if stream is ordered
    auto distinct() const noexcept
    {
        using allocator = typename Self::allocator;

        return make_transformation<partial_apply3<distinct_range, allocator>::bind_3>();
    }

private:

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }

    template <template <typename, typename, typename> class TransformRange,
              typename Result,
              typename Function>
    auto make_transformation(const Function& function) const noexcept
    {
        using self_range_type = typename Self::range_type;
        using allocator = typename Self::allocator;
        using meta = typename Self::meta;
        using range_type = TransformRange<self_range_type, Function, meta>;
        using new_meta = typename range_type::meta;

        return transformation<Result, Self, Function, range_type, allocator, new_meta>(self(), function, self().get_allocator());
    }

    template <template <typename, typename> class TransformRange>
    auto make_transformation() const noexcept
    {
        using self_range_type = typename Self::range_type;
        using allocator = typename Self::allocator;
        using meta = typename Self::meta;
        using range_type = TransformRange<self_range_type, meta>;
        using new_meta = typename range_type::meta;

        return independent_transformation<T, Self, range_type, allocator, new_meta>(self(), self().get_allocator());
    }
};

} // cppstream namespace
