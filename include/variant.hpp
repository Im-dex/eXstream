#pragma once

#include "option.hpp"
#include "detail/type_list.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <exception>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

constexpr size_t variant_npos = size_t(-1);

class bad_variant_access : public std::exception {};

namespace detail {

template <typename... Ts>
struct variant_helper;

template<typename T, typename... Ts>
struct variant_helper<T, Ts...> final
{
    CPPSTREAM_FORCEINLINE
    static void destroy(const size_t index, void* ptr) noexcept(std::is_nothrow_destructible_v<T> && std::conjunction_v<std::is_nothrow_destructible<Ts>...>)
    {
        if (index == sizeof...(Ts))
        {
            static_cast<T*>(ptr)->~T();
        }
        else
        {
            variant_helper<Ts...>::destroy(index, ptr);
        }
    }

    CPPSTREAM_FORCEINLINE
    static void copy(const size_t index, const void* from, void* to) noexcept(std::is_nothrow_copy_constructible_v<T> && std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    {
        if (index == sizeof...(Ts))
        {
            new (static_cast<T*>(to)) T(*static_cast<const T*>(from));
        }
        else
        {
            variant_helper<Ts...>::copy(index, from, to);
        }
    }

    CPPSTREAM_FORCEINLINE
    static void move(const size_t index, void* from, void* to) noexcept(std::is_nothrow_move_constructible_v<T> && std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>)
    {
        if (index == sizeof...(Ts))
        {
            new (static_cast<T*>(to)) T(std::move(*static_cast<T*>(from)));
        }
        else
        {
            variant_helper<Ts...>::move(index, from, to);
        }
    }

    CPPSTREAM_FORCEINLINE
    static void assign(const size_t index, const void* from, void* to) noexcept
    {
        if (index == sizeof...(Ts))
        {
            *(static_cast<T*>(to)) = *(static_cast<const T*>(from));
        }
        else
        {
            variant_helper<Ts...>::assign(index, from, to);
        }
    }

    CPPSTREAM_FORCEINLINE
    static void move_assign(const size_t index, void* from, void* to) noexcept
    {
        if (index == sizeof...(Ts))
        {
            *(static_cast<T*>(to)) = std::move(*(static_cast<T*>(from)));
        }
        else
        {
            variant_helper<Ts...>::move_assign(index, from, to);
        }
    }

    CPPSTREAM_FORCEINLINE
    static void swap(const size_t index, void* lhs, void* rhs) noexcept(std::is_nothrow_swappable_v<T>)
    {
        if (index == sizeof...(Ts))
        {
            using std::swap;
            swap(*static_cast<T*>(lhs), *static_cast<T*>(rhs));
        }
        else
        {
            variant_helper<Ts...>::swap(index, lhs, rhs);
        }
    }

    CPPSTREAM_FORCEINLINE
    static size_t hash(const size_t index, const void* ptr) noexcept
    {
        if (index == sizeof...(Ts))
            return std::hash<T>()(*static_cast<const T*>(ptr));

        return variant_helper<Ts...>::hash(index, ptr);
    }

    template <typename Result, typename Function>
    CPPSTREAM_FORCEINLINE static Result invoke(const size_t index, void* ptr, Function&& function)
        noexcept(noexcept(std::declval<Function>()(std::declval<T&>())))
    {
        if (index == sizeof...(Ts))
            return function(*static_cast<T*>(ptr));

        return variant_helper<Ts...>::template invoke<Result>(index, ptr, std::forward<Function>(function));
    }

    template <typename Result, typename Function>
    CPPSTREAM_FORCEINLINE static Result invoke(const size_t index, const void* ptr, Function&& function)
        noexcept(noexcept(std::declval<Function>()(std::declval<const T&>())))
    {
        if (index == sizeof...(Ts))
            return function(*static_cast<const T*>(ptr));

        return variant_helper<Ts...>::template invoke<Result>(index, ptr, std::forward<Function>(function));
    }

    template <typename Result, typename Function>
    CPPSTREAM_FORCEINLINE static Result invoke_on_rvalue(const size_t index, void* ptr, Function&& function)
        noexcept(noexcept(std::declval<Function>()(std::declval<T&&>())))
    {
        if (index == sizeof...(Ts))
            return function(std::move(*static_cast<T*>(ptr)));

        return variant_helper<Ts...>::template invoke_on_rvalue<Result>(index, ptr, std::forward<Function>(function));
    }
};

template<>
struct variant_helper<> final
{
    static void destroy(const size_t, void*) noexcept
    {
    }

    static void copy(const size_t, const void*, void*) noexcept
    {
    }

    static void move(const size_t, const void*, void*) noexcept
    {
    }

    static void assign(const size_t, const void*, void*) noexcept
    {
    }

    static void move_assign(const size_t, void*, void*) noexcept
    {
    }

    static void swap(const size_t, void*, void*) noexcept
    {
    }

    static size_t hash(const size_t, const void*) noexcept
    {
        return size_t(0);
    }

    CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4297)

    // NOTE: invoke* functions throws an exception and marked as noexcept due to that functions has never called

    template <typename Result, typename Function>
    static Result invoke(const size_t, void*, Function&&) noexcept
    {
        throw bad_variant_access();
    }

    template <typename Result, typename Function>
    static Result invoke(const size_t, const void*, Function&&) noexcept
    {
        throw bad_variant_access();
    }

    template <typename Result, typename Function>
    static Result invoke_on_rvalue(const size_t, void*, Function&&) noexcept
    {
        throw bad_variant_access();
    }

    CPPSTREAM_MSVC_WARNINGS_POP
};

template <typename Function, typename T>
using is_nothrow_match_call = std::bool_constant<noexcept(std::declval<Function>()(std::declval<T>()))>;

template <typename Function, typename... Ts>
constexpr bool is_nothrow_match_function_call() noexcept
{
    return std::conjunction_v<is_nothrow_match_call<Function, Ts>...>;
}

} // detail namespaces

CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4521 4522)

template <typename... Ts>
class variant final
{
    struct valueless_by_exception final {};

    using types = type_list<Ts...>;
    using helper = detail::variant_helper<Ts...>;

    using is_copy_constructible = std::conjunction<std::is_copy_constructible<Ts>...>;
    using is_move_constructible = std::conjunction<std::is_move_constructible<Ts>...>;
    using is_copy_assignable    = std::conjunction<std::is_copy_assignable<Ts>...>;
    using is_move_assignable    = std::conjunction<std::is_move_assignable<Ts>...>;
    using is_swappable          = std::conjunction<std::is_swappable<std::add_lvalue_reference_t<Ts>>...>;

    using is_nothrow_copy_constructible = std::conjunction<std::is_nothrow_copy_constructible<Ts>...>;
    using is_nothrow_move_constructible = std::conjunction<std::is_nothrow_move_constructible<Ts>...>;
    using is_nothrow_copy_assignable    = std::conjunction<std::is_nothrow_copy_assignable<Ts>...>;
    using is_nothrow_move_assignable    = std::conjunction<std::is_nothrow_move_assignable<Ts>...>;
    using is_nothrow_destructible       = std::conjunction<std::is_nothrow_destructible<Ts>...>;
    using is_nothrow_swappable          = std::conjunction<std::is_nothrow_swappable<std::add_lvalue_reference_t<Ts>>...>;

    static_assert(sizeof...(Ts) != 0, "Empty type list isn't allowed.");
    static_assert(std::conjunction_v<std::is_destructible<Ts>...>, "All varian types should be destructible");
    static_assert(!std::conjunction_v<std::is_const<Ts>...>, "All variant types should not be const");
    static_assert(!std::conjunction_v<std::is_volatile<Ts>...>, "All variant types should not be volatile");
    static_assert(!std::conjunction_v<std::is_reference<Ts>...>, "All variant types should not be a reference");
    static_assert(!std::conjunction_v<std::is_void<Ts>...>, "Variant can't hold a value of 'void' type");
    static_assert(!std::conjunction_v<std::is_array<Ts>...>, "Variant can't hold a value of array type");
public:

    template <typename T>
    explicit variant(type_t<T> type) noexcept(std::is_nothrow_default_constructible_v<T>)
        : storage(),
          index_(type_list_ops::index_of_v<types, T>)
    {
        using namespace type_list_ops;

        constexpr_if<contains_v<types, T> &&
                     std::is_default_constructible_v<T>>()
            .then([this](auto type) noexcept(std::is_nothrow_default_constructible_v<remove_cvr_t<decltype(type)>::type>)
            {
                construct<remove_cvr_t<decltype(type)>::type>();
                CPPSTREAM_UNUSED(type);
            })
            .else_([](auto type) noexcept
            {
                using value_type = remove_cvr_t<decltype(type)::type>;
                static_assert(contains_v<types, value_type>, "Invalid variant type");
                static_assert(std::is_default_constructible_v<value_type>, "Type isn't default constructible");
                CPPSTREAM_UNUSED(type);
            })(type);
    }

    template <typename T>
    explicit variant(T&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<T>, T>)
        : storage(),
          index_(type_list_ops::index_of_v<types, T>)
    {
        using namespace type_list_ops;
        using value_type = remove_cvr_t<T>;

        constexpr_if<contains_v<types, value_type> &&
                     std::is_constructible_v<value_type, T>>()
            .then([this](auto&& value) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<decltype(value)>, decltype(value)>)
            {
                using val_type = remove_cvr_t<decltype(value)>;
                construct<val_type>(std::forward<decltype(value)>(value));
            })
            .else_([](auto&& value) noexcept
            {
                using val_type = remove_cvr_t<decltype(value)>;
                static_assert(contains_v<types, val_type>, "Invalid variant type");
                static_assert(std::is_constructible_v<val_type, decltype(value)>, "Type isn't copy or move constructible");
                CPPSTREAM_UNUSED(value);
            })(std::forward<T>(value));
    }

    template <typename T, typename... Args>
    explicit variant(in_place_type_t<T> type, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : storage(),
          index_(type_list_ops::index_of_v<types, T>)
    {
        using namespace type_list_ops;

        constexpr_if<contains_v<types, T> &&
                     std::is_constructible_v<T, Args...>>()
            .then([this](auto type, auto&&... args) noexcept(std::is_nothrow_constructible_v<remove_cvr_t<decltype(type)>::type, decltype(args)...>)
            {
                using value_type = remove_cvr_t<decltype(type)>::type;
                construct<value_type>(std::forward<decltype(args)>(args)...);
                CPPSTREAM_UNUSED(type);
            })
            .else_([](auto type, auto&&... args) noexcept
            {
                using value_type = remove_cvr_t<decltype(type)>::type;
                static_assert(contains_v<types, value_type>, "Invalid variant type");
                static_assert(std::is_constructible_v<value_type, decltype(args)...>, "Invalid type constructor arguments");
                CPPSTREAM_UNUSED(type);
            })(type, std::forward<Args>(args)...);
    }

    variant(variant& that) noexcept(is_nothrow_copy_constructible::value)
        : variant(std::as_const(that))
    {
    }

    variant(const variant& that) noexcept(is_nothrow_copy_constructible::value)
        : storage(),
          index_(that.index_)
    {
        constexpr_if<is_copy_constructible::value>()
            .then([&](auto) noexcept(is_nothrow_copy_constructible::value)
            {
                copy(that);
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Variant isn't copy constructible");
            })(nothing);
    }

    variant(variant&& that) noexcept(is_nothrow_move_constructible::value)
        : storage(),
          index_(that.index_)
    {
        constexpr_if<is_move_constructible::value>()
            .then([&](auto) noexcept(is_nothrow_move_constructible::value)
            {
                move(std::move(that));
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Variant isn't move constructible");
            })(nothing);
    }

    ~variant() noexcept(is_nothrow_destructible::value)
    {
        destroy();
    }

    template <typename T>
    variant& operator= (T&& that) noexcept(is_nothrow_destructible::value                   &&
                                           std::is_nothrow_assignable_v<remove_cvr_t<T>&, T> &&
                                           std::is_nothrow_constructible_v<remove_cvr_t<T>, T>)
    {
        using namespace type_list_ops;
        using value_type = remove_cvr_t<T>;

        constexpr_if<contains_v<types, value_type>        &&
                     std::is_assignable_v<value_type&, T> &&
                     std::is_constructible_v<value_type, T>>()
            .then([this](auto&& that) noexcept(is_nothrow_destructible::value                                             &&
                                               std::is_nothrow_assignable_v<remove_cvr_t<decltype(that)>&, decltype(that)> &&
                                               std::is_nothrow_constructible_v<remove_cvr_t<decltype(that)>, decltype(that)>)
            {
                using that_reference = decltype(that);
                using val_type = remove_cvr_t<that_reference>;
                const auto value_type_index = index_of_v<types, val_type>;

                if (index_ == value_type_index)
                {
                    try_change([&]() noexcept(std::is_nothrow_assignable_v<val_type&, that_reference>)
                    {
                        assign<val_type>(std::forward<that_reference>(that));
                    });
                }
                else
                {
                    destroy();

                    try_change([&]() noexcept(std::is_nothrow_constructible_v<val_type, that_reference>)
                    {
                        construct<val_type>(std::forward<that_reference>(that));
                        index_ = value_type_index;
                    });
                }
            })
            .else_([](auto&& that) noexcept
            {
                using that_reference = decltype(that);
                using val_type = remove_cvr_t<that_reference>;

                static_assert(contains_v<types, val_type>, "Invalid variant type");
                static_assert(std::is_assignable_v<val_type&, that_reference>, "Type isn't assignable");
                static_assert(std::is_constructible_v<val_type, that_reference>, "Type isn't copy constructible");
                CPPSTREAM_UNUSED(that);
            })(std::forward<T>(that));

        return *this;
    }

    variant& operator= (variant& that) noexcept(is_nothrow_destructible::value       &&
                                                is_nothrow_copy_constructible::value &&
                                                is_nothrow_copy_assignable::value)
    {
        return this->operator=(std::as_const(that));
    }

    variant& operator= (const variant& that) noexcept(is_nothrow_destructible::value       &&
                                                      is_nothrow_copy_constructible::value &&
                                                      is_nothrow_copy_assignable::value)
    {
        constexpr_if<is_copy_constructible::value &&
                     is_copy_assignable::value>()
            .then([&](auto) noexcept(is_nothrow_destructible::value       &&
                                     is_nothrow_copy_constructible::value &&
                                     is_nothrow_copy_assignable::value)
            {
                assign(that);
            })
            .else_([](auto) noexcept
            {
                static_assert(is_copy_constructible::value, "Varint isn't copy constructible");
                static_assert(is_copy_assignable::value, "Varint isn't copy assignable");
            })(nothing);

        return *this;
    }

    variant& operator= (variant&& that) noexcept(is_nothrow_destructible::value       &&
                                                 is_nothrow_move_constructible::value &&
                                                 is_nothrow_move_assignable::value)
    {
        constexpr_if<is_move_constructible::value &&
                     is_move_assignable::value>()
            .then([&](auto) noexcept(is_nothrow_destructible::value       &&
                                     is_nothrow_move_constructible::value &&
                                     is_nothrow_move_assignable::value)
            {
                assign(std::move(that));
            })
            .else_([](auto) noexcept
            {
                static_assert(is_move_constructible::value, "Varint isn't move constructible");
                static_assert(is_move_assignable::value, "Varint isn't move assignable");
            })(nothing);

        return *this;
    }

    template <typename T, typename... Args>
    void emplace(in_place_type_t<T>, Args&&... arguments) noexcept(std::is_nothrow_constructible_v<T, Args...> && is_nothrow_destructible::value)
    {
        using namespace type_list_ops;

        constexpr_if<contains_v<types, T> &&
                     std::is_constructible_v<T, Args...>>()
            .then([this](auto type, auto&&... args) noexcept(std::is_nothrow_constructible_v<std::remove_reference_t<decltype(type)>::type, decltype(args)...> &&
                                                             is_nothrow_destructible::value)
            {
                using type_t = std::remove_reference_t<decltype(type)>::type;

                destroy();

                try_change([&]() noexcept(std::is_nothrow_constructible_v<type_t, decltype(args)...>)
                {
                    construct<type_t>(std::forward<decltype(args)>(args)...);
                    index_ = index_of_v<types, type_t>;
                });

                CPPSTREAM_UNUSED(type);
            })
            .else_([](auto type, auto&&... args) noexcept
            {
                using type_t = std::remove_reference_t<decltype(type)>::type;
                static_assert(contains_v<types, type_t>, "Invalid variant type");
                static_assert(std::is_constructible_v<type_t, decltype(args)...>, "Invalid type constructor arguments");
                CPPSTREAM_UNUSED(type);
            })(type_t<T>(), std::forward<Args>(arguments)...);
    }

    template <typename T>
    bool contains() const noexcept
    {
        return index_ == type_list_ops::index_of_v<types, T>;
    }

    bool is_valueless_by_exception() const noexcept
    {
        return index_ == variant_npos;
    }

    size_t index() const noexcept
    {
        return index_;
    }

    size_t hash() const noexcept
    {
        return is_valueless_by_exception() ? size_t(0)
                                           : helper::hash(helper_index(), raw_pointer());
    }

    template <typename T>
    T& get() & noexcept
    {
        static_assert(type_list_ops::contains_v<types, T>, "Invalid type");
        assert(contains<T>() && "Invalid type");
        return *pointer<T>();
    }

    template <typename T>
    const T& get() const & noexcept
    {
        static_assert(type_list_ops::contains_v<types, T>, "Invalid type");
        assert(contains<T>() && "Invalid type");
        return *pointer<T>();
    }

    template <typename T>
    T&& get() && noexcept
    {
        static_assert(type_list_ops::contains_v<types, T>, "Invalid type");
        assert(contains<T>() && "Invalid type");
        return std::move(*pointer<T>());
    }

    template <typename T>
    option<T&> get_if() noexcept
    {
        using namespace type_list_ops;
        static_assert(contains_v<types, T>, "Invalid variant type");

        return (index_ == index_of_v<types, T>) ? option<T&>(*pointer<T>())
                                                : option<T&>();
    }

    template <typename T>
    option<const T&> get_if() const noexcept
    {
        using namespace type_list_ops;
        static_assert(contains_v<types, T>, "Invalid variant type");

        return (index_ == index_of_v<types, T>) ? option<const T&>(*pointer<T>())
                                                : option<const T&>();
    }

    template <typename Function>
    decltype(auto) match(Function&& function) &
    {
        if (is_valueless_by_exception()) throw bad_variant_access();

        using is_function_invokable = std::conjunction<is_invokable<Function, Ts&>...>;

        return constexpr_if<is_function_invokable::value>()
            .then([this](auto&& func) noexcept(detail::is_nothrow_match_function_call<decltype(func), Ts&...>()) -> decltype(auto)
            {
                using result = std::common_type_t<std::result_of_t<decltype(func)(Ts&)>...>;
                return helper::template invoke<result>(helper_index(), raw_pointer(), std::forward<decltype(func)>(func));
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Function can't handle all of the variant types");
            })(std::forward<Function>(function));
    }

    template <typename Function>
    decltype(auto) match(Function&& function) const &
    {
        if (is_valueless_by_exception()) throw bad_variant_access();

        using is_function_invokable = std::conjunction<is_invokable<Function, const Ts&>...>;

        return constexpr_if<is_function_invokable::value>()
            .then([this](auto&& func) noexcept(detail::is_nothrow_match_function_call<decltype(func), const Ts&...>()) -> decltype(auto)
            {
                using result = std::common_type_t<std::result_of_t<decltype(func)(const Ts&)>...>;
                return helper::template invoke<result>(helper_index(), raw_pointer(), std::forward<decltype(func)>(func));
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Function can't handle all of the variant types");
            })(std::forward<Function>(function));
    }

    template <typename Function>
    decltype(auto) match(Function&& function) &&
    {
        if (is_valueless_by_exception()) throw bad_variant_access();

        using is_function_invokable = std::conjunction<is_invokable<Function, Ts&&>...>;

        return constexpr_if<is_function_invokable::value>()
            .then([this](auto&& func) noexcept(detail::is_nothrow_match_function_call<decltype(func), Ts&&...>()) -> decltype(auto)
            {
                using result = std::common_type_t<std::result_of_t<decltype(func)(Ts&&)>...>;
                return helper::template invoke_on_rvalue<result>(helper_index(), raw_pointer(), std::forward<decltype(func)>(func));
            })
            .else_([](auto) noexcept
            {
                static_assert(false, "Function can't handle all of the variant types");
            })(std::forward<Function>(function));
    }

    static void swap(variant& lhs, variant& rhs) noexcept(is_nothrow_swappable::value && is_nothrow_move_constructible::value)
    {
        constexpr_if<is_swappable::value &&
                     is_nothrow_move_constructible::value>()
            .then([&](auto) noexcept(is_nothrow_swappable::value && is_nothrow_move_constructible::value)
            {
                if (lhs.index() == rhs.index())
                {
                    if (lhs.is_valueless_by_exception()) return;
                    helper::swap(lhs.helper_index(), lhs.raw_pointer(), rhs.raw_pointer());
                }
                else
                {
                    if (lhs.is_valueless_by_exception() &&
                        rhs.is_valueless_by_exception()) return;

                    if (lhs.is_valueless_by_exception())
                    {
                        lhs = std::move(rhs);
                        rhs.construct_valueless();
                    }
                    else if (rhs.is_valueless_by_exception())
                    {
                        rhs = std::move(lhs);
                        lhs.construct_valueless();
                    }
                    else
                    {
                        std::swap(lhs, rhs);
                    }
                }
            })
            .else_([&](auto) noexcept
            {
                static_assert(is_swappable::value, "All variant types should be swappable");
                static_assert(is_nothrow_move_constructible::value, "All variant types should be nothrow move constructible");
            })(nothing);
    }

private:

    void* raw_pointer() noexcept
    {
        return reinterpret_cast<void*>(std::addressof(storage));
    }

    const void* raw_pointer() const noexcept
    {
        return reinterpret_cast<const void*>(std::addressof(storage));
    }

    size_t helper_index() const noexcept
    {
        return sizeof...(Ts) - index_ - 1;
    }

    template <typename T>
    T* pointer() noexcept
    {
        return static_cast<T*>(raw_pointer());
    }

    template <typename T>
    const T* pointer() const noexcept
    {
        return static_cast<const T*>(raw_pointer());
    }

    void destroy() noexcept(is_nothrow_destructible::value)
    {
        if (index_ == variant_npos) return;
        helper::destroy(helper_index(), raw_pointer());
    }

    void construct_valueless() noexcept
    {
        construct<valueless_by_exception>();
        index_ = variant_npos;
    }

    template <typename T, typename... Args>
    void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        new (pointer<T>()) T(std::forward<Args>(args)...);
    }

    void copy(const variant& that) noexcept(is_nothrow_copy_constructible::value)
    {
        if (that.index_ == variant_npos) construct_valueless();
        else                             helper::copy(that.helper_index(), that.raw_pointer(), raw_pointer());
    }

    void move(variant&& that) noexcept(is_nothrow_move_constructible::value)
    {
        if (that.index_ == variant_npos) construct_valueless();
        else                             helper::move(that.helper_index(), that.raw_pointer(), raw_pointer());
    }

    template <typename T, typename U>
    void assign(U&& that) noexcept(std::is_nothrow_assignable_v<T&, U>)
    {
        (*pointer<T>()) = std::forward<U>(that);
    }

    void assign(const variant& that) noexcept(is_nothrow_copy_assignable::value &&
                                              is_nothrow_destructible::value    &&
                                              is_nothrow_copy_constructible::value)
    {
        if (index_ == that.index_)
        {
            if (index_ == variant_npos) return;

            try_change([&]() noexcept(is_nothrow_copy_assignable::value)
            {
                helper::assign(helper_index(), that.raw_pointer(), raw_pointer());
            });
        }
        else
        {
            destroy();

            try_change([&]() noexcept(is_nothrow_copy_assignable::value)
            {
                copy(that);
                index_ = that.index_;
            });
        }
    }

    void assign(variant&& that) noexcept(is_nothrow_move_assignable::value &&
                                         is_nothrow_destructible::value    &&
                                         is_nothrow_move_constructible::value)
    {
        if (index_ == that.index_)
        {
            if (index_ == variant_npos) return;

            try_change([&]() noexcept(is_nothrow_move_assignable::value)
            {
                helper::move_assign(helper_index(), that.raw_pointer(), raw_pointer());
            });
        }
        else
        {
            destroy();

            try_change([&]() noexcept(is_nothrow_move_constructible::value)
            {
                move(std::move(that));
                index_ = that.index_;
            });
        }
    }

    template <typename Function>
    void try_change(Function&& function) noexcept(noexcept(std::declval<Function>()()))
    {
        constexpr_if<noexcept(std::declval<Function>()())>()
            .then([&](auto) noexcept
            {
                function();
            })
            .else_([&](auto)
            {
                try
                {
                    function();
                }
                catch (...)
                {
                    construct_valueless();
                    throw;
                }
            })(nothing);
    }

    using storage_t = std::aligned_union_t<sizeof(valueless_by_exception), Ts...>;

    storage_t storage;
    size_t index_;
};

CPPSTREAM_MSVC_WARNINGS_POP

template <typename... Ts>
void swap(variant<Ts...>& lhs, variant<Ts...>& rhs) noexcept(noexcept(variant<Ts...>::swap(lhs, rhs)))
{
    variant<Ts...>::swap(lhs, rhs);
}

} // cppstream namespace

namespace std {

template <typename... Ts>
struct hash<cppstream::variant<Ts...>>
{
    size_t operator() (const cppstream::variant<Ts...>& value) const noexcept
    {
        return value.hash();
    }
};

} // std namespace
