#pragma once

#include "detail/bool_c.hpp"
#include "utility.hpp"

namespace cppstream {

constexpr size_t type_list_npos = size_t(-1);

template <typename... Ts>
struct type_list;

template <typename T>
struct is_type_list : std::false_type {};

template <typename... Ts>
struct is_type_list<type_list<Ts...>> : std::true_type {};

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

    constexpr type_list() noexcept = default;

    static constexpr size_t size() noexcept
    {
        return sizeof...(Tail) + 1;
    }

    static constexpr bool empty() noexcept
    {
        return false;
    }

    static constexpr bool non_empty() noexcept
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

    constexpr type_list() noexcept = default;

    static constexpr size_t size() noexcept
    {
        return 0;
    }

    static constexpr bool empty() noexcept
    {
        return true;
    }

    static constexpr bool non_empty() noexcept
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

namespace detail {

template <typename T, size_t Index, typename... Ts>
struct index_of;

template <typename T, size_t Index, typename H, typename... Ts>
struct index_of<T, Index, H, Ts...>
{
    static constexpr size_t value = std::is_same_v<T, H> ? Index : index_of<T, Index + 1, Ts...>::value;
};

template <typename T, size_t Index>
struct index_of<T, Index>
{
    static constexpr size_t value = type_list_npos;
};

} // detail namespace

namespace type_list_ops {

template <typename List, typename T>
struct contains;

template <typename T, typename H, typename... Ts>
struct contains<type_list<H, Ts...>, T> : std::disjunction<std::is_same<T, H>, contains<type_list<Ts...>, T>> {};

template <typename T>
struct contains<type_list<>, T> : std::false_type {};

template <typename List, typename T>
constexpr bool contains_v = contains<List, T>::value;

template <typename List, typename T>
struct index_of;

template <typename T, typename... Ts>
struct index_of<type_list<Ts...>, T> : detail::index_of<T, 0, Ts...> {};

template <typename T>
struct index_of<type_list<>, T>
{
    static constexpr size_t value = type_list_npos;
};

template <typename List, typename T>
constexpr size_t index_of_v = index_of<List, T>::value;

} // type_list_ops namespace
} // cppstream namespace
