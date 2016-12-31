#pragma once

#include "detail/type_traits.hpp"
#include "detail/type_list.hpp"
#include "constexpr_if.hpp"

namespace cppstream {

template <typename... Ts>
class variant final
{
    using types = type_list<Ts...>;

    static_assert(sizeof...(Ts) != 0, "Empty type list not allowed.");
    static_assert(!std::conjunction_v<std::is_const<Ts>...>, "All variant types should not be const");
    static_assert(!std::conjunction_v<std::is_volatile<Ts>...>, "All variant types should not be volatile");
    static_assert(!std::conjunction_v<std::is_reference<Ts>...>, "All variant types should not be a reference");
    static_assert(!std::conjunction_v<std::is_void<Ts>...>, "Variant can't hold a value of 'void' type");
    static_assert(!std::conjunction_v<std::is_array<Ts>...>, "Variant can't hold a value of array type");
public:

    template <typename T>
    explicit variant(type_t<T>) noexcept(std::is_nothrow_default_constructible_v<T>)
        : index_(types::index_of(type<T>)),
          storage()
    {
        CPPSTREAM_CONSTEXPR_IFELSE(types::contains(type<T>),
            noexcept(std::is_nothrow_default_constructible_v<T>) {
                construct<T>();
            },
            noexcept {
                static_assert(false, "Invalid variant type");
            }
        );
    }

    template <typename T>
    variant(T&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<T>, T>)
        : index_(types::index_of(type<T>)),
          storage()
    {
        using value_type = remove_cvr_t<T>;

        CPPSTREAM_CONSTEXPR_IFELSE(types::contains(type<T>),
            noexcept(std::is_nothrow_constructible_v<value_type, T>) {
                construct<value_type>(std::forward<T>(value));
            },
            noexcept {
                static_assert(false, "Invalid variant type");
            }
        );
    }

    template <typename T, typename... Args>
    variant(in_place_type<T>, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : index_(types::index_of(type<T>)),
          storage()
    {
        using arguments = type_list<Args...>;

#ifdef CPPSTREAM_MSVC
#   define CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND
#else
#   define CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND noexcept(std::is_nothrow_constructible_v<T, Args...>)
#endif

        CPPSTREAM_CONSTEXPR_IFELSE(types::contains(type<T>),
            CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND {
                construct<T>(arguments(), args...);
            },
            noexcept {
                static_assert(false_v<T>, "Invalid variant type");
            }
        );
    }

#undef CPPSTREAM_MSVC_NOEXCEPT_WORKAROUND

    variant(const variant& that) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
        : index_(that.index_),
          storage()
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::conjunction_v<std::is_copy_constructible<Ts>...>,
            noexcept {

            },
            noexcept {
                static_assert(false, "All variant types should be copy constructible.");
            }
        );
    }

    variant(variant&& that) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>)
        : index_(that.index_),
          storage()
    {
        CPPSTREAM_CONSTEXPR_IFELSE(std::conjunction_v<std::is_move_constructible<Ts>...>,
            noexcept {

            },
            noexcept {
                static_assert(false, "All variant types should be move constructible.");
            }
        );
    }

    // TODO: copy and move assign

    size_t index() const noexcept
    {
        return index_;
    }

private:

    template <typename T>
    T* pointer() noexcept
    {
        return reinterpret_cast<T*>(&storage);
    }

    template <typename T>
    const T* pointer() const noexcept
    {
        return reinterpret_cast<T*>(&storage);
    }

    template <typename T, typename... Args>
    void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        new (pointer<T>()) T(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args, typename... Us>
    void construct(const type_list<Args...>, Us&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        return construct<T, Args...>(std::forward<Args>(args)...);
    }

    size_t index_;
    std::aligned_union_t<1, Ts...> storage;
};

} // cppstream namespace
