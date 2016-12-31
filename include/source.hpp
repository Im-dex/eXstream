#pragma once

#include "stream.hpp"
#include "detail/traits.hpp"

namespace cppstream {
namespace detail {

template <typename Iterable, typename T>
auto source(T&& iterable, std::true_type /*is_lvalue_reference*/)
{
    using const_reference = const Iterable&;
    const_reference ref = iterable;

    using begin_iterator = decltype(ref.begin());
    using end_iterator = decltype(ref.end());
    using value_type = decltype(*std::declval<begin_iterator>());
    
    return stream<value_type, begin_iterator, end_iterator>(ref.begin(), ref.end());
}

template <typename Iterable, typename T>
auto source(T&& iterable, std::false_type /*is_lvalue_reference*/)
{
    using begin_iterator = decltype(iterable.begin());
    using end_iterator = decltype(iterable.end());
    using value_type = decltype(*std::declval<begin_iterator>());

    return stream<value_type, begin_iterator, end_iterator>(iterable.begin(), iterable.end());
}

} // detail namespasce

template <typename T>
auto source(T&& iterable)
{
    using iterable_type = remove_cvr_t<T>;

    return CPPSTREAM_CONSTEXPR_IFELSE(is_iterable_v<iterable_type>,
        noexcept {
            return detail::source<iterable_type>(std::forward<T>(iterable), std::is_lvalue_reference<T>());
        },
        noexcept {
            static_assert(false, "Source should meets 'Iterable' concept");
            return error_t();
        }
    );
}

} // cppstream namespace
