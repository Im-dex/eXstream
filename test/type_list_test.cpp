#include "test.hpp"

#include "detail/type_list.hpp"

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME TypeListest

using List = type_list<float, int, double>;

TEST(TEST_CASE_NAME, EmptyTest)
{
    EXPECT_TRUE(type_list<>::empty());
    EXPECT_FALSE(type_list<>::non_empty());
    EXPECT_FALSE(type_list<int>::empty());
    EXPECT_TRUE(type_list<int>::non_empty());
}

TEST(TEST_CASE_NAME, SizeTest)
{
    EXPECT_THAT(type_list<>::size(), Eq(0));
    EXPECT_THAT(type_list<int>::size(), Eq(1));
    EXPECT_THAT((type_list<int, double, short>::size()), Eq(3));
}

TEST(TEST_CASE_NAME, HeadTailTest)
{
    using ListTail = type_list<int, double>;

    EXPECT_TYPES_EQ(type_list<int>::head, int);
    EXPECT_TYPES_EQ(List::head, float);
    EXPECT_TYPES_EQ(List::tail, ListTail);
}

TEST(TEST_CASE_NAME, ConcatTest)
{
    using Expected = type_list<float, int, double, float, int, double>;
    EXPECT_TYPES_EQ(List::concat<List>, Expected);
    EXPECT_TYPES_EQ(List::concat<type_list<>>, List);
}
