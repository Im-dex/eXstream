#pragma once

namespace exstream {

struct in_place_t final {};
constexpr auto in_place = in_place_t();

template <typename T>
struct in_place_type_t final
{
    using type = T;
};

// TODO: ICE
/*template <typename T>
constexpr auto in_place_type = in_place_type_t<T>();*/

class nothing_t final {};
constexpr auto nothing = nothing_t();

template <typename T>
struct type_t final
{
    using type = T;
};

// TODO: ICE under MSVC 2015
/*template <typename T>
constexpr type_t<T> type = type_t<T>();*/

} // exstream namespace
