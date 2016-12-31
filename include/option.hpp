#pragma once

#include "utility.hpp"
#include "ordering_traits.hpp"
#include "detail/type_traits.hpp"
#include "detail/type_list.hpp"
#include "not_null.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
#include <iterator>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

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
class basic_option_iterator final
{
public:

    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = std::conditional_t<Const, std::add_pointer_t<std::add_const_t<T>>, std::add_pointer_t<T>>;
    using reference = std::conditional_t<Const, std::add_lvalue_reference_t<std::add_const_t<T>>, std::add_lvalue_reference_t<T>>;
    using iterator_category = std::forward_iterator_tag;

    constexpr basic_option_iterator() noexcept
        : ptr(nullptr)
    {
    }

    constexpr explicit basic_option_iterator(pointer ptr) noexcept
        : ptr(ptr)
    {
    }

    basic_option_iterator(const basic_option_iterator&) noexcept = default;
    basic_option_iterator(basic_option_iterator&&) noexcept = default;

    basic_option_iterator& operator= (const basic_option_iterator&) noexcept = default;
    basic_option_iterator& operator= (basic_option_iterator&&) noexcept = default;

    ~basic_option_iterator() noexcept = default;

    reference operator* () noexcept
    {
        assert(non_empty() && "Iterator is empty");
        return *ptr;
    }

    const value_type& operator* () const noexcept
    {
        assert(non_empty() && "Iterator is empty");
        return *ptr;
    }

    pointer operator-> () noexcept
    {
        assert(non_empty() && "Iterator is empty");
        return ptr;
    }

    const value_type* operator-> () const noexcept
    {
        assert(non_empty() && "Iterator is empty");
        return ptr;
    }

    basic_option_iterator& operator++ () noexcept
    {
        ptr = nullptr;
        return *this;
    }

    basic_option_iterator operator++ (int) noexcept
    {
        auto old = *this;
        ptr = nullptr;
        return old;
    }

    constexpr bool operator== (const basic_option_iterator& that) const noexcept
    {
        return ptr == that.ptr;
    }


    constexpr bool operator!= (const basic_option_iterator& that) const noexcept
    {
        return ptr != that.ptr;
    }

    void swap(basic_option_iterator& that) noexcept
    {
        using std::swap;
        swap(ptr, that.ptr);
    }

private:

    bool is_empty() const noexcept
    {
        return ptr == nullptr;
    }

    bool non_empty() const noexcept
    {
        return ptr != nullptr;
    }

    pointer ptr;
};

template <typename T>
using option_iterator = basic_option_iterator<T, false>;

template <typename T>
using const_option_iterator = basic_option_iterator<T, true>;

template <typename T>
class option final
{
    static_assert(!std::is_const_v<T>, "Optional type shouldn't be const. Use 'const option<T>' declaration instead.");
    static_assert(!std::is_volatile_v<T>, "Optional type shouldn't be volatile. Use 'volatile option<T>' declaration instead.");
    static_assert(!std::is_rvalue_reference_v<T>, "Optional type shouldn't be a rvalue reference.");
    static_assert(!std::is_void_v<T>, "Option can't hold a value of 'void' type.");
    static_assert(!std::is_array_v<T>, "Option can't hold a value of array type.");
    static_assert(std::is_destructible_v<T>, "Optional type should be destructible");
public:

    using value_type = T;
    using iterator = option_iterator<T>;
    using const_iterator = const_option_iterator<T>;
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;
    
    option() noexcept
        : storage(),
          emptyFlag(true)
    {
    }
    
    explicit option(const none_t&) noexcept
        : option()
    {
    }
    
    explicit option(const T& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : storage(),
          emptyFlag(false)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_copy_constructible_v<T>,
            noexcept(std::is_nothrow_copy_constructible_v<T>) {
                construct(that);
            },
            noexcept {
                static_assert(false, "Optional type should be copy constructible");
            }
        );
    }
    
    explicit option(T&& that) noexcept(std::is_nothrow_move_constructible_v<T>)
        : storage(),
          emptyFlag(false)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_move_constructible_v<T>,
            noexcept(std::is_nothrow_move_constructible_v<T>) {
                construct(std::move(that));
            },
            noexcept {
                static_assert(false, "Optional type should be move constructible");
            }
        );
    }
    
    option(const option& that) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : storage(),
          emptyFlag(that.emptyFlag)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_copy_constructible_v<T>,
            noexcept(std::is_nothrow_copy_constructible_v<T>) {
                if (that.non_empty()) construct(that.value());
            },
            noexcept {
                static_assert(false, "Optional type should be copy constructible");
            }
        );
    }
    
    option(option&& that) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
        : storage(),
          emptyFlag(that.emptyFlag)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_move_constructible_v<T>,
            noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>) {
                if (that.non_empty())
                {
                    construct(std::move(that).value());
                    that.destroy();
                    that.emptyFlag = true;
                }
            },
            noexcept {
                static_assert(false, "Optional type should be move constructible");
            }
        );
    }
    
    template <typename... Args>
    explicit option(const in_place&, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : storage(),
          emptyFlag(false)
    {
        // NOTE: workaround for msvc
        using types = type_list<Args...>;

// NOTE: msvc 2015-2017RC can't access both Ts... and Arg in noexcept declaration
#ifdef CPPSTREAM_MSVC
#   define CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND
#else
#   define CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND noexcept(std::is_nothrow_constructible_v<T, Ts...>)
#endif

        CPPSTREAM_CONSTEXPR_IFELSE((std::is_constructible_v<T, Args...>),
            CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND {
                construct(types(), args...);
            },
            noexcept {
                static_assert(false, "Optional type should be constructible from 'Ts&&...'");
            }
        );

#undef CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND
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

    option& operator= (const option& that) noexcept(std::is_nothrow_copy_assignable_v<T>    &&
                                                    std::is_nothrow_copy_constructible_v<T> &&
                                                    std::is_nothrow_destructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>,
            noexcept(std::is_nothrow_copy_assignable_v<T>    &&
                     std::is_nothrow_copy_constructible_v<T> &&
                     std::is_nothrow_destructible_v<T>)
            {
                if (!(empty() && that.empty()))
                {
                    if (non_empty() && that.non_empty())   assign(that.value());
                    else if (empty() && that.non_empty())  initialize(that.value());
                    else                                   reset();
                }
            },
            noexcept
            {
                static_assert(false, "Optional type should be copy constructible and copy assignable");
            }
        );
    
        return *this;
    }

    option& operator= (option&& that) noexcept(std::is_nothrow_move_assignable_v<T>    &&
                                               std::is_nothrow_move_constructible_v<T> &&
                                               std::is_nothrow_destructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>,
            noexcept(std::is_nothrow_move_assignable_v<T>    &&
                     std::is_nothrow_move_constructible_v<T> &&
                     std::is_nothrow_destructible_v<T>)
            {
                if (!(empty() && that.empty()))
                {
                    if (non_empty() && that.non_empty())   assign(std::move(that).value());
                    else if (empty() && that.non_empty())  initialize(std::move(that).value());
                    else                                   reset();
                }
            },
            noexcept
            {
                static_assert(false, "Optional type should be move constructible and move assignable");
            }
        );

        return *this;
    }

    option& operator= (const T& that) noexcept(std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_copy_constructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>,
            noexcept(std::is_nothrow_copy_assignable_v<T> &&
                     std::is_nothrow_copy_constructible_v<T>)
            {
                if (empty()) initialize(that);
                else         assign(that);
            },
            noexcept
            {
                static_assert(false, "Optional type should be copy constructible and copy assignable");
            }
        );
    
        return *this;
    }
    
    option& operator= (T&& that) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>,
            noexcept(std::is_nothrow_move_assignable_v<T> &&
                     std::is_nothrow_move_constructible_v<T>)
            {
                if (empty()) initialize(std::move(that));
                else         assign(std::move(that));
            },
            noexcept
            {
                static_assert(false, "Optional type should be move constructible and move assignable");
            }
        );
    
        return *this;
    }
    
    void reset() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if (non_empty())
        {
            destroy();
            emptyFlag = true;
        }
    }
    
    bool operator== (const none_t&) const noexcept
    {
        return empty();
    }
    
    bool operator!= (const none_t&) const noexcept
    {
        return non_empty();
    }
    
    bool operator== (const option& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return CPPSTREAM_CONSTEXPR_IFELSE(is_comparable_v<const T>,
            noexcept(is_nothrow_comparable_v<const T>) {
                return (empty() == that.empty()) && (empty() ? true : (value() == that.value()));
            },
            noexcept {
                static_assert(false, "Optional type should be comparable");
                return false;
            }
        );
    }
    
    bool operator!= (const option& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(*this == that);
    }
    
    bool operator== (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return CPPSTREAM_CONSTEXPR_IFELSE(is_comparable_v<const T>,
            noexcept(is_nothrow_comparable_v<const T>) {
                return empty() ? false : value() == that;
            },
            noexcept {
                static_assert(false, "Optional type should be comparable");
                return true;
            }
        );
    }
    
    bool operator!= (const T& that) const noexcept(is_nothrow_comparable_v<const T>)
    {
        return !(*this == that);
    }
    
    size_t size() const noexcept
    {
        return empty() ? size_t(0) : size_t(1);
    }
    
    bool empty() const noexcept { return emptyFlag; }
    bool non_empty() const noexcept { return !emptyFlag; }
    
    const T& get() const & noexcept
    {
        assert(non_empty());
        return value();
    }
    
    T& get() & noexcept
    {
        assert(non_empty());
        return value();
    }
    
    T get() && noexcept
    {
        assert(non_empty());
        return std::move(value());
    }
    
    const T& get_or_else(const T& $default) const noexcept
    {
        return empty() ? $default : value();
    }
    
    T& get_or_else(T& $default) noexcept
    {
        return empty() ? $default : value();
    }
    
    T get_or_else(T&& $default) && noexcept
    {
        return empty() ? $default : std::move(value());
    }
    
    template <typename E>
    const T& get_or_throw() const &
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty()) throw E{};
            },
            noexcept {
                static_assert(false, "Exception type should be default constructible");
            }
        );
    
        return value();
    }
    
    template <typename E>
    T& get_or_throw() &
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty()) throw E{};
            },
            noexcept {
                static_assert(false, "Exception type should be default constructible");
            }
        );
    
        return value();
    }
    
    template <typename E>
    T get_or_throw() &&
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty()) throw E{};
            },
            noexcept {
                static_assert(false, "Exception type should be default constructible");
            }
        );
    
        return std::move(value());
    }
    
    template <typename E>
    const T& get_or_throw(E&& exception) const &
    {
        if (empty()) throw std::forward<E>(exception);
        return value();
    }
    
    template <typename E>
    T& get_or_throw(E&& exception) &
    {
        if (empty()) throw std::forward<E>(exception);
        return value();
    }
    
    template <typename E>
    T get_or_throw(E&& exception) const &&
    {
        if (empty()) throw std::forward<E>(exception);
        return std::move(value());
    }

    void swap(option& that) noexcept(std::is_nothrow_swappable_v<T&>         &&
                                     std::is_nothrow_move_constructible_v<T> &&
                                     std::is_nothrow_destructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_swappable_v<T&> && std::is_move_constructible_v<T>,
            noexcept(std::is_nothrow_swappable_v<T&>         &&
                     std::is_nothrow_move_constructible_v<T> &&
                     std::is_nothrow_destructible_v<T>)
            {
                using std::swap;

                if (!(empty() && that.empty()))
                {
                    if (non_empty() && that.non_empty()) swap(value(), that.value());
                    else if (empty())
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
            },
            noexcept
            {
                static_assert(false, "Optional type should be swapable and move constructible");
            }
        );
    }

    void swap(T& that) noexcept(std::is_nothrow_swappable_v<T&> && std::is_nothrow_move_constructible_v<T>)
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_swappable_v<T&> && std::is_move_constructible_v<T>,
            noexcept(std::is_nothrow_swappable_v<T&> &&
                std::is_nothrow_move_constructible_v<T>)
            {
                using std::swap;
    
                if (empty()) initialize(std::move(that));
                else         swap(value(), that);
            },
            noexcept
            {
                static_assert(false, "Optional type should be swapable and move constructible");
            }
        );
    }
    
    explicit operator bool() const noexcept
    {
        return non_empty();
    }

#pragma region iterators

    iterator begin() noexcept
    {
        return iterator(pointer_or_null());
    }
    
    iterator end() noexcept
    {
        return iterator();
    }
    
    const_iterator begin() const noexcept
    {
        return const_iterator(pointer_or_null());
    }
    
    const_iterator end() const noexcept
    {
        return const_iterator();
    }
    
    const_iterator cbegin() noexcept
    {
        return const_iterator(pointer_or_null());
    }
    
    const_iterator cend() noexcept
    {
        return const_iterator();
    }
    
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(pointer_or_null());
    }
    
    reverse_iterator rend() noexcept
    {
        return reverse_iterator();
    }
    
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(pointer_or_null());
    }
    
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator();
    }
    
    const_reverse_iterator crbegin() noexcept
    {
        return const_reverse_iterator(pointer_or_null());
    }
    
    const_reverse_iterator crend() noexcept
    {
        return const_reverse_iterator();
    }

#pragma endregion

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
        return empty() ? nullptr : pointer();
    }
    
    T* pointer_or_null() noexcept
    {
        return empty() ? nullptr : pointer();
    }
    
    const T& value() const noexcept
    {
        return *pointer();
    }
    
    T& value() noexcept
    {
        return *pointer();
    }
    
    template <typename... Ts>
    void construct(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts...>)
    {
        new (pointer()) T(std::forward<Ts>(args)...);
    }

    template <typename... Ts, typename... Us>
    void construct(const type_list<Ts...>, Us&... args) noexcept(std::is_nothrow_constructible_v<T, Ts...>)
    {
        construct(std::forward<Ts>(args)...);
    }
    
    template <typename... Ts>
    void initialize(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts...>)
    {
        construct(std::forward<Ts>(args)...);
        emptyFlag = false;
    }
    
    template <typename U>
    void assign(U&& object) noexcept(std::is_nothrow_assignable_v<T, U>)
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
    
    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    bool emptyFlag;
};

template <typename T>
class option<T*> final
{
public:

    using value_type = T*;
    using const_value_type = std::add_pointer_t<std::add_const_t<T>>;
    using iterator = option_iterator<T*>;
    using const_iterator = const_option_iterator<T*>;
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;

    option() noexcept
        : pointer(nullptr),
          empty(true)
    {
    }

    explicit option(value_type pointer) noexcept
        : pointer(pointer),
          empty(false)
    {
    }

    option(const option&) noexcept = default;
    option(option&&) noexcept = default;

    option& operator= (const option&) noexcept = default;
    option& operator= (option&&) noexcept = default;

    ~option() noexcept = default;

    bool operator== (const option& that) const noexcept
    {
        return (pointer == that.pointer) && (empty == that.empty);
    }

    bool operator!= (const option& that) const noexcept
    {
        return !(*this == that);
    }

    bool operator== (const T* const that) const noexcept
    {
        return empty ? false : pointer == that;
    }

    bool operator!= (const T* const that) const noexcept
    {
        return !(*this == that);
    }

    size_t size() const noexcept
    {
        return empty ? size_t(0) : size_t(1);
    }

    bool is_empty() const noexcept { return empty; }
    bool non_empty() const noexcept { return !empty; }

    void reset() noexcept { empty = true; }

    value_type get() noexcept
    {
        assert(non_empty());
        return pointer;
    }

    const_value_type get() const noexcept
    {
        assert(non_empty());
        return pointer;
    }

    value_type get_or_else(value_type $default) noexcept
    {
        return empty ? $default : pointer;
    }

    const_value_type get_or_else(const_value_type $default) const noexcept
    {
        return empty ? $default : pointer;
    }

    template <typename E>
    value_type get_or_throw()
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty) throw E{};
            },
            noexcept {
                static_assert(false, "E should be default constructible");
            }
        );

        return pointer;
    }

    template <typename E>
    const_value_type get_or_throw() const
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty) throw E{};
            },
            noexcept {
                static_assert(false, "E should be default constructible");
            }
        );

        return pointer;
    }

    template <typename E>
    value_type get_or_throw(E&& exception)
    {
        if (empty) throw std::forward<E>(exception);
        return pointer;
    }

    template <typename E>
    const_value_type get_or_throw(E&& exception) const
    {
        if (empty) throw std::forward<E>(exception);
        return pointer;
    }

    void swap(option& that) noexcept
    {
        std::swap(pointer, that.pointer);
        std::swap(empty, that.empty);
    }

    iterator begin() noexcept
    {
        return iterator(pointer);
    }

    explicit operator bool() const noexcept
    {
        return non_empty();
    }

#pragma region iterators

    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator cend() noexcept
    {
        return const_iterator(nullptr);
    }

    iterator rbegin() noexcept
    {
        return iterator(pointer);
    }

    iterator rend() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator rbegin() const noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator rend() const noexcept
    {
        return const_iterator(nullptr);
    }

    const_iterator crbegin() noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator crend() noexcept
    {
        return const_iterator(nullptr);
    }

#pragma endregion

private:

    value_type pointer;
    bool empty;
};

template <typename T>
class option<T&> final
{
public:

    using value_type = T&;
    using const_value_type = std::add_rvalue_reference_t<std::add_const_t<T>>;
    using iterator = option_iterator<T>;
    using const_iterator = const_option_iterator<T>;
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;

    option() noexcept
        : pointer(nullptr)
    {
    }

    option(const none_t&) noexcept
        : option()
    {
    }

    option(value_type reference) noexcept
        : pointer(&reference)
    {
    }

    option(const option& that) noexcept
        : pointer(that.pointer)
    {
    }

    option(option&& that) noexcept
        : pointer(that.pointer)
    {
        that.pointer = nullptr;
    }

    ~option() noexcept = default;

    option& operator= (const option& that) noexcept
    {
        pointer = that.pointer;
        return *this;
    }

    option& operator= (option&& that) noexcept
    {
        pointer = that.pointer;
        that.pointer = nullptr;
        return *this;
    }

    void reset() noexcept
    {
        pointer = nullptr;
    }

    bool operator== (const none_t) const noexcept
    {
        return pointer == nullptr;
    }

    bool operator!= (const none_t) const noexcept
    {
        return pointer != nullptr;
    }

    bool operator== (const option& that) const noexcept(is_nothrow_comparable_v<T>)
    {
        return CPPSTREAM_CONSTEXPR_IFELSE(is_comparable_v<T>,
            noexcept(is_nothrow_comparable_v<T>) {
                return non_empty() && that.non_empty() && (reference() == that.reference());
            },
            noexcept {
                static_assert(false, "Optional type should be comparable");
                return false;
            }
        );
    }

    bool operator!= (const option& that) const noexcept(is_nothrow_comparable_v<T>)
    {
        return !(*this == that);
    }

    bool operator== (const T& that) noexcept(is_nothrow_comparable_v<T>)
    {
        return CPPSTREAM_CONSTEXPR_IFELSE(is_comparable_v<T>,
            noexcept(is_nothrow_comparable_v<T>) {
                return non_empty() && (reference() == that);
            },
            noexcept {
                static_assert(false, "Optional type should be comparable");
                return false;
            }
        );
    }

    bool operator!= (const T& that) const noexcept(is_nothrow_comparable_v<T>)
    {
        return !(*this == that);
    }

    size_t size() const noexcept
    {
        return empty() ? 0 : 1;
    }

    bool empty() const noexcept
    {
        return pointer == nullptr;
    }

    bool non_empty() const noexcept
    {
        return pointer != nullptr;
    }

    value_type get() noexcept
    {
        assert(non_empty());
        return reference();
    }

    const_value_type get() const noexcept
    {
        assert(non_empty());
        return reference();
    }

    value_type get_or_else(value_type $default) noexcept
    {
        return empty ? $default : reference();
    }

    const_value_type get_or_else(const_value_type $default) const noexcept
    {
        return empty ? $default : reference();
    }

    template <typename E>
    value_type get_or_throw()
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty) throw E{};
            },
            noexcept {
                static_assert(false, "E should be default constructible");
            }
        );

        return reference();
    }

    template <typename E>
    const_value_type get_or_throw() const
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::is_default_constructible_v<E>,
            {
                if (empty) throw E{};
            },
            noexcept{
                static_assert(false, "E should be default constructible");
            }
        );

        return reference();
    }

    template <typename E>
    value_type get_or_throw(E&& exception)
    {
        if (empty) throw std::forward<E>(exception);
        return reference();
    }

    template <typename E>
    const_value_type get_or_throw(E&& exception) const
    {
        if (empty) throw std::forward<E>(exception);
        return reference();
    }

    void swap(option& that) noexcept
    {
        std::swap(pointer, that.pointer);
    }

    explicit operator bool() const noexcept
    {
        return non_empty();
    }

#pragma region iterators

    iterator begin() noexcept
    {
        return iterator(pointer);
    }

    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator cend() noexcept
    {
        return const_iterator(nullptr);
    }

    iterator rbegin() noexcept
    {
        return iterator(pointer);
    }

    iterator rend() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator rbegin() const noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator rend() const noexcept
    {
        return const_iterator(nullptr);
    }

    const_iterator crbegin() noexcept
    {
        return const_iterator(pointer);
    }

    const_iterator crend() noexcept
    {
        return const_iterator(nullptr);
    }

#pragma endregion

private:

    value_type reference() noexcept
    {
        return *pointer;
    }

    const_value_type reference() const noexcept
    {
        return *pointer;
    }

    T* pointer;
};

/*template <typename T>
class option<not_null<T>> final
{
    using pointer_t = std::remove_const_t<T>;
public:

    using value_type = not_null<T>;
    using const_value_type = std::add_const_t<value_type>;

    /*using iterator = option_iterator<T>;
    using const_iterator = const_option_iterator<T>;
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;#1#

    option() noexcept(std::is_nothrow_constructible_v<pointer_t, nullptr_t>)
        : pointer(nullptr)
    {
    }

private:

    pointer_t pointer;
};*/

template <typename T>
option<remove_cvr_t<T>> some(T&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<T>, T>)
{
    return option<remove_cvr_t<T>>(std::forward<T>(value));
}

template <typename T>
option<T> someRef(T& reference) noexcept
{
    return option<T>(reference);
}

CPPSTREAM_FORCEINLINE constexpr none_t none() noexcept
{
    return none_t();
}

template <typename T, typename... Ts>
option<T> make_option(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T, Ts...>)
{
    return option<T>(in_place{}, std::forward<Ts>(args)...);
}

template <typename T>
struct ordering_traits<option<T>>
{
    static constexpr bool is_ordered = true;
};

} // cppstream namespace

namespace std {

template <typename T>
void swap(cppstream::option<T>& a, cppstream::option<T>& b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

template <typename T>
void swap(cppstream::option<T>& option, T& value) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
void swap(T& value, cppstream::option<T>& option) noexcept(noexcept(option.swap(value)))
{
    option.swap(value);
}

template <typename T>
struct hash<cppstream::option<T>>
{
    size_t operator()(const cppstream::option<T>& option) const noexcept
    {
        if (option.empty()) return 0;

        hash<T> valueHash;
        return valueHash(option.value());
    }
};

} // std namespace
