#pragma once

#include "config.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <functional>
EXSTREAM_RESTORE_ALL_WARNINGS

// NOTE: depends on result of std::result_of

namespace exstream {
namespace detail {

template <typename T>
class value_storage final
{
public:

    explicit value_storage(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value(std::move(value))
    {
    }

    value_storage(value_storage&&) = default;

    value_storage(const value_storage&) = delete;
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

    T release() noexcept
    {
        return std::move(value);
    }

    T copy() const noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return copy(std::is_copy_constructible<T>());
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

    T copy(std::true_type /* is copy constructible */) const noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return value;
    }

    [[noreturn]]
    static T copy(std::false_type /* is copy constructible */) noexcept
    {
        std:terminate(); // dummy terminate
    }

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

    reference_storage(reference_storage&&) = default;

    reference_storage(const reference_storage&) = delete;
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
    using value_type = T;
    using result_type = T;

    using storage = detail::value_storage<T>;

    static result_type unwrap(T&& ref) noexcept
    {
        return std::move(ref);
    }
};

template <typename T>
struct result_traits<T&>
{
    using value_type = std::remove_const_t<T>;
    using result_type = T&;

    using storage = detail::reference_storage<T>;

    static result_type unwrap(T& ref) noexcept
    {
        return ref;
    }
};

template <typename T>
struct result_traits<T&&>
{
    static_assert(!std::is_const_v<T>, "Rvalue reference to const isn't allowed");

    using value_type = T;
    using result_type = T;

    using storage = detail::value_storage<T>;

    static result_type unwrap(T&& ref) noexcept
    {
        return std::move(ref);
    }
};

namespace detail {

template <typename T>
struct reference_wrapper_traits
{
    using value_type = std::remove_const_t<T>;
    using result_type = T&;

    using storage = reference_storage<value_type>;

    static result_type unwrap(const std::reference_wrapper<T> ref) noexcept
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

} // exstream namespaces

namespace std {

template <typename T>
struct hash<exstream::detail::value_storage<T>>
{
    size_t operator() (const exstream::detail::value_storage<T>& value) const noexcept
    {
        return std::hash<remove_const_t<T>>()(value.get_ref());
    }
};

template <typename T>
struct hash<exstream::detail::reference_storage<T>>
{
    size_t operator() (const exstream::detail::reference_storage<T>& value) const noexcept
    {
        return std::hash<remove_const_t<T>>()(value.get_ref());
    }
};

} // std namespace
