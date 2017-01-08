#pragma once

namespace cppstream {

template <template <typename, typename> class T, typename Arg>
struct partial_apply2 final
{
    template <typename _2>
    using bind_1 = T<Arg, _2>;

    template <typename _1>
    using bind_2 = T<_1, Arg>;
};

template <template <typename, typename, typename> class T, typename Arg>
struct partial_apply3 final
{
    template <typename _2, typename _3>
    using bind_1 = T<Arg, _2, _3>;

    template <typename _1, typename _3>
    using bind_2 = T<_1, Arg, _3>;

    template <typename _1, typename _2>
    using bind_3 = T<_1, _2, Arg>;
};

} // cppstream namespace
