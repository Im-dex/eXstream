#pragma once

#include "transform_iterator.hpp"
#include "option.hpp"
#include "detail/scope_guard.hpp"

namespace exstream {
namespace detail {
namespace filter {

template <typename Iterator, typename Function>
constexpr bool is_nothrow_fetch() noexcept
{
    using reference = typename Iterator::reference;
    using storage = typename result_traits<reference>::storage;
    using function_arg = std::add_lvalue_reference_t<std::add_const_t<typename Iterator::value_type>>;

    return noexcept(std::declval<Iterator&>().has_next())                          &&
           noexcept(std::declval<Iterator&>().next())                              &&
           noexcept(std::declval<const Function&>()(std::declval<function_arg>())) &&
           noexcept(std::declval<option<storage>&>().emplace(std::declval<reference>()));
}

}} // detail::filter namespace

template <typename Iterator,
          typename Function,
          typename Meta>
class filter_iterator final : public transform_iterator<Iterator>
{
public:

    using value_type = typename Iterator::value_type;
    using reference = typename Iterator::reference;
    using meta = Meta;

    template <typename Allocator>
    filter_iterator(const Iterator& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          cache(),
          function(function)
    {
    }

    template <typename Allocator>
    filter_iterator(Iterator&& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          cache(),
          function(function)
    {
    }

    filter_iterator(const filter_iterator&) = delete;
    filter_iterator(filter_iterator&&) = default;

    filter_iterator& operator= (const filter_iterator&) = delete;
    filter_iterator& operator= (filter_iterator&&) = delete;

    bool has_next() noexcept(detail::filter::is_nothrow_fetch<Iterator, Function>())
    {
        if (cache.empty()) fetch();
        return iterator.has_next() || cache.non_empty();
    }

    reference next() noexcept(detail::filter::is_nothrow_fetch<Iterator, Function>())
    {
        if (cache.empty()) fetch();
        EXSTREAM_SCOPE_EXIT noexcept(std::is_nothrow_destructible_v<storage>)
        {
            cache.reset();
        };
        return cache.get().release();
    }

    void skip() noexcept(detail::filter::is_nothrow_fetch<Iterator, Function>())
    {
        cache.reset();
        fetch();
    }

private:

    using storage = typename result_traits<reference>::storage;

    void fetch() noexcept(detail::filter::is_nothrow_fetch<Iterator, Function>())
    {
        while (iterator.has_next())
        {
            auto&& value = iterator.next();

            if (function(std::as_const(get_lvalue_reference(value))))
            {
                cache.emplace(std::forward<decltype(value)>(value));
                break;
            }
        }
    }

    option<storage> cache;
    const Function& function;
};

} // exstream namespace
