#pragma once

#include "config.hpp"
#include "utility.hpp"

namespace std {

template <typename T>
class allocator;

} // std namespace

namespace exstream {

template <typename T,
          typename Container,
          template <typename, typename> class Adaptor>
class adaptor_builder final
{
    using adaptor_t = Adaptor<T, Container>;
public:

    adaptor_builder() = default;

    explicit adaptor_builder(adaptor_t&& adaptor)
        : adaptor(std::move(adaptor))
    {
    }

    adaptor_builder(adaptor_builder&&) = default;

    adaptor_builder(const adaptor_builder&) = delete;
    adaptor_builder& operator= (const adaptor_builder&) = delete;

    void reserve(const size_t) const noexcept
    {
    }

    void append(const T& value)
    {
        adaptor.push(value);
    }

    void append(T&& value)
    {
        adaptor.push(std::move(value));
    }

    adaptor_t build()
    {
        return std::move(adaptor);
    }

private:

    adaptor_t adaptor;
};

template <template <typename, typename> class Container,
          template <typename, typename> class Adaptor>
struct generic_adaptor_collector final
{
    generic_adaptor_collector() noexcept = default;
    generic_adaptor_collector(generic_adaptor_collector&&) noexcept = default;

    generic_adaptor_collector(const generic_adaptor_collector&) = delete;
    generic_adaptor_collector& operator= (const generic_adaptor_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>)
    {
        using container_t = Container<T, std::allocator<T>>;
        return adaptor_builder<T, container_t, Adaptor>();
    }
};

template <typename T,
          typename Container,
          template <typename, typename> class Adaptor>
class adaptor_collector final
{
    using adaptor_t = Adaptor<T, Container>;
public:

    explicit adaptor_collector(Container&& container)
        : adaptor(std::move(container))
    {
    }

    adaptor_collector(adaptor_collector&&) = default;

    adaptor_collector(const adaptor_collector&) = delete;
    adaptor_collector& operator= (const adaptor_collector&) = delete;

    template <typename U>
    auto builder(type_t<U>) -> std::enable_if_t<std::is_same_v<T, U>, adaptor_builder<U, Container, Adaptor>>
    {
        return adaptor_builder<U, Container, Adaptor>(std::move(adaptor));
    }

private:

    adaptor_t adaptor;
};

} // exstream namespace
