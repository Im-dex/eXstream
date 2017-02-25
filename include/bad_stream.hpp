#pragma once

namespace exstream {

enum class bad_collect_result;

template <typename Error>
struct bad_stream final
{
    // transformations

    template <typename Function>
    bad_stream map(const Function&) const noexcept;

    template <typename Function>
    bad_stream flat_map(const Function&) const noexcept;

    template <typename Function>
    bad_stream filter(const Function&) const noexcept;

    bad_stream distinct() const noexcept;

    // terminators

    size_t count() const noexcept;

    template <typename OutIter>
    void fill(OutIter&&) const noexcept;

    template <typename Collector>
    bad_collect_result collect(Collector&&) const noexcept;

    template <typename Function>
    void foreach(Function&&) const noexcept;
};

} // exstream namespace
