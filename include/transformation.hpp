#pragma once

#include "detail/traits.hpp"

namespace stream {

template <typename T, typename Function, typename Source>
class transformation;

namespace detail {

template <typename Source>
constexpr bool is_nothrow_source_value(bool constSource) noexcept
{
    using source_type = std::conditional_t<constSource, const Source, Source>;
    return noexcept(std::declval<source_type>().value());
}

template <typename Function, typename Source>
constexpr bool is_nothrow_function_call(bool constSource) noexcept
{
    using source_type = std::conditional_t<constSource, const Source, Source>;
    return noexcept(std::declval<Function>()(std::declval<source_type>()));
}

template <bool IsIterator, typename Source, typename MapFunction>
struct map_traits_impl
{
};

template <typename Iterator, typename MapFunction>
struct map_traits_impl<true, Iterator, MapFunction>
{
    using input_value = const typename Iterator::value_type&;
    using output_value = std::result_of_t<MapFunction(input_value)>;
    using result_transformation = transformation<output_value, remove_cvr_t<MapFunction>, Iterator>;

    static constexpr bool is_nothrow_map() noexcept
    {
        return std::is_nothrow_constructible_v<result_transformation, Iterator&&, MapFunction>;
    }
};

template <typename Source, typename MapFunction>
struct map_traits : public map_traits_impl<is_iterator_v<Source>, Source, MapFunction>
{
};

template <typename T, typename Function, typename Source, typename MapFunction>
struct map_traits<transformation<T, Function, Source>, MapFunction> final
{
    using input_value = std::conditional_t<is_iterator_v<Source>, const T&, T&>;
    using output_value = std::result_of<MapFunction(input_value)>;
    using transformation_t = transformation<T, Function, Source>;
    using result_transformation = transformation<output_value, remove_cvr_t<MapFunction>, transformation_t>;

    static constexpr bool is_nothrow_map() noexcept
    {
        return std::is_nothrow_constructible_v<result_transformation, transformation_t&&, MapFunction>;
    }
};

} // detail namespace

template <typename T, typename Function, typename Source>
class transformation final
{
    using this_t = transformation<T, Function, Source>;
public:

    explicit transformation(const Source& source, const Function& function) noexcept(std::is_nothrow_copy_constructible_v<Source> &&
                                                                                     std::is_nothrow_copy_constructible_v<Function>)
        : source(source),
          function(function)
    {
    }

    explicit transformation(Source&& source, const Function& function) noexcept(std::is_nothrow_move_constructible_v<Source> &&
                                                                                std::is_nothrow_copy_constructible_v<Function>)
        : source(std::move(source)),
          function(function)
    {
    }

    explicit transformation(const Source& source, Function&& function) noexcept(std::is_nothrow_copy_constructible_v<Source> &&
                                                                                std::is_nothrow_move_constructible_v<Function>)
        : source(source),
          function(std::move(function))
    {
    }

    explicit transformation(Source&& source, Function&& function) noexcept(std::is_nothrow_move_constructible_v<Source> &&
                                                                           std::is_nothrow_move_constructible_v<Function>)
        : source(std::move(source)),
          function(std::move(function))
    {
    }

    ~transformation() noexcept(std::is_nothrow_destructible_v<Source> &&
                               std::is_nothrow_destructible_v<Function>) = default;

    transformation(const transformation&) = delete;
    transformation(transformation&&) noexcept(std::is_nothrow_move_constructible_v<Source> &&
                                              std::is_nothrow_move_constructible_v<Function>) = default;

    transformation& operator= (const transformation&) = delete;
    transformation& operator= (transformation&&) = delete;

    bool is_empty() const noexcept(noexcept(std::declval<const Source>().is_empty()))
    {
        return source.is_empty();
    }

    bool non_empty() const noexcept(noexcept(std::declval<const Source>().non_empty()))
    {
        return source.non_empty();
    }

    void advance() noexcept(noexcept(std::declval<Source>().advance()))
    {
        source.advance();
    }

    T value() noexcept(detail::is_nothrow_source_value<Source>(is_iterator_v<Source>) &&
                       detail::is_nothrow_function_call<Function, Source>(is_iterator_v<Source>))
    {
        return function(value(is_iterator_v<Source>()));
    }

    template <typename MapFunction>
    auto map(MapFunction&& mapFunction) && noexcept(detail::map_traits<this_t, MapFunction>::is_nothrow_map())
    {
        using value_type = std::conditional_t<is_iterator_v<Source>, const T&, T&>;
        return map(std::forward<MapFunction>(mapFunction), is_invokable<MapFunction, value_type>());
    }

private:

    const T& value(std::true_type /* is_iterator */) const noexcept(detail::is_nothrow_source_value<Source>(true))
    {
        return source.value();
    }

    T value(std::false_type /* is_iterator */) noexcept(detail::is_nothrow_source_value<Source>(false))
    {
        return source.value();
    }

    template <typename MapFunction>
    auto map(MapFunction&& mapFunction, std::true_type /* is_invokable */) noexcept(detail::map_traits<this_t, MapFunction>::is_nothrow_map())
    {
        using map_traits = detail::map_traits<this_t, MapFunction>;
        return typename map_traits::result_transformation(std::move(*this), std::forward<MapFunction>(mapFunction));
    }

    template <typename MapFuntion>
    static void map(MapFuntion&&, std::false_type /* is_invokable */) noexcept
    {
        static_assert(false_v<MapFuntion>, "Invalid map function");
    }

    Source source;
    Function function;
};

} // stream namespace
