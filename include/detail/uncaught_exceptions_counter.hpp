#pragma once

#include "config.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <exception>
EXSTREAM_RESTORE_ALL_WARNINGS

namespace exstream {

class uncaught_exceptions_counter final
{
public:

    uncaught_exceptions_counter() noexcept
        : counter(std::uncaught_exceptions())
    {
    }

    uncaught_exceptions_counter(uncaught_exceptions_counter&& that) noexcept
        : counter(that.counter)
    {
        that.counter = 0;
    }

    uncaught_exceptions_counter(const uncaught_exceptions_counter&) = delete;

    uncaught_exceptions_counter& operator= (const uncaught_exceptions_counter&) = delete;
    uncaught_exceptions_counter& operator= (uncaught_exceptions_counter&&) = delete;

    bool is_new_uncaught_exception() const noexcept
    {
        return counter != std::uncaught_exceptions();
    }

private:

    int counter;
};

} // exstream namespaces
