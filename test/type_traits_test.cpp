#include "test.hpp"

#include "detail/type_traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <functional>
CPPSTREAM_RESTORE_ALL_WARNINGS

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME TraitsTest

TEST(TEST_CASE_NAME, remove_crv_Test)
{
    EXPECT_TYPES_EQ(remove_cvr_t<int>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<const int>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<int&>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<volatile int>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<const int&>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<volatile int&>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<const volatile int>, int);
    EXPECT_TYPES_EQ(remove_cvr_t<const volatile int&>, int);

    EXPECT_TYPES_NE(remove_cvr_t<int&>, int&);
    EXPECT_TYPES_NE(remove_cvr_t<const int>, const int);
    EXPECT_TYPES_NE(remove_cvr_t<volatile int>, volatile int);
    EXPECT_TYPES_NE(remove_cvr_t<const int&>, const int&);
    EXPECT_TYPES_NE(remove_cvr_t<volatile int&>, volatile int&);
    EXPECT_TYPES_NE(remove_cvr_t<const volatile int>, const volatile int);
    EXPECT_TYPES_NE(remove_cvr_t<const volatile int&>, const volatile int&);
}

struct non_comparable {};

TEST(TEST_CASE_NAME, is_comparable_Test)
{
    EXPECT_TRUE((is_comparable_to_v<int, float>));
    EXPECT_FALSE((is_comparable_to_v<int, non_comparable>));

    EXPECT_TRUE((is_comparable_v<int>));
    EXPECT_FALSE((is_comparable_v<non_comparable>));
}

TEST(TEST_CASE_NAME, is_nothrow_comparable_Test)
{
    struct throw_comparable
    {
        bool operator== (const throw_comparable&) { return true; }
    };

    EXPECT_TRUE((is_nothrow_comparable_to_v<int, float>));
    EXPECT_TRUE(is_nothrow_comparable_v<int>);

    EXPECT_FALSE(is_nothrow_comparable_v<throw_comparable>);
    EXPECT_FALSE(is_nothrow_comparable_v<non_comparable>);
}

TEST(TEST_CASE_NAME, sum_Test)
{
    EXPECT_THAT((sum(0, 1, 5)), Eq(6));
    EXPECT_THAT((sum(-4, 1, 2, -10)), Eq(-11));

    EXPECT_THAT((sum(4, size_t(16))), Eq(20));
    EXPECT_THAT((sum(-2, size_t(1))), Eq(std::numeric_limits<size_t>::max()));

}

int func(const float, int&) { return 0; }

struct StaticFunctionHolder
{
    static void staticFunc(std::string) {}
};

int value = 0;
auto lambda = [&](int, double*) -> int& { return value; };
auto genericLambda = [&](auto, auto) -> int& { return value; };

const std::function<void(std::string, float)> stdFunc = [&](auto, auto) -> void {};

TEST(TEST_CASE_NAME, is_callable_Test)
{
    EXPECT_TRUE((is_callable_v<decltype(func), int, const float, int&>));
    EXPECT_TRUE((is_callable_v<decltype(StaticFunctionHolder::staticFunc), void, std::string>));
    EXPECT_TRUE((is_callable_v<decltype(stdFunc), void, std::string, float>));
    EXPECT_TRUE((is_callable_v<decltype(lambda), int&, int, double*>));
    EXPECT_TRUE((is_callable_v<decltype(genericLambda), int&, int, float&>));

    // incorrect return type
    EXPECT_FALSE((is_callable_v<decltype(func), float, const float, int&>));
    EXPECT_FALSE((is_callable_v<decltype(StaticFunctionHolder::staticFunc), int, std::string>));
    EXPECT_FALSE((is_callable_v<decltype(stdFunc), int, std::string, float>));
    EXPECT_FALSE((is_callable_v<decltype(lambda), int, int, double*>));
    EXPECT_FALSE((is_callable_v<decltype(genericLambda), float, int, float&>));

    // incorrect arguments
    EXPECT_FALSE((is_callable_v<decltype(func), int, std::string, int&>));
    EXPECT_FALSE((is_callable_v<decltype(StaticFunctionHolder::staticFunc), void, int>));
    EXPECT_FALSE((is_callable_v<decltype(stdFunc), void, float, std::string>));
    EXPECT_FALSE((is_callable_v<decltype(lambda), int&, int*, double*>));
    // NOTE: generic lambda accepts all types of arguments

    // incorrect arguments count
    EXPECT_FALSE((is_callable_v<decltype(func), int, const float>));
    EXPECT_FALSE((is_callable_v<decltype(StaticFunctionHolder::staticFunc), void>));
    EXPECT_FALSE((is_callable_v<decltype(stdFunc), void, std::string, float, int>));
    EXPECT_FALSE((is_callable_v<decltype(lambda), int&, double*>));
    EXPECT_FALSE((is_callable_v<decltype(genericLambda), int&, int, float&, const double>));
}

TEST(TEST_CASE_NAME, is_invokable_Test)
{
    EXPECT_TRUE((is_invokable_v<decltype(func), const float, int&>));
    EXPECT_TRUE((is_invokable_v<decltype(StaticFunctionHolder::staticFunc), std::string>));
    EXPECT_TRUE((is_invokable_v<decltype(stdFunc), std::string, float>));
    EXPECT_TRUE((is_invokable_v<decltype(lambda), int, double*>));
    EXPECT_TRUE((is_invokable_v<decltype(genericLambda), int, float&>));

    // incorrect arguments
    EXPECT_FALSE((is_invokable_v<decltype(func), std::string, int&>));
    EXPECT_FALSE((is_invokable_v<decltype(StaticFunctionHolder::staticFunc), int>));
    EXPECT_FALSE((is_invokable_v<decltype(stdFunc), float, std::string>));
    EXPECT_FALSE((is_invokable_v<decltype(lambda), int*, double*>));
    // NOTE: generic lambda accepts any type of argument

    // incorrect arguments count
    EXPECT_FALSE((is_invokable_v<decltype(func), const float>));
    EXPECT_FALSE((is_invokable_v<decltype(StaticFunctionHolder::staticFunc)>));
    EXPECT_FALSE((is_invokable_v<decltype(stdFunc), std::string, float, int>));
    EXPECT_FALSE((is_invokable_v<decltype(lambda), double*>));
    EXPECT_FALSE((is_invokable_v<decltype(genericLambda), int, float&, const double>));
}

CPPSTREAM_DEFINE_HAS_TYPE(value_type)

TEST(TEST_CASE_NAME, has_type_Test)
{
    struct NoValueType
    {
        using type = int;
    };

    struct HasValueType
    {
        using value_type = int;
    };

    struct HasReferenceValueType
    {
        using value_type = int&;
    };

    EXPECT_TRUE(has_value_type<std::string>::value);
    EXPECT_TRUE(has_value_type_v<std::vector<int>::iterator>);
    EXPECT_TRUE(has_value_type<HasValueType>::value);
    EXPECT_TRUE(has_value_type_v<HasReferenceValueType>);

    EXPECT_FALSE(has_value_type<int>::value);
    EXPECT_FALSE(has_value_type_v<NoValueType>);
}
