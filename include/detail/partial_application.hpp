#pragma once

namespace exstream {

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

template <template <typename, typename, typename, typename> class T, typename Arg>
struct partial_apply4 final
{
    template <typename _2, typename _3, typename _4>
    using bind_1 = T<Arg, _2, _3, _4>;

    template <typename _1, typename _3, typename _4>
    using bind_2 = T<_1, Arg, _3, _4>;

    template <typename _1, typename _2, typename _4>
    using bind_3 = T<_1, _2, Arg, _4>;

    template <typename _1, typename _2, typename _3>
    using bind_4 = T<_1, _2, _3, Arg>;
};

} // exstream namespace
