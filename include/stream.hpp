#pragma once

#include "transformation.hpp"

namespace stream {

template <typename T, typename Iterator>
class stream final
{
    static_assert(is_iterator_v<Iterator>, "Illegal Iterator type");
public:

    explicit stream(const Iterator& iterator) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : iterator(iterator)
    {
    }

    explicit stream(Iterator&& iterator) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : iterator(std::move(iterator))
    {
    }

    stream(stream&& that) noexcept(std::is_nothrow_move_constructible_v<Iterator>) = default;
    stream& operator= (stream&& that) noexcept(std::is_nothrow_move_assignable_v<Iterator>) = default; // TODO: delete

    stream(const stream&) = delete;
    stream& operator= (const stream&) = delete;

    ~stream() noexcept(std::is_nothrow_destructible_v<Iterator>) = default;

    template <typename Function>
    auto map(Function&& function) && noexcept(detail::map_traits<Iterator, Function>::is_nothrow_map())
    {
        return map(std::forward<Function>(function), is_invokable<Function, const T&>());
    }

private:

    template <typename Function>
    auto map(Function&& function, std::true_type /* is_invokable */) noexcept(detail::map_traits<Iterator, Function>::is_nothrow_map())
    {
        using traits = detail::map_traits<Iterator, Function>;
        return typename traits::result_transformation(std::move(iterator), std::forward<Function>(function));
    }

    template <typename MapFunction>
    static void map(MapFunction&&, std::false_type /* is_invokable */) noexcept
    {
        static_assert(false_v<MapFunction>, "Invalid map function");
    }

    Iterator iterator;
};

} // stream namespace
