#pragma once

#include "utility.hpp"
#include "exception.hpp"
#include "iterable.hpp"
#include "ordering_traits.hpp"

STREAM_SUPRESS_ALL_WARNINGS
#include <stdexcept>
#include <cassert>
STREAM_RESTORE_ALL_WARNINGS

#define STREAM_OPTION_TYPE_CHECK(T)\
    static_assert(!std::is_reference_v<T>, STREAM_STRINGIZE(T) ## " shouldn't be reference type");\
    static_assert(!std::is_const_v<T>, STREAM_STRINGIZE(T) ## " shouldn't be const qualified type");\
    static_assert(!std::is_volatile_v<T>, STREAM_STRINGIZE(T) ## " shouldn't be volatile qualified type");

namespace stream {

template <typename T>
class option;

struct none_t final {};

template <typename T>
struct is_option : public std::false_type
{
};

template <typename T>
struct is_option<option<T>> : public std::true_type
{
};

template <typename T>
constexpr bool is_option_v = is_option<T>::value;

template <typename T, bool Const>
class basic_option_iterator final : public iterator<T, Const>
{
    STREAM_OPTION_TYPE_CHECK(T)
public:

    constexpr basic_option_iterator() noexcept
        : pointer(nullptr)
    {
    }

    constexpr explicit basic_option_iterator(std::conditional_t<Const, const T*, T*> pointer) noexcept
        : pointer(pointer)
    {
    }

    basic_option_iterator(const basic_option_iterator&) noexcept = default;
    basic_option_iterator(basic_option_iterator&&) noexcept = default;

    basic_option_iterator& operator= (const basic_option_iterator&) noexcept = default;
    basic_option_iterator& operator= (basic_option_iterator&&) noexcept = default;

    bool is_empty() const noexcept override
    {
        return pointer == nullptr;
    }

    bool non_empty() const noexcept override
    {
        return pointer != nullptr;
    }

    std::conditional_t<Const, const T&, T&> value() noexcept override
    {
        assert(non_empty() && "Iterator is empty");
        return *pointer;
    }

    const T& value() const noexcept override
    {
        assert(non_empty() && "Iterator is empty");
        return *pointer;
    }

    void advance() noexcept override
    {
        pointer = nullptr;
    }

    constexpr bool operator== (const basic_option_iterator& that) const noexcept
    {
        return pointer == that.pointer;
    }


    constexpr bool operator!= (const basic_option_iterator& that) const noexcept
    {
        return pointer != that.pointer;
    }

private:

    std::conditional_t<Const, const T*, T*> pointer;
};

template <typename T>
using option_iterator = basic_option_iterator<T, false>;

template <typename T>
using const_option_iterator = basic_option_iterator<T, true>;

template <typename T>
class option final : public iterable<T, option_iterator<T>, const_option_iterator<T>>,
                     public reverse_iterable<T, option_iterator<T>, const_option_iterator<T>>
{
    STREAM_OPTION_TYPE_CHECK(T)
public:

    option() noexcept
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(true)
    {
    }

    explicit option(const none_t&) noexcept
        : option()
    {
    }

    explicit option(const T& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(false)
    {
        static_assert(std::is_copy_constructible_v<T>, "'T' should be copy constructible");
        construct(that);
    }

    explicit option(T&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(false)
    {
        static_assert(std::is_move_constructible_v<T>, "'T' should be move constructible");
        construct(std::move(that));
    }

    option(const option& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(that.empty)
    {
        static_assert(std::is_copy_constructible_v<T>, "'T' should be copy constructible");
        if (that.non_empty()) construct(that.value());
    }

    option(option&& that) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(that.empty)
    {
        static_assert(std::is_move_constructible_v<T>, "'T' should be move constructible");
        if (that.non_empty())
        {
            construct(std::move(that).value());
            that.destroy();
            that.empty = true;
        }
    }

    template <typename... Ts>
    explicit option(const in_place&, Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
        : iterable(),
          reverse_iterable(),
          storage(),
          empty(false)
    {
        static_assert(std::is_constructible_v<T, Ts&&...>, "'T' should be constructible from 'Ts&&...'");
        construct(std::forward<Ts>(args)...);
    }

    ~option() noexcept(std::is_nothrow_destructible_v<T>)
    {
        destroy();
    }

    option& operator= (const none_t&) noexcept(std::is_nothrow_destructible_v<T>)
    {
        reset();
        return *this;
    }

    option& operator= (const option& that) noexcept(std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_copy_constructible_v<T>, "'T' should be copy constructible");
        static_assert(std::is_copy_assignable_v<T>, "'T' should be copy assignable");

        if (empty && that.empty)             return *this;

        if (non_empty() && that.non_empty()) assign(that.value());
        else if (empty && that.non_empty())  initialize(that.value());
        else                                 reset();

        return *this;
    }

    option& operator= (option&& that) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_move_constructible_v<T>, "'T' should be move constructible");
        static_assert(std::is_move_assignable_v<T>, "'T' should be move assignable");

        if (empty && that.empty) return *this;

        if (non_empty() && that.non_empty()) assign(std::move(that).value());
        else if (empty && that.non_empty())  initialize(std::move(that).value());
        else                                 reset();
        return *this;
    }

    option& operator= (const T& that) noexcept(std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_copy_constructible_v<T>)
    {
        static_assert(std::is_copy_constructible_v<T>, "'T' should be copy constructible");
        static_assert(std::is_copy_assignable_v<T>, "'T' should be copy assignable");

        if (empty) initialize(that);
        else       assign(that);
        return *this;
    }

    option& operator= (T&& that) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
    {
        static_assert(std::is_move_constructible_v<T>, "'T' should be move constructible");
        static_assert(std::is_move_assignable_v<T>, "'T' should be move assignable");

        if (empty) initialize(std::move(that));
        else       assign(std::move(that));
        return *this;
    }

    void reset() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if (non_empty())
        {
            destroy();
            empty = true;
        }
    }

    bool operator== (const none_t&) const noexcept
    {
        return is_empty();
    }

    bool operator!= (const none_t&) const noexcept
    {
        return non_empty();
    }

    bool operator== (const option& that) const noexcept(noexcept(std::declval<T>() == std::declval<T>()))
    {
        if (empty == that.empty)
            return empty ? true : value() == that.value();

        return false;
    }

    bool operator!= (const option& that) const noexcept(noexcept(std::declval<T>() == std::declval<T>()))
    {
        return !(*this == that);
    }

    bool operator== (const T& that) const noexcept(noexcept(std::declval<T>() == std::declval<T>()))
    {
        return empty ? false : value() == that;
    }

    bool operator!= (const T& that) const noexcept(noexcept(std::declval<T>() != std::declval<T>()))
    {
        return empty ? true : value() != that;
    }

    size_t size() const noexcept
    {
        return empty ? size_t(0) : size_t(1);
    }

    bool is_empty() const noexcept
    {
        return empty;
    }

    bool non_empty() const noexcept
    {
        return !empty;
    }

    const T& get() const &
    {
        return get_or_throw<no_such_element_exception>();
    }

    T& get() &
    {
        return get_or_throw<no_such_element_exception>();
    }

    T get() &&
    {
        return std::move(get_or_throw<no_such_element_exception>());
    }

    const T& get_or_else(const T& $default) const noexcept
    {
        return empty ? $default : value();
    }

    T& get_or_else(T& $default) noexcept
    {
        return empty ? $default : value();
    }

    T get_or_else(T&& $default) && noexcept
    {
        return empty ? std::move($default) : std::move(value());
    }

    template <typename E>
    const T& get_or_throw() const &
    {
        static_assert(std::is_default_constructible_v<E>, "E should be default constructible");
        if (empty) throw E{};
        return value();
    }

    template <typename E>
    T& get_or_throw() &
    {
        static_assert(std::is_default_constructible_v<E>, "E should be default constructible");
        if (empty) throw E{};
        return value();
    }

    template <typename E>
    T get_or_throw() &&
    {
        static_assert(std::is_default_constructible_v<E>, "E should be default constructible");
        if (empty) throw E{};
        return std::move(value());
    }

    template <typename E>
    const T& get_or_throw(E&& exception) const &
    {
        if (empty) throw std::forward<E>(exception);
        return value();
    }

    template <typename E>
    T& get_or_throw(E&& exception) &
    {
        if (empty) throw std::forward<E>(exception);
        return value();
    }

    template <typename E>
    T get_or_throw(E&& exception) const &&
    {
        if (empty) throw std::forward<E>(exception);
        return std::move(value());
    }

    void swap(option& that) noexcept(std::is_nothrow_swappable_v<T&> && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    {
        static_assert(std::is_swappable_v<T&>, "T should be swapable");
        static_assert(std::is_move_constructible_v<T>, "T should be move constructible");

        using std::swap;

        if (empty && that.empty) return;

        if (non_empty() && that.non_empty()) swap(value(), that.value());
        else if (empty)
        {
            initialize(std::move(that.value()));
            that.reset();
        }
        else
        {
            that.initialize(std::move(value()));
            reset();
        }
    }

    void swap(T& that) noexcept(std::is_nothrow_swappable_v<T&> && std::is_nothrow_move_constructible_v<T>)
    {
        static_assert(std::is_swappable_v<T&>, "T should be swapable");
        static_assert(std::is_move_constructible_v<T>, "T should be move constructible");

        using std::swap;

        if (empty) initialize(std::move(that));
        else       swap(value(), that);
    }

    option_iterator<T> iterator() noexcept override
    {
        return option_iterator<T>(pointer_or_null());
    }

    const_option_iterator<T> iterator() const noexcept override
    {
        return const_option_iterator<T>(pointer_or_null());
    }

    const_option_iterator<T> const_iterator() noexcept override
    {
        return const_option_iterator<T>(pointer_or_null());
    }

    option_iterator<T> reverse_iterator() noexcept override
    {
        return option_iterator<T>(pointer_or_null());
    }

    const_option_iterator<T> reverse_iterator() const noexcept override
    {
        return const_option_iterator<T>(pointer_or_null());
    }

    const_option_iterator<T> const_reverse_iterator() noexcept override
    {
        return const_option_iterator<T>(pointer_or_null());
    }

private:

    template <typename U>
    friend struct std::hash;

    const T* pointer() const noexcept
    {
        return reinterpret_cast<const T*>(std::addressof(storage));
    }

    T* pointer() noexcept
    {
        return reinterpret_cast<T*>(std::addressof(storage));
    }

    const T* pointer_or_null() const noexcept
    {
        return empty ? nullptr : pointer();
    }

    T* pointer_or_null() noexcept
    {
        return empty ? nullptr : pointer();
    }

    const T& value() const & noexcept
    {
        return *pointer();
    }

    T& value() & noexcept
    {
        return *pointer();
    }

    T&& value() && noexcept
    {
        return std::move(*pointer());
    }

    template <typename... Ts>
    void construct(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
    {
        new (pointer()) T(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void initialize(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
    {
        construct(std::forward<Ts>(args)...);
        empty = false;
    }

    template <typename U>
    void assign(U&& object) noexcept(std::is_nothrow_assignable_v<T, U&&>)
    {
        value() = std::forward<U>(object);
    }

    template <typename U>
    static auto destroy(U*) noexcept -> std::enable_if_t<std::is_trivially_destructible_v<U>>
    {
    }

    template <typename U>
    static auto destroy(U* object) noexcept(std::is_nothrow_destructible_v<U>) -> std::enable_if_t<!std::is_trivially_destructible_v<U>>
    {
        object->~U();
    }

    void destroy() noexcept(noexcept(destroy(pointer())))
    {
        if (non_empty()) destroy(pointer());
    }

    std::aligned_storage_t<sizeof(T)> storage;
    bool empty;
};

template <typename T>
option<remove_cvr_t<T>> some(T&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<T>, T&&>)
{
    return option<remove_cvr_t<T>>(std::forward<T>(value));
}

STREAM_FORCEINLINE constexpr none_t none() noexcept
{
    return none_t();
}

template <typename T, typename... Ts>
option<T> make_option(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts&&...>)
{
    return option<T>(in_place{}, std::forward<Ts>(args)...);
}

template <typename T>
struct ordering_traits<option<T>>
{
    static constexpr bool is_ordered = true;
};

} // stream namespace

namespace std {

template <typename T>
void swap(stream::option<T>& a, stream::option<T>& b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

template <typename T>
void swap(stream::option<T>& option, T& value) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
void swap(T& value, stream::option<T>& option) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
struct hash<stream::option<T>>
{
    size_t operator()(const stream::option<T>& option) const noexcept
    {
        if (option.is_empty()) return 0;

        hash<T> valueHash;
        return valueHash(option.value());
    }
};

} // std namespace
