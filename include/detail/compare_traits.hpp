#pragma once

namespace std {

template <typename T>
struct less;

template <typename T>
struct less_equal;

template <typename T>
struct greater;

template <typename T>
struct greater_equal;

} // std namespace

namespace exstream {

enum class Order
{
    Ascending,
    Descending,
    Unknown
};

template <typename T>
struct compare_traits
{
    static constexpr Order order = Order::Unknown;
};

template <typename T>
struct compare_traits<std::less<T>>
{
    static constexpr Order order = Order::Ascending;
};

template <typename T>
struct compare_traits<std::less_equal<T>>
{
    static constexpr Order order = Order::Ascending;
};

template <typename T>
struct compare_traits<std::greater<T>>
{
    static constexpr Order order = Order::Descending;
};

template <typename T>
struct compare_traits<std::greater_equal<T>>
{
    static constexpr Order order = Order::Descending;
};

} // exstream namespace
