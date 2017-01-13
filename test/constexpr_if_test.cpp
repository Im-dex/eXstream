#include "test.hpp"

#include "detail/constexpr_if.hpp"
#include "detail/type_traits.hpp"

using namespace exstream;

#define TEST_CASE_NAME ConstexprIfTest

struct CallChecker
{
    CallChecker() = default;
    CallChecker(const CallChecker&) = delete;
    CallChecker& operator= (const CallChecker&) = delete;

    MOCK_CONST_METHOD0(called, void());
};

struct comparable
{
    bool operator== (const comparable&) const { return true; }
};

struct non_comparable
{
};

TEST(TEST_CASE_NAME, ConstexprIf)
{
    CallChecker successCallChecker;
    CallChecker failureCallChecker;
    EXPECT_CALL(successCallChecker, called()).Times(testing::Exactly(1));
    EXPECT_CALL(failureCallChecker, called()).Times(testing::Exactly(0));

    constexpr_if<is_comparable_v<comparable>>()
        .then([&](auto) noexcept
        {
            comparable() == comparable();
            successCallChecker.called();
        })(nothing);

    constexpr_if<is_comparable_v<non_comparable>>()
        .then([&](auto) noexcept
        {
            non_comparable() == non_comparable();
            failureCallChecker.called();
        })(nothing);

    constexpr_if<false>()
        .then([](auto) noexcept
        {
            static_assert(false, "Test failed");
        })(nothing);
}

TEST(TEST_CASE_NAME, ConstexprIfElse)
{
    CallChecker successCallChecker;
    CallChecker failureCallChecker;
    EXPECT_CALL(successCallChecker, called()).Times(testing::Exactly(1));
    EXPECT_CALL(failureCallChecker, called()).Times(testing::Exactly(0));

    constexpr_if<is_comparable_v<comparable>>()
        .then([&](auto) noexcept
        {
            comparable() == comparable();
            successCallChecker.called();
        })
        .else_([&](auto) noexcept
        {
            non_comparable() == non_comparable();
            failureCallChecker.called();
        })(nothing);

    constexpr_if<true>()
        .then([](auto) noexcept
        {
            static_assert(true, "Test success");
        })
        .else_([](auto) noexcept
        {
            static_assert(false, "Test failed");
        })(nothing);

    const auto result = constexpr_if<false>()
        .then([](auto) noexcept
        {
            return 0;
        })
        .else_if<true>()
            .then([](auto) noexcept
            {
                return 1;
            })
            .else_([](auto) noexcept
            {
                return 2;
            })(nothing);

    EXPECT_THAT(result, testing::Eq(1));
}

TEST(TEST_CASE_NAME, ConstexprIfTypeInference)
{
    auto result1 = constexpr_if<true>()
        .then([](auto) noexcept
        {
            return 1;
        })
        .else_([](auto) noexcept
        {
            return 0.f;
        })(nothing);

    auto result2 = constexpr_if<false>()
        .then([](auto) noexcept
        {
            return 1;
        })
        .else_([](auto) noexcept
        {
            return 0.f;
        })(nothing);

    EXPECT_TYPES_EQ(decltype(result1), int);
    EXPECT_TYPES_EQ(decltype(result2), float);

    EXPECT_THAT(result1, testing::Eq(1));
    EXPECT_THAT(result2, testing::Eq(0.f));
}
