#include "test.hpp"

#include "detail/cstring.hpp"

using namespace stream;
using namespace std::string_literals;

#define TEST_CASE_NAME CStringTest

TEST(TEST_CASE_NAME, Test)
{
    EXPECT_EQ(cstring().value(), nullptr);
    EXPECT_EQ(cstring("test").value(), "test"s);

    EXPECT_TRUE(cstring().is_empty());
    EXPECT_FALSE(cstring().non_empty());

    EXPECT_EQ(cstring().size(), 0);
    EXPECT_EQ(cstring("test").size(), 4);
}
