#include "test.hpp"

#include "constexpr_if.hpp"
#include "detail/type_traits.hpp"

using namespace cppstream;

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

    CPPSTREAM_CONSTEXPR_IF(is_comparable_v<comparable>,
        noexcept {
            comparable() == comparable();
            successCallChecker.called();
        }
    );

    CPPSTREAM_CONSTEXPR_IF(is_comparable_v<non_comparable>,
        noexcept {
            non_comparable() == non_comparable();
            failureCallChecker.called();
        }
    );

    CPPSTREAM_CONSTEXPR_IF(false,
        noexcept {
            static_assert(false, "Test failed");
        }
    );
}

TEST(TEST_CASE_NAME, ConstexprIfElse)
{
    CallChecker successCallChecker;
    CallChecker failureCallChecker;
    EXPECT_CALL(successCallChecker, called()).Times(testing::Exactly(1));
    EXPECT_CALL(failureCallChecker, called()).Times(testing::Exactly(0));

    CPPSTREAM_CONSTEXPR_IFELSE(is_comparable_v<comparable>,
        noexcept {
            comparable() == comparable();
            successCallChecker.called();
        },
        noexcept {
            non_comparable() == non_comparable();
            failureCallChecker.called();
        }
    );

    CPPSTREAM_CONSTEXPR_IFELSE(true,
        noexcept {
            static_assert(true, "Test success");
        },
        noexcept {
            static_assert(false, "Test failed");
        }
    );

    const auto result = CPPSTREAM_CONSTEXPR_IFELSE(true,
        noexcept {
            return 0;
        },
        noexcept {
            return 1;
        }
    );

    EXPECT_THAT(result, testing::Eq(0));
}

TEST(TEST_CASE_NAME, ConstexprIfElseTypeInference)
{
    auto result1 = CPPSTREAM_CONSTEXPR_IFELSE(true,
        noexcept {
            return 1;
        },
        noexcept {
            return 0.f;
        }
    );

    auto result2 = CPPSTREAM_CONSTEXPR_IFELSE(false,
        noexcept {
            return 1;
        },
        noexcept {
            return 0.f;
        }
    );

    EXPECT_TYPES_EQ(decltype(result1), int);
    EXPECT_TYPES_EQ(decltype(result2), float);

    CPPSTREAM_UNUSED(result1);
    CPPSTREAM_UNUSED(result2);
}
