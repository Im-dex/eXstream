#pragma once

#include "detail/bool_c.hpp"
#include "utility.hpp"

namespace cppstream {

constexpr size_t type_list_npos = size_t(-1);

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

template <typename T, typename... Ts>
struct contains;

template <typename T, typename H, typename... Ts>
struct contains<T, H, Ts...>
{
    static constexpr bool value = std::is_same_v<T, H> && contains<T, Ts...>::value;
};

template <typename T>
struct contains<T>
{
    static constexpr bool value = false;
};

template <size_t Index, typename T, typename... Ts>
struct index_of;

template <size_t Index, typename T, typename H, typename... Ts>
struct index_of<Index, T, H, Ts...>
{
    static constexpr size_t value = std::is_same_v<T, H> ? Index : index_of<Index + 1, T, Ts...>::value;
};

template <size_t Index, typename T>
struct index_of<Index, T>
{
    static constexpr size_t value = type_list_npos;
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

    template <typename T>
    static constexpr bool contains() noexcept
    {
        return detail::contains<T, Head, Tail...>::value;
    }

    template <typename T>
    static constexpr bool contains(type_t<T>) noexcept
    {
        return contains<T>();
    }

    template <typename T>
    static constexpr size_t index_of() noexcept
    {
        return detail::index_of<T, Head, Tail...>::value;
    }

    template <typename T>
    static constexpr size_t index_of(type_t<T>) noexcept
    {
        return index_of<T>();
    }
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

    template <typename>
    static constexpr bool contains() noexcept
    {
        return false;
    }

    template <typename T>
    static constexpr bool contains(type_t<T>) noexcept
    {
        return false;
    }

    template <typename>
    static constexpr size_t index_of() noexcept
    {
        return type_list_npos;
    }

    template <typename T>
    static constexpr size_t index_of(type_t<T>) noexcept
    {
        return type_list_npos;
    }
};

} // cppstream namespace
