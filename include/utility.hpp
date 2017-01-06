#pragma once

namespace cppstream {

class in_place_t final {};
constexpr const auto in_place = in_place_t();

template <typename T>
class in_place_type_t final {};

template <typename T>
constexpr const auto in_place_type = in_place_type_t<T>();

class nothing_t final {};
constexpr const auto nothing = nothing_t();

template <typename T>
struct type_t final
{
    constexpr type_t() = default;
};

template <typename T>
constexpr const auto type = type_t<T>();

} // cppstream namespace
