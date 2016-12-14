#pragma once

#include "config.hpp"

STREAM_SUPRESS_ALL_WARNINGS
#include <exception>
STREAM_RESTORE_ALL_WARNINGS

namespace stream {

class no_such_element_exception : public std::exception
{
public:
    no_such_element_exception() noexcept
        : std::exception("No such element")
    {
    }
};

} // stream namespace
