#include "test.hpp"

#include "detail/type_traits.hpp"

STREAM_SUPRESS_ALL_WARNINGS
#include <functional>
STREAM_RESTORE_ALL_WARNINGS

using namespace stream;

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

TEST(TEST_CASE_NAME, has_operator_Test)
{
    struct empty {};

    struct type_with_const_operator
    {
        void operator* () const {}
    };

    struct type_with_operator
    {
        void operator* () {}
        void operator++ () {}
        void operator++ (int) {}
    };

    struct type_with_prefix_increment
    {
        void operator++() {}
    };

    // unary operator *
    EXPECT_TRUE(has_unary_operator_asterisk_v<int*>);
    EXPECT_TRUE(has_unary_operator_asterisk_v<type_with_operator>);
    EXPECT_TRUE(has_unary_operator_asterisk_v<type_with_const_operator>);
    EXPECT_FALSE(has_unary_operator_asterisk_v<int>);
    EXPECT_FALSE(has_unary_operator_asterisk_v<empty>);

    // prefix operator ++
    EXPECT_TRUE(has_prefix_increment_operator_v<int>);
    EXPECT_TRUE(has_prefix_increment_operator_v<int*>);
    EXPECT_TRUE(has_prefix_increment_operator_v<type_with_operator>);
    EXPECT_TRUE(has_prefix_increment_operator_v<type_with_prefix_increment>);
    EXPECT_FALSE(has_prefix_increment_operator_v<empty>);

    // postfix operator ++
    EXPECT_TRUE(has_postfix_increment_operator_v<int>);
    EXPECT_TRUE(has_postfix_increment_operator_v<int*>);
    EXPECT_TRUE(has_postfix_increment_operator_v<type_with_operator>);
    EXPECT_FALSE(has_postfix_increment_operator_v<type_with_prefix_increment>);
    EXPECT_FALSE(has_postfix_increment_operator_v<empty>);
}

TEST(TEST_CASE_NAME, int_sum_Test)
{
    EXPECT_EQ((int_sum_v<int, 0, 1, 5>), 6);
    EXPECT_EQ((int_sum_v<int, -4, 1, 2, -10>), -11);

    EXPECT_EQ((int_sum_v<size_t, 4, 16>), 20);
    EXPECT_EQ((int_sum_v<size_t, -2, 1>), std::numeric_limits<size_t>::max());

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
    // NOTE: generic lambda accepts all types of arguments

    // incorrect arguments count
    EXPECT_FALSE((is_invokable_v<decltype(func), const float>));
    EXPECT_FALSE((is_invokable_v<decltype(StaticFunctionHolder::staticFunc)>));
    EXPECT_FALSE((is_invokable_v<decltype(stdFunc), std::string, float, int>));
    EXPECT_FALSE((is_invokable_v<decltype(lambda), double*>));
    EXPECT_FALSE((is_invokable_v<decltype(genericLambda), int, float&, const double>));
}

STREAM_DEFINE_HAS_TYPE(value_type)

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

    EXPECT_TRUE(has_value_type_type_v<std::string>);
    EXPECT_TRUE(has_value_type_type_v<std::vector<int>::iterator>);
    EXPECT_TRUE(has_value_type_type_v<HasValueType>);

    EXPECT_FALSE(has_value_type_type_v<int>);
    EXPECT_FALSE(has_value_type_type_v<NoValueType>);
}

TEST(TEST_CASE_NAME, result_of_Test)
{
    {
        using Result = result_of<decltype(func), float, int&>;
        using BadResult = result_of<decltype(func), float>;
        EXPECT_TYPES_EQ(Result, int);
        EXPECT_TYPES_EQ(BadResult, error_t);
    }

    {
        using Result = result_of<decltype(StaticFunctionHolder::staticFunc), std::string>;
        using BadResult = result_of<decltype(StaticFunctionHolder::staticFunc), int, std::string>;
        EXPECT_TYPES_EQ(Result, void);
        EXPECT_TYPES_EQ(BadResult, error_t);
    }

    {
        using Result = result_of<decltype(lambda), int, double*>;
        using BadResult = result_of<decltype(lambda), int, double*, char>;
        EXPECT_TYPES_EQ(Result, int&);
        EXPECT_TYPES_EQ(BadResult, error_t);
    }

    {
        using Result = result_of<decltype(genericLambda), int, int>;
        using BadResult = result_of<decltype(genericLambda), int>;
        EXPECT_TYPES_EQ(Result, int&);
        EXPECT_TYPES_EQ(BadResult, error_t);
    }
}
