#pragma once

#include "transform_iterator.hpp"

namespace cppstream {
namespace detail {
namespace filter {

template <typename Iterator, typename Function>
constexpr bool is_nothrow_synchronize()
{
    return noexcept(std::declval<Iterator>().at_end())  &&
           noexcept(std::declval<Iterator>().advance()) &&
           noexcept(std::declval<const Function&>()(std::declval<typename Iterator::value_type>()));
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
          function(function),
          synchronized(false)
    {
    }

    template <typename Allocator>
    filter_iterator(Iterator&& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          function(function),
          synchronized(false)
    {
    }

    filter_iterator(const filter_iterator&) = delete;
    filter_iterator(filter_iterator&&) = default;

    filter_iterator& operator= (const filter_iterator&) = delete;
    filter_iterator& operator= (filter_iterator&&) = delete;

    bool at_end() noexcept(detail::filter::is_nothrow_synchronize<Iterator, Function>())
    {
        synchronize();
        return iterator.at_end();
    }

    void advance() noexcept(noexcept(std::declval<Iterator>().advance()))
    {
        iterator.advance();
        synchronized = false;
    }

    reference get_value() noexcept(noexcept(std::declval<Iterator>().get_value()))
    {
        return iterator.get_value();
    }

private:

    void synchronize() noexcept(detail::filter::is_nothrow_synchronize<Iterator, Function>())
    {
        if (synchronized) return;

        while (!(iterator.at_end() || function(iterator.get_value())))
        {
            iterator.advance();
        }

        synchronized = true;
    }

    const Function& function;
    bool synchronized;
};

} // cppstream namespace
