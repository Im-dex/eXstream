#pragma once

namespace cppstream {

class in_place final {}; // TODO: constexpr constants
class error_t final {};
class empty final {};

template <typename T>
class in_place_type final {};

template <typename...>
constexpr bool true_v = true;

template <typename...>
constexpr bool false_v = false;

template <typename T>
struct type_t final
{
    constexpr type_t() = default;
};

template <typename T>
constexpr type_t<T> type = type_t<T>();

} // cppstream namespace
