#pragma once

#include "utility.hpp"

namespace stream {

template <typename... Ts>
struct type_list;

template <typename T>
struct is_type_list : public std::false_type
{
};

template <typename... Ts>
struct is_type_list<type_list<Ts...>> : public std::true_type
{
};

template <typename T>
constexpr bool is_type_list_v = is_type_list<T>::value;

namespace detail {

template <typename T, typename... Ts>
struct concat
{
    static_assert(false_v<T>, "T isn't a type list");
    using type = type_list<>;
};

template <typename... Us, typename... Ts>
struct concat<type_list<Us...>, Ts...>
{
    using type = type_list<Ts..., Us...>;
};

} // detail namespace

template <typename Head, typename... Tail>
struct type_list<Head, Tail...> final
{
    using head = Head;
    using tail = type_list<Tail...>;

    static constexpr size_t size() noexcept
    {
        return sizeof...(Tail) + 1;
    }

    static bool empty() noexcept
    {
        return false;
    }

    static bool non_empty() noexcept
    {
        return true;
    }

    template <typename T>
    using concat = typename detail::concat<T, Head, Tail...>::type;

    template <typename T>
    using push_front = type_list<T, Head, Tail...>;

    template <typename T>
    using push_back = type_list<Head, Tail..., T>;
};

template <>
struct type_list<> final
{
    using tail = type_list<>;

    static constexpr size_t size() noexcept
    {
        return 0;
    }

    static bool empty() noexcept
    {
        return true;
    }

    static bool non_empty() noexcept
    {
        return false;
    }

    template <typename U>
    using concat = typename detail::concat<U>::type;

    template <typename T>
    using push_front = type_list<T>;

    template <typename T>
    using push_back = type_list<T>;
};

} // stream namespace
