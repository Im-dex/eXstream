#include "test.hpp"

#include "detail/type_list.hpp"

using namespace exstream;
using namespace testing;

#define TEST_CASE_NAME TypeListTest

using list = type_list<float, int, double>;

TEST(TEST_CASE_NAME, empty_Test)
{
    EXPECT_TRUE(type_list<>::empty());
    EXPECT_FALSE(type_list<>::non_empty());
    EXPECT_FALSE(type_list<int>::empty());
    EXPECT_TRUE(type_list<int>::non_empty());
}

TEST(TEST_CASE_NAME, size_Test)
{
    EXPECT_THAT(type_list<>::size(), Eq(0));
    EXPECT_THAT(type_list<int>::size(), Eq(1));
    EXPECT_THAT((type_list<int, double, short>::size()), Eq(3));
}

TEST(TEST_CASE_NAME, head_tail_Test)
{
    using expected_tail = type_list<int, double>;

    EXPECT_TYPES_EQ(type_list<int>::head, int);
    EXPECT_TYPES_EQ(list::head, float);
    EXPECT_TYPES_EQ(list::tail, expected_tail);
}

TEST(TEST_CASE_NAME, concat_Test)
{
    using expected = type_list<float, int, double, float, int, double>;
    EXPECT_TYPES_EQ(list::concat<list>, expected);
    EXPECT_TYPES_EQ(list::concat<type_list<>>, list);
}

TEST(TEST_CASE_NAME, push_front_Test)
{
    using expected = type_list<char, float, int, double>;
    EXPECT_TYPES_EQ(list::push_front<char>, expected);
}

TEST(TEST_CASE_NAME, push_back_Test)
{
    using expected = type_list<float, int, double, char>;
    EXPECT_TYPES_EQ(list::push_back<char>, expected);
}

TEST(TEST_CASE_NAME, contains_Test)
{
    using namespace type_list_ops;

    EXPECT_TRUE((contains_v<list, int>));
    EXPECT_TRUE((contains_v<list, double>));

    EXPECT_FALSE((contains_v<list, char>));
    EXPECT_FALSE((contains_v<list, std::string>));
}

TEST(TEST_CASE_NAME, index_of_Test)
{
    using namespace type_list_ops;

    EXPECT_THAT((index_of_v<list, int>), Eq(1));
    EXPECT_THAT((index_of_v<list, float>), Eq(0));
    EXPECT_THAT((index_of_v<list, std::string>), Eq(type_list_npos));
}
