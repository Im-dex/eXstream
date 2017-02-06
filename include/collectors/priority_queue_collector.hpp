#pragma once

#include "detail/traits.hpp"

namespace std {

template <typename T, typename Allocator>
class vector;

template <typename T, typename Container, typename Compare>
class priority_queue;

} // std namespace

namespace exstream {

template <typename T,
          typename Container,
          typename Compare>
class priority_queue_builder final
{
    using queue_t = std::priority_queue<T, Container, Compare>;
public:

    priority_queue_builder() = default;

    explicit priority_queue_builder(queue_t&& queue)
        : queue(std::move(queue))
    {
    }

    priority_queue_builder(priority_queue_builder&&) = default;

    priority_queue_builder(const priority_queue_builder&) = delete;
    priority_queue_builder& operator= (const priority_queue_builder&) = delete;

    void reserve(const size_t) const noexcept
    {
    }

    void append(const T& value)
    {
        queue.push(value);
    }

    void append(T&& value)
    {
        queue.push(std::move(value));
    }

    queue_t build()
    {
        return std::move(queue);
    }

private:

    queue_t queue;
};

struct generic_priority_queue_collector final
{
    generic_priority_queue_collector() noexcept = default;
    generic_priority_queue_collector(generic_priority_queue_collector&&) noexcept = default;

    generic_priority_queue_collector(const generic_priority_queue_collector&) = delete;
    generic_priority_queue_collector& operator= (const generic_priority_queue_collector&) = delete;

    template <typename T>
    auto builder(type_t<T>)
    {
        return priority_queue_builder<T, std::vector<T, std::allocator<T>>, std::less<T>>();
    }
};

template <typename T, typename Container, typename Compare>
class priority_queue_collector final
{
    using queue_t = std::priority_queue<T, Container, Compare>;
public:

    explicit priority_queue_collector(queue_t&& queue)
        : queue(std::move(queue))
    {
    }

    priority_queue_collector(priority_queue_collector&&) = default;

    priority_queue_collector(const priority_queue_collector&) = delete;
    priority_queue_collector& operator= (const priority_queue_collector&) = delete;

    template <typename U>
    auto builder(type_t<U>) -> std::enable_if_t<std::is_same_v<T, U>, priority_queue_builder<T, Container, Compare>>
    {
        return priority_queue_builder<T, Container, Compare>(std::move(queue));
    }

private:

    queue_t queue;
};

template <typename T, typename Container, typename Compare>
auto to_priority_queue(std::priority_queue<T, Container, Compare>&& queue)
{
    return priority_queue_collector<T, Container, Compare>(std::move(queue));
}

inline auto to_priority_queue() noexcept
{
    return generic_priority_queue_collector();
}

} // exstream namespace
