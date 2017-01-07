#pragma once

#include "constexpr_if.hpp"
#include "detail/traits.hpp"

#include "error_transformation.hpp"
#include "map_iterator.hpp"
#include "flat_map_iterator.hpp"

namespace cppstream {

template <typename T,
          typename Source,
          typename Function,
          typename BeginTransformIterator,
          typename EndTransformIterator>
class transformation;

template <typename SourceSelf,
          template <typename, typename> class BeginTransformIterator,
          template <typename> class EndTransformIterator>
class mixin
{
protected:

    template <typename Result, typename Function, typename Self>
    auto make_transformation(const Function& function, const Self& self) const noexcept
    {
        using begin_iterator = typename SourceSelf::begin_iterator;
        using end_iterator = typename SourceSelf::end_iterator;

        using begin_transform_iterator = BeginTransformIterator<begin_iterator, Function>;
        using end_transform_iterator = EndTransformIterator<end_iterator>;

        return transformation<Result, SourceSelf, Function, begin_transform_iterator, end_transform_iterator>(static_cast<const SourceSelf&>(self), function);
    }
};

//========================map=======================

template <typename T, typename SourceSelf>
struct with_map : mixin<SourceSelf, begin_map_iterator, end_map_iterator>
{
    template <typename Function>
    auto map(const Function& function) && noexcept
    {
        return constexpr_if<(is_invokable_v<Function, T>)>()
            .then([&](auto) noexcept
            {
                using result = std::result_of_t<Function(T)>;
                return make_transformation<result>(function, *this);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function argument type");
                return error_transformation();
            })(nothing);
    }
};

//========================flat_map=======================

template <typename T, typename SourceSelf>
struct with_flat_map : mixin<SourceSelf, begin_flat_map_iterator, end_flat_map_iterator>
{
    template <typename Function>
    auto flat_map(const Function& function) && noexcept
    {
        return constexpr_if<is_invokable_v<Function, T>>()
            .then([&](auto) noexcept
            {
                using result = remove_cvr_t<std::result_of_t<Function(T)>>;
                return constexpr_if<is_iterable_v<result>>()
                    .then([&](auto) noexcept
                    {
                        using value_type = remove_cvr_t<decltype(*std::declval<result>().begin())>;
                        return make_transformation<value_type&>(function, *this);
                    })
                    .else_([](auto) noexcept
                    {
                        static_assert(false, "Function return type should be iterable");
                        return error_transformation();
                    })(nothing);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Illegal function argument type");
                return error_transformation();
            })(nothing);
    }
};

//========================transformations=======================

template <typename T, typename SourceSelf>
struct with_transformations : public with_map<T, SourceSelf>,
                              public with_flat_map<T, SourceSelf>
{
};

} // cppstream namespace
