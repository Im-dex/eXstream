#pragma once

namespace cppstream {

struct error_transformation final
{
    template <typename Function>
    const error_transformation& map(const Function&) && noexcept
    {
        return *this;
    }

    template <typename Function>
    const error_transformation& flat_map(const Function&) && noexcept
    {
        return *this;
    }
};

} // cppstream namespace
