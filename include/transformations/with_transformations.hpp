#pragma once

#include "detail/traits.hpp"
#include "detail/partial_application.hpp"
#include "detail/result_traits.hpp"
#include "detail/constexpr_if.hpp"
#include "bad_stream.hpp"

#include "map_iterator.hpp"
#include "flat_map_iterator.hpp"
#include "filter_iterator.hpp"
#include "distinct_iterator.hpp"

namespace exstream {

template <typename T,
          typename Source,
          typename Function,
          typename TransformRange,
          typename Meta>
class transformation;

template <typename T,
          typename Source,
          typename TransformRange,
          typename Meta>
class independent_transformation;

enum class inappropriate_map_predicate;
enum class map_predicate_returns_void;
enum class inappropriate_flat_map_predicate;
enum class inappropriate_flat_map_predicate_result;
enum class inappropriate_filter_predicate;

template <typename T, typename Self>
class with_transformations
{
public:

    template <typename Function>
    auto map(const Function& function) const noexcept
    {
        static_assert(is_invokable_v<const Function&, T>,
                      "'map' predicate needs to be a function that takes a stream element and returns non-void value");

        return map(function, is_invokable<const Function&, T>());
    }

    template <typename Function>
    auto flat_map(const Function& function) const noexcept
    {
        static_assert(is_invokable_v<const Function&, T>,
                      "'flat_map' predicate needs to be a function that takes a stream element and returns 'Iterable' value");

        return flat_map(function, is_invokable<const Function&, T>());
    }

    template <typename Function>
    auto filter(const Function& function) const noexcept
    {
        static_assert(is_callable_v<const Function&, bool, T>,
                      "'filter' predicate needs to be a function that takes a stream element and returns boolean value");

        return filter(function, is_callable<const Function&, bool, T>());
    }

    template <typename Allocator = std::allocator<T>>
    auto distinct(Allocator&& alloc = Allocator()) const noexcept
    {
        return make_independent_transformation<distinct_iterator>(std::forward<Allocator>(alloc));
    }

private:

    template <typename Function>
    auto map(const Function& function, std::true_type) const noexcept
    {
        using result = std::result_of_t<const Function&(T)>;

        static_assert(!std::is_void_v<result>, "'map' predicate should returns non-void value");
        return map_impl(function, std::negation<std::is_void<result>>());
    }

    template <typename Function>
    static auto map(const Function&, std::false_type) noexcept
    {
        return bad_stream<inappropriate_map_predicate>();
    }

    template <typename Function>
    auto map_impl(const Function& function, std::true_type) const noexcept
    {
        return make_transformation<map_iterator>(function);
    }

    template <typename Function>
    static auto map_impl(const Function&, std::false_type) noexcept
    {
        return bad_stream<map_predicate_returns_void>();
    }

    template <typename Function>
    auto flat_map(const Function& function, std::true_type) const noexcept
    {
        using result = std::result_of_t<const Function&(T)>;

        static_assert(is_iterable_v<result>, "'flat_map' predicate should returns 'Iterable' value");

        return flat_map_impl(function, is_iterable<result>());
    }

    template <typename Function>
    static auto flat_map(const Function&, std::false_type) noexcept
    {
        return bad_stream<inappropriate_flat_map_predicate>();
    }

    template <typename Function>
    auto flat_map_impl(const Function& function, std::true_type) const noexcept
    {
        return make_transformation<flat_map_iterator>(function);
    }

    template <typename Function>
    static auto flat_map_impl(const Function&, std::false_type) noexcept
    {
        return bad_stream<inappropriate_flat_map_predicate_result>();
    }

    template <typename Function>
    auto filter(const Function& function, std::true_type) const noexcept
    {
        return make_transformation<filter_iterator>(function);
    }

    template <typename Function>
    static auto filter(const Function&, std::false_type) noexcept
    {
        return bad_stream<inappropriate_filter_predicate>();
    }

    const Self& self() const noexcept
    {
        return static_cast<const Self&>(*this);
    }

    template <template <typename, typename, typename> class TransformIterator,
              typename Function,
              typename... Args>
    auto make_transformation(const Function& function, Args&&... args) const noexcept
    {
        using self_iterator_type = typename Self::iterator_type;
        using meta = typename Self::meta;

        using iterator_type = TransformIterator<self_iterator_type, Function, meta>;
        using new_meta = typename iterator_type::meta;
        using value_type = typename iterator_type::value_type;

        return transformation<value_type, Self, Function, iterator_type, new_meta>(self(), function, std::forward<Args>(args)...);
    }

    template <template <typename, typename> class TransformIterator,
              typename... Args>
    auto make_independent_transformation(Args&&... args) const noexcept
    {
        using self_iterator_type = typename Self::iterator_type;
        using meta = typename Self::meta;

        using iterator_type = TransformIterator<self_iterator_type, meta>;
        using new_meta = typename iterator_type::meta;
        using value_type = typename iterator_type::value_type;

        return independent_transformation<value_type, Self, iterator_type, new_meta>(self(), std::forward<Args>(args)...);
    }
};

} // exstream namespace
