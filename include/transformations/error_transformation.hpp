#pragma once

namespace exstream {

struct error_transformation final
{
    template <typename Function>
    const error_transformation& map(const Function&) const noexcept
    {
        return *this;
    }

    template <typename Function>
    const error_transformation& flat_map(const Function&) const noexcept
    {
        return *this;
    }

    template <typename Function>
    const error_transformation& filter(const Function&) const noexcept
    {
        return *this;
    }

    const error_transformation& distinct() const noexcept
    {
        return *this;
    }
};

} // exstream namespace
