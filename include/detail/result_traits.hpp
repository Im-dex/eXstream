#pragma once

#include "config.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <functional>
CPPSTREAM_RESTORE_ALL_WARNINGS

// NOTE: depends on result of std::result_of

namespace cppstream {
namespace detail {

template <typename T>
class value_storage final
{
public:

    explicit value_storage(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value(std::move(value))
    {
    }

    value_storage(const value_storage&) = delete;
    value_storage(value_storage&&) = delete;

    value_storage& operator= (const value_storage&) = delete;
    value_storage& operator= (value_storage&&) = delete;

    ~value_storage() noexcept(std::is_nothrow_destructible_v<T>) = default;

    T& get_ref() noexcept
    {
        return value;
    }

    const T& get_ref() const noexcept
    {
        return value;
    }

    T&& release() noexcept
    {
        return std::move(value);
    }

    T&& copy() const noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        auto copy = value;
        return std::move(copy);
    }

    bool operator== (const value_storage& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return value == that.value;
    }

    bool operator!= (const value_storage& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(value == that.value);
    }

    bool operator== (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return value == that;
    }

    bool operator!= (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(value == that);
    }

private:

    T value;
};

template <typename T>
bool operator== (const T& value, const value_storage<T>& storage) noexcept(is_nothrow_comparable_v<const T>)
{
    return storage == value;
}

template <typename T>
bool operator!= (const T& value, const value_storage<T>& storage) noexcept(is_nothrow_comparable_v<const T>)
{
    return storage != value;
}

template <typename T>
class reference_storage final
{
public:

    explicit reference_storage(const T& ref) noexcept
        : ref(ref)
    {
    }

    explicit reference_storage(const std::reference_wrapper<T> ref) noexcept
        : ref(ref)
    {
    }

    reference_storage(const reference_storage&) = delete;
    reference_storage(reference_storage&&) = delete;

    reference_storage& operator= (const reference_storage&) = delete;
    reference_storage& operator= (reference_storage&&) = delete;

    const T& get_ref() const noexcept
    {
        return ref.get();
    }

    const T& release() const noexcept
    {
        return ref.get();
    }

    const T& copy() const noexcept
    {
        return ref.get();
    }

    bool operator== (const reference_storage& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return ref.get() == that.get_ref();
    }

    bool operator!= (const reference_storage& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(ref.get() == that.get_ref());
    }

    bool operator== (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return ref.get() == that;
    }

    bool operator!= (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(ref.get() == that);
    }

private:

    std::reference_wrapper<const T> ref;
};

template <typename T>
bool operator== (const T& value, const reference_storage<T> storage) noexcept(is_nothrow_comparable_v<const T>)
{
    return storage == value;
}

template <typename T>
bool operator!= (const T& value, const reference_storage<T> storage) noexcept(is_nothrow_comparable_v<const T>)
{
    return storage != value;
}

} // detail namespace

template <typename T>
struct result_traits
{
    using type = T;
    using reference = T&&;

    using storage = detail::value_storage<T>;

    static reference unwrap(T&& ref) noexcept
    {
        return std::move(ref);
    }
};

template <typename T>
struct result_traits<T&>
{
    using type = std::remove_const_t<T>;
    using reference = T&;

    using storage = detail::reference_storage<T>;

    static reference unwrap(T& ref) noexcept
    {
        return ref;
    }
};

template <typename T>
struct result_traits<T&&>
{
    using type = std::remove_const_t<T>;
    using reference = T&&;

    using storage = detail::value_storage<T>;

    static reference unwrap(T&& ref) noexcept
    {
        return std::move(ref);
    }
};

namespace detail {

template <typename T>
struct reference_wrapper_traits
{
    using type = std::remove_const_t<T>;
    using reference = T&;

    using storage = reference_storage<type>;

    static reference unwrap(const std::reference_wrapper<T> ref) noexcept
    {
        return ref.get();
    }
};

} // detail namespace

template <typename T>
struct result_traits<std::reference_wrapper<T>&> : detail::reference_wrapper_traits<T> {};

template <typename T>
struct result_traits<const std::reference_wrapper<T>&> : detail::reference_wrapper_traits<T> {};

template <typename T>
struct result_traits<std::reference_wrapper<T>&&> : detail::reference_wrapper_traits<T> {};

template <typename T>
struct result_traits<const std::reference_wrapper<T>&&> : detail::reference_wrapper_traits<T> {};

template <typename T>
T& get_lvalue_reference(T& ref) noexcept
{
    return ref;
}

template <typename T>
T& get_lvalue_reference(const std::reference_wrapper<T> ref) noexcept
{
    return ref.get();
}

} // cppstream namespaces

namespace std {

template <typename T>
struct hash<cppstream::detail::value_storage<T>>
{
    size_t operator() (const cppstream::detail::value_storage<T>& value) const noexcept
    {
        return std::hash<T>()(value.get_ref());
    }
};

template <typename T>
struct hash<cppstream::detail::reference_storage<T>>
{
    size_t operator() (const cppstream::detail::reference_storage<T> value) const noexcept
    {
        return std::hash<T>()(value.get_ref());
    }
};

} // std namespace
