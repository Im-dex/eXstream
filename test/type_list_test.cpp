#include "test.hpp"

#include "detail/type_list.hpp"

using namespace stream;

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
    EXPECT_EQ(type_list<>::size(), 0);
    EXPECT_EQ(type_list<int>::size(), 1);
    EXPECT_EQ((type_list<int, double, short>::size()), 3);
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
