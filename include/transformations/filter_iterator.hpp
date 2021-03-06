#pragma once

#include "transform_iterator.hpp"
#include "option.hpp"
#include "detail/scope_guard.hpp"

// TODO: optimize filter ordered with greater, less, less_or_eq etc

namespace exstream {

template <typename Iterator,
          typename Function,
          typename Meta>
class filter_iterator final : public transform_iterator<Iterator>
{
    using traits = result_traits<typename Iterator::result_type>;
public:

    using value_type = typename traits::value_type;
    using result_type = typename traits::result_type;
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

    bool has_next()
    {
        if (cache.empty()) fetch();
        return iterator.has_next() || cache.non_empty();
    }

    result_type next()
    {
        if (cache.empty()) fetch();
        EXSTREAM_SCOPE_SUCCESS noexcept(std::is_nothrow_destructible_v<storage>)
        {
            cache.reset();
        };
        return cache.get().release();
    }

    void skip()
    {
        cache.reset();
        fetch();
    }

    size_t elements_count() const noexcept
    {
        return unknown_count;
    }

private:

    using storage = typename traits::storage;

    void fetch()
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
