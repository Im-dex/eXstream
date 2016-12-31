#pragma once

#include "transformation.hpp"
#include "transform_iterator.hpp"
#include "option.hpp"

namespace cppstream {

template <typename T,
          typename Iter,
          typename Function>
class begin_flat_map_iterator final : public transform_iterator<T, Iter>
{
    using stream_type = typename std::iterator_traits<Iter>::value_type;
    using stream_iterator_type = typename stream_type::iterator;
public:

    explicit begin_flat_map_iterator(const Iter& iterator, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Iter>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    explicit begin_flat_map_iterator(Iter&& iterator, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Iter>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    bool operator== (const begin_flat_map_iterator& that) noexcept(is_nothrow_comparable_v<Iter> && is_nothrow_comparable_v<stream_iterator_type>)
    {
        return (iterator == that.iterator) && (stream_iterator == that.stream_iterator);
    }

    bool operator!= (const begin_flat_map_iterator& that) noexcept(is_nothrow_comparable_v<Iter> && is_nothrow_comparable_v<stream_iterator_type>)
    {
        return (iterator != that.iterator) || (stream_iterator != that.stream_iterator);
    }

    bool operator== (const transform_iterator<T, Iter>& that) noexcept(is_nothrow_comparable_v<Iter> && noexcept(std::declval<stream_type>().end()))
    {
        return (iterator == that.iterator) && (stream.empty() || stream_iterator == stream.get().end());
    }

    bool operator!= (const transform_iterator<T, Iter>& that) noexcept(is_nothrow_comparable_v<Iter> && noexcept(std::declval<stream_type>().end()))
    {
        return (iterator != that.iterator) || (stream.non_empty() && stream_iterator != stream.get().end());
    }

    begin_flat_map_iterator& operator++ () noexcept(noexcept(++std::declval<Iter>())              &&
                                                    std::is_nothrow_destructible_v<stream_type>   &&
                                                    is_nothrow_comparable_v<stream_iterator_type> &&
                                                    noexcept(++std::declval<stream_iterator_type>()))
    {
        if (stream.non_empty() && (stream_iterator != stream.get().end()))
        {
            ++stream_iterator;
        }
        else
        {
            stream.reset();
            ++iterator;
        }

        return *this;
    }

    auto operator* () noexcept(noexcept(std::declval<Function>()(*std::declval<Iter>())) &&
                               noexcept(std::declval<stream_type>().begin()) &&
                               noexcept(*(std::declval<stream_iterator_type>()))) -> value_type<std::remove_reference_t<T>>
    {
        if (stream.empty())
        {
            stream = function(*iterator);
            stream_iterator = stream.get().begin();
        }

        return *stream_iterator;
    }

private:

    const Function& function;
    stream_iterator_type stream_iterator;
    option<stream_type> stream;
};

template <typename T,
          typename Source,
          typename Function>
class flat_map_transformation final : public transformation<T, Source, Function, flat_map_transformation<T, Source, Function>>
{
public:

    using begin_iterator = begin_flat_map_iterator<T, source_begin_iterator, Function>;
    using end_iterator = transform_iterator<T, source_end_iterator>;

    flat_map_transformation(const Source& source, const Function& function) noexcept
        : transformation(source, function)
    {
    }

    begin_iterator begin() const noexcept(std::is_nothrow_constructible_v<begin_iterator, source_begin_iterator, Function>)
    {
        return begin_iterator(source.begin(), function);
    }

    end_iterator end() const noexcept(std::is_nothrow_constructible_v<end_iterator, source_end_iterator>)
    {
        return end_iterator(source.end());
    }
};

} // cppstream namespace
