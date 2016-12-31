#pragma once

#include "config.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <cassert>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace std {

template <typename T, typename Deleter>
class unique_ptr;

template <typename T>
class shared_ptr;

template <typename T>
class weak_ptr;

} // std namespace

namespace cppstream {

/*template <typename T>
struct not_null_traits
{
    static constexpr bool exists = false;
};

template <typename T>
struct not_null_traits<T*>
{
    static constexpr bool exists = true;
    static constexpr T* get(T* pointer) noexcept { return pointer; }
};

template <typename T, typename Deleter>
struct not_null_traits<std::unique_ptr<T, Deleter>>
{
private:
    using pointer_t = std::unique_ptr<T, Deleter>;
public:

    static constexpr bool exists = true;
    static typename pointer_t::pointer get(const pointer_t& pointer) noexcept { return pointer.get(); }
};

template <typename T>
struct not_null_traits<std::shared_ptr<T>>
{
private:
    using pointer_t = std::shared_ptr<T>;
public:

    static constexpr bool exists = true;
    static typename pointer_t::pointer get(const pointer_t& pointer) noexcept { return pointer.get(); }
};*/

template <typename T>
class not_null;

template <typename T>
class not_null<T*> final
{
public:

    using pointer = T*;
    using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
    using element_type = T;
    using const_element_type = std::add_const_t<T>;
    using element_reference = std::add_lvalue_reference_t<element_type>;
    using const_element_reference = std::add_lvalue_reference_t<const_element_type>;

    explicit not_null(pointer ptr) noexcept
        : ptr(ptr)
    {
        assert(ptr != nullptr);
    }

    element_reference operator* () noexcept
    {
        return *ptr;
    }

    const_element_reference operator* () const noexcept
    {
        return *ptr;
    }

    pointer operator-> () noexcept
    {
        return ptr;
    }

    const_pointer operator-> () const noexcept
    {
        return ptr;
    }

    pointer get() noexcept
    {
        return ptr;
    }

    const_pointer get() const noexcept
    {
        return ptr;
    }

    void swap(not_null& that) noexcept
    {
        const auto tmp = that.ptr;
        that.ptr = ptr;
        ptr = tmp;
    }

#pragma region comparison

    bool operator== (const not_null& that) const noexcept
    {
        return ptr == that.ptr;
    }

    bool operator!= (const not_null& that) const noexcept
    {
        return ptr != that.ptr;
    }

    bool operator> (const not_null& that) const noexcept
    {
        return ptr > that.ptr;
    }

    bool operator>= (const not_null& that) const noexcept
    {
        return ptr >= that.ptr;
    }

    bool operator< (const not_null& that) const noexcept
    {
        return ptr < that.ptr;
    }

    bool operator<= (const not_null& that) const noexcept
    {
        return ptr <= that.ptr;
    }

    bool operator== (const_pointer that) const noexcept
    {
        return ptr == that;
    }

    bool operator!= (const_pointer that) const noexcept
    {
        return ptr != that;
    }

    bool operator> (const_pointer that) const noexcept
    {
        return ptr > that;
    }

    bool operator>= (const_pointer that) const noexcept
    {
        return ptr >= that;
    }

    bool operator< (const_pointer that) const noexcept
    {
        return ptr < that;
    }

    bool operator<= (const_pointer that) const noexcept
    {
        return ptr <= that;
    }

    bool operator== (nullptr_t) const noexcept
    {
        return false;
    }

    bool operator!= (nullptr_t) const noexcept
    {
        return true;
    }

#pragma endregion

    explicit operator pointer() noexcept
    {
        return ptr;
    }

    explicit operator const const_pointer() const noexcept
    {
        return ptr;
    }

    explicit operator bool() const noexcept
    {
        return true;
    }

private:

    pointer ptr;
};

template <typename T, typename Deleter>
class not_null<std::unique_ptr<T, Deleter>>
{
public:

    using smart_pointer = std::unique_ptr<T, Deleter>;
    using pointer = std::add_pointer_t<T>;
    using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
    using element_type = T;
    using const_element_type = std::add_const_t<element_type>;
    using element_reference = std::add_lvalue_reference_t<T>;
    using const_element_reference = std::add_const_t<element_reference>;

    explicit not_null(smart_pointer&& ptr) noexcept
        : ptr(std::move(ptr))
    {
        assert(this->ptr != nullptr);
    }

    not_null(not_null&&) noexcept = default;
    not_null& operator= (not_null&&) noexcept = default;

    not_null(const not_null&) = delete;
    not_null& operator= (const not_null&) = delete;

    ~not_null() noexcept(std::is_nothrow_destructible_v<T>) = default;

    element_reference operator* () noexcept
    {
        return *ptr;
    }

    const_element_reference operator* () const noexcept
    {
        return *ptr;
    }

    pointer operator-> () noexcept
    {
        return ptr.get();
    }

    const_pointer operator-> () const noexcept
    {
        return get();
    }

    pointer get() noexcept
    {
        return get();
    }

    const_pointer get() const noexcept
    {
        return ptr.get();
    }

    std::add_lvalue_reference_t<Deleter> get_deleter() noexcept
    {
        return ptr.get_deleter();
    }

    std::add_lvalue_reference_t<std::add_const_t<Deleter>> get_deleter() const noexcept
    {
        return ptr.get_deleter();
    }

    void swap(not_null& that) noexcept
    {
        ptr.swap(that.ptr);
    }

#pragma region comparison

    bool operator== (const not_null& that) const noexcept
    {
        return ptr == that.ptr;
    }

    bool operator!= (const not_null& that) const noexcept
    {
        return ptr != that.ptr;
    }

    bool operator> (const not_null& that) const noexcept
    {
        return ptr > that.ptr;
    }

    bool operator>= (const not_null& that) const noexcept
    {
        return ptr >= that.ptr;
    }

    bool operator< (const not_null& that) const noexcept
    {
        return ptr < that.ptr;
    }

    bool operator<= (const not_null& that) const noexcept
    {
        return ptr <= that.ptr;
    }

    bool operator== (const_pointer that) const noexcept
    {
        return ptr.get() == that;
    }

    bool operator!= (const_pointer that) const noexcept
    {
        return ptr.get() != that;
    }

    bool operator> (const_pointer that) const noexcept
    {
        return ptr.get() > that;
    }

    bool operator>= (const_pointer that) const noexcept
    {
        return ptr.get() >= that;
    }

    bool operator< (const_pointer that) const noexcept
    {
        return ptr.get() < that;
    }

    bool operator<= (const_pointer that) const noexcept
    {
        return ptr.get() <= that;
    }

    bool operator== (const smart_pointer& that) const noexcept
    {
        return ptr == that;
    }

    bool operator!= (const smart_pointer& that) const noexcept
    {
        return ptr != that;
    }

    bool operator> (const smart_pointer& that) const noexcept
    {
        return ptr > that;
    }

    bool operator>= (const smart_pointer& that) const noexcept
    {
        return ptr >= that;
    }

    bool operator< (const smart_pointer& that) const noexcept
    {
        return ptr < that;
    }

    bool operator<= (const smart_pointer& that) const noexcept
    {
        return ptr <= that;
    }

    bool operator== (nullptr_t) const noexcept
    {
        return false;
    }

    bool operator!= (nullptr_t) const noexcept
    {
        return true;
    }

#pragma endregion

    explicit operator smart_pointer() && noexcept
    {
        return std::move(ptr);
    }

    explicit operator bool() const noexcept
    {
        return true;
    }

private:

    smart_pointer ptr;
};

template <typename T>
class not_null<std::shared_ptr<T>>
{
public:

    using smart_pointer = std::shared_ptr<T>;
    using pointer = std::add_pointer_t<T>;
    using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
    using element_type = T;
    using const_element_type = std::add_const_t<element_type>;
    using element_reference = std::add_lvalue_reference_t<T>;
    using const_element_reference = std::add_const_t<element_reference>;

    explicit not_null(const smart_pointer& ptr) noexcept
        : ptr(ptr)
    {
        assert(ptr != nullptr);
    }

    explicit not_null(smart_pointer&& ptr) noexcept
        : ptr(std::move(ptr))
    {
        assert(this->ptr != nullptr);
    }

    ~not_null() noexcept(std::is_nothrow_destructible_v<T>) = default;

    element_reference operator* () noexcept
    {
        return *ptr;
    }

    const_element_reference operator* () const noexcept
    {
        return *ptr;
    }

    pointer operator-> () noexcept
    {
        return get();
    }

    const_pointer operator-> () const noexcept
    {
        return get();
    }

    void swap(not_null& that) noexcept
    {
        ptr.swap(that.ptr);
    }

    pointer get() noexcept
    {
        return ptr.get();
    }

    const_pointer get() const noexcept
    {
        return ptr.get();
    }

    long use_count() const noexcept
    {
        return ptr.use_count();
    }

    template <typename U>
    bool owner_before(const std::shared_ptr<U>& that) const
    {
        return ptr.owner_before(that);
    }

    template <typename U>
    bool owner_before(const std::weak_ptr<U>& that) const
    {
        return ptr.owner_before(that);
    }

#pragma region comparison

    bool operator== (const not_null& that) const noexcept
    {
        return ptr == that.ptr;
    }

    bool operator!= (const not_null& that) const noexcept
    {
        return ptr != that.ptr;
    }

    bool operator> (const not_null& that) const noexcept
    {
        return ptr > that.ptr;
    }

    bool operator>= (const not_null& that) const noexcept
    {
        return ptr >= that.ptr;
    }

    bool operator< (const not_null& that) const noexcept
    {
        return ptr < that.ptr;
    }

    bool operator<= (const not_null& that) const noexcept
    {
        return ptr <= that.ptr;
    }

    bool operator== (const_pointer that) const noexcept
    {
        return ptr.get() == that;
    }

    bool operator!= (const_pointer that) const noexcept
    {
        return ptr.get() != that;
    }

    bool operator> (const_pointer that) const noexcept
    {
        return ptr.get() > that;
    }

    bool operator>= (const_pointer that) const noexcept
    {
        return ptr.get() >= that;
    }

    bool operator< (const_pointer that) const noexcept
    {
        return ptr.get() < that;
    }

    bool operator<= (const_pointer that) const noexcept
    {
        return ptr.get() <= that;
    }

    bool operator== (const smart_pointer& that) const noexcept
    {
        return ptr == that;
    }

    bool operator!= (const smart_pointer& that) const noexcept
    {
        return ptr != that;
    }

    bool operator> (const smart_pointer& that) const noexcept
    {
        return ptr > that;
    }

    bool operator>= (const smart_pointer& that) const noexcept
    {
        return ptr >= that;
    }

    bool operator< (const smart_pointer& that) const noexcept
    {
        return ptr < that;
    }

    bool operator<= (const smart_pointer& that) const noexcept
    {
        return ptr <= that;
    }

    bool operator== (nullptr_t) const noexcept
    {
        return false;
    }

    bool operator!= (nullptr_t) const noexcept
    {
        return true;
    }

#pragma endregion

    explicit operator smart_pointer() & noexcept
    {
        return ptr;
    }

    explicit operator smart_pointer() && noexcept
    {
        return std::move(ptr);
    }

    explicit operator bool() const noexcept
    {
        return true;
    }

private:

    smart_pointer ptr;
};

} // cppstream namespace

#pragma region comparison

// raw pointer

template <typename T>
bool operator== (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b == a;
}

template <typename T>
bool operator!= (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b != a;
}

template <typename T>
bool operator> (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b < a;
}

template <typename T>
bool operator>= (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b <= a;
}

template <typename T>
bool operator< (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b > a;
}

template <typename T>
bool operator<= (const T* a, const cppstream::not_null<T*>& b) noexcept
{
    return b >= a;
}

template <typename T>
bool operator== (nullptr_t, const cppstream::not_null<T*>&) noexcept
{
    return false;
}

template <typename T>
bool operator!= (nullptr_t, const cppstream::not_null<T*>&) noexcept
{
    return true;
}

// unique pointer

template <typename T, typename Deleter>
bool operator== (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b == a;
}

template <typename T, typename Deleter>
bool operator!= (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b != a;
}

template <typename T, typename Deleter>
bool operator> (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b < a;
}

template <typename T, typename Deleter>
bool operator>= (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b <= a;
}

template <typename T, typename Deleter>
bool operator< (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b > a;
}

template <typename T, typename Deleter>
bool operator<= (const T* a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b >= a;
}

template <typename T, typename Deleter>
bool operator== (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b == a;
}

template <typename T, typename Deleter>
bool operator!= (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b != a;
}

template <typename T, typename Deleter>
bool operator> (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b < a;
}

template <typename T, typename Deleter>
bool operator>= (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b <= a;
}

template <typename T, typename Deleter>
bool operator< (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b > a;
}

template <typename T, typename Deleter>
bool operator<= (const std::unique_ptr<T, Deleter>& a, const cppstream::not_null<std::unique_ptr<T, Deleter>>& b) noexcept
{
    return b >= a;
}

template <typename T, typename Deleter>
bool operator== (nullptr_t, const cppstream::not_null<std::unique_ptr<T, Deleter>>&) noexcept
{
    return false;
}

template <typename T, typename Deleter>
bool operator!= (nullptr_t, const cppstream::not_null<std::unique_ptr<T, Deleter>>&) noexcept
{
    return true;
}

// shared pointer

template <typename T>
bool operator== (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b == a;
}

template <typename T>
bool operator!= (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b != a;
}

template <typename T>
bool operator> (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b < a;
}

template <typename T>
bool operator>= (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b <= a;
}

template <typename T>
bool operator< (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b > a;
}

template <typename T>
bool operator<= (const T* a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b >= a;
}

template <typename T>
bool operator== (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b == a;
}

template <typename T>
bool operator!= (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b != a;
}

template <typename T>
bool operator> (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b < a;
}

template <typename T>
bool operator>= (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b <= a;
}

template <typename T>
bool operator< (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b > a;
}

template <typename T>
bool operator<= (const std::shared_ptr<T>& a, const cppstream::not_null<std::shared_ptr<T>>& b) noexcept
{
    return b >= a;
}

template <typename T>
bool operator== (nullptr_t, const cppstream::not_null<std::shared_ptr<T>>&) noexcept
{
    return false;
}

template <typename T>
bool operator!= (nullptr_t, const cppstream::not_null<std::shared_ptr<T>>&) noexcept
{
    return true;
}

#pragma endregion

namespace std {

template <typename T>
struct hash;

template <typename T>
void swap(cppstream::not_null<T*>& a, cppstream::not_null<T*>& b) noexcept
{
    a.swap(b);
}

template <typename T, typename Deleter>
void swap(cppstream::not_null<unique_ptr<T, Deleter>>& a, cppstream::not_null<unique_ptr<T, Deleter>>& b) noexcept
{
    a.swap(b);
}

template <typename T>
void swap(cppstream::not_null<shared_ptr<T>>& a, cppstream::not_null<shared_ptr<T>>& b) noexcept
{
    a.swap(b);
}

template <typename T>
struct hash<cppstream::not_null<T*>>
{
    size_t operator() (const cppstream::not_null<T*>& pointer) const noexcept
    {
        return std::hash<T*>()(pointer);
    }
};

template <typename T, typename Deleter>
struct hash<cppstream::not_null<unique_ptr<T, Deleter>>>
{
    size_t operator() (const cppstream::not_null<unique_ptr<T, Deleter>>& pointer) const noexcept
    {
        return std::hash<T*>()(pointer.get());
    }
};

template <typename T>
struct hash<cppstream::not_null<shared_ptr<T>>>
{
    size_t operator() (const cppstream::not_null<shared_ptr<T>>& pointer) const noexcept
    {
        return std::hash<T*>()(pointer.get());
    }
};

} // std namespace
