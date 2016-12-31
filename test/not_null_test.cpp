#include "test.hpp"

#include "not_null.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <memory>
CPPSTREAM_RESTORE_ALL_WARNINGS

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME NotNullTest

using unique_ptr = std::unique_ptr<int>;
using shared_ptr = std::shared_ptr<int>;

using raw = not_null<int*>;
using unique = not_null<unique_ptr>;
using shared = not_null<shared_ptr>;

TEST(TEST_CASE_NAME, ConstructorTest)
{
#ifdef STREAM_DEBUG
    EXPECT_ANY_DEATH(raw(nullptr));
    EXPECT_ANY_DEATH(unique(nullptr));
    EXPECT_ANY_DEATH(shared(nullptr));
#endif

    int value = 0;
    auto* rawPtr = &value;
    auto* uniquePtr = new int(0);
    auto* sharedPtr = new int(0);

    EXPECT_THAT(raw(rawPtr), Eq(rawPtr));
    EXPECT_THAT(unique(unique_ptr(uniquePtr)), Eq(uniquePtr));
    EXPECT_THAT(shared(shared_ptr(sharedPtr)), Eq(sharedPtr));
}

// TODO:
