#pragma once

namespace stream {

class in_place final {};
class error_t final {};

template <typename...>
constexpr bool true_v = true;

template <typename...>
constexpr bool false_v = false;

} // stream namespace
