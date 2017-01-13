#pragma once

#include "config.hpp"
#include "detail/preprocessor.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <gtest/gtest.h>
#include <gmock/gmock.h>
EXSTREAM_RESTORE_ALL_WARNINGS

#define EXPECT_TYPES_EQ(T1, T2)\
    EXPECT_PRED_FORMAT4((::exstream::is_types_eq<T1, T2>), Type<T1>(), Type<T2>(), EXSTREAM_PP_STRINGIZE(T1), EXSTREAM_PP_STRINGIZE(T2))

#define EXPECT_TYPES_NE(T1, T2)\
    EXPECT_PRED_FORMAT4((::exstream::is_types_ne<T1, T2>), Type<T1>(), Type<T2>(), EXSTREAM_PP_STRINGIZE(T1), EXSTREAM_PP_STRINGIZE(T2))

#define EXPECT_ANY_DEATH(expression) EXPECT_DEATH(expression, ".*")

namespace exstream {

template <typename T>
class Type final
{
public:

    const char* name() const noexcept
    {
        return typeid(T).name();
    }
};

template <typename T>
std::ostream& operator<< (std::ostream& ostream, const Type<T>& type)
{
    ostream << type.name();
    return ostream;
}

template <typename T, typename U>
std::enable_if_t<std::is_same_v<T, U>, testing::AssertionResult>
is_types_eq(const char*, const char*, const char*, const char*,
            const Type<T>&, const Type<U>&, const char*, const char*) noexcept
{
    return testing::AssertionSuccess();
}

template <typename T, typename U>
std::enable_if_t<!std::is_same_v<T, U>, testing::AssertionResult>
is_types_eq(const char*, const char*, const char*, const char*,
            const Type<T>& x, const Type<U>& y, const char* xSource, const char* ySource)
{
    return testing::AssertionFailure()                          << std::endl
                                       << "Types mismatch"      << std::endl
                                       << xSource << " = " << x << std::endl
                                       << ySource << " = " << y;
}

template <typename T, typename U>
std::enable_if_t<!std::is_same_v<T, U>, testing::AssertionResult>
is_types_ne(const char*, const char*, const char*, const char*,
            const Type<T>&, const Type<U>&, const char*, const char*) noexcept
{
    return testing::AssertionSuccess();
}

template <typename T, typename U>
std::enable_if_t<std::is_same_v<T, U>, testing::AssertionResult>
is_types_ne(const char*, const char*, const char*, const char*,
            const Type<T>& x, const Type<U>& y, const char* xSource, const char* ySource)
{
    return testing::AssertionFailure()                          << std::endl
                                       << "The types is same"   << std::endl
                                       << xSource << " = " << x << std::endl
                                       << ySource << " = " << y;
}

} // exstream namespace
