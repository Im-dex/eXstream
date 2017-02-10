#include "test.hpp"

#include "make_array.hpp"

using namespace exstream;
using namespace testing;

#define TEST_CASE_NAME make_array_Test

TEST(TEST_CASE_NAME, make_array_Test)
{
    auto array = make_array(1, 4, 5);

    using expected_type = std::array<int, 3>;
    EXPECT_TYPES_EQ(decltype(array), expected_type);

    EXPECT_THAT(array, ElementsAre(1, 4, 5));
}
