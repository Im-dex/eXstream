#include "test.hpp"

#include "detail/traits.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <forward_list>
CPPSTREAM_RESTORE_ALL_WARNINGS

using namespace cppstream;

#define TEST_CASE_NAME TraitsTest

TEST(TEST_CASE_NAME, is_sizable_Test)
{
    struct Sizable
    {
        std::size_t size() const { return 1; }
    };

    struct NonConstSizable
    {
        std::size_t size() { return 1; }
    };

    struct NonIntegerSizable
    {
        float size() { return 1.0f; }
    };

    EXPECT_TRUE(is_sizable_v<std::string>);
    EXPECT_TRUE(is_sizable_v<std::vector<int>>);
    EXPECT_TRUE(is_sizable_v<Sizable>);
    EXPECT_TRUE(is_sizable_v<NonConstSizable>);

    EXPECT_FALSE(is_sizable_v<int>);
    EXPECT_FALSE(is_sizable_v<NonIntegerSizable>);
}

struct MyOrdered
{
};

namespace stream {

template <>
struct ordering_traits<MyOrdered>
{
    static constexpr bool is_ordered = true;
};

} // stream namespace

TEST(TEST_CASE_NAME, is_ordered_Test)
{
    struct NonOrdered {};

    EXPECT_TRUE((is_ordered_v<std::map<int, int>>));
    EXPECT_TRUE((is_ordered_v<std::set<int, float>>));
    EXPECT_TRUE(is_ordered_v<MyOrdered>);

    EXPECT_FALSE(is_ordered_v<int>);
    EXPECT_FALSE(is_ordered_v<std::string>);
    EXPECT_FALSE(is_ordered_v<std::vector<float>>);
    EXPECT_FALSE(is_ordered_v<NonOrdered>);
}

TEST(TEST_CASE_NAME, is_iterator_Test)
{
    EXPECT_TRUE(is_iterator_v<int*>);
    EXPECT_TRUE(is_iterator_v<std::vector<int>::iterator>);
    EXPECT_TRUE(is_iterator_v<std::string::const_reverse_iterator>);

    EXPECT_FALSE(is_iterator_v<float>);
}

TEST(TEST_CASE_NAME, is_forward_iterator_Test)
{
    EXPECT_TRUE(is_forward_iterator_v<int*>);
    EXPECT_TRUE(is_forward_iterator_v<std::string::iterator>);
    EXPECT_TRUE(is_forward_iterator_v<std::set<int>::reverse_iterator>);
}

TEST(TEST_CASE_NAME, is_const_iterator_Test)
{
    EXPECT_TRUE(is_const_iterator_v<const int*>);
    EXPECT_TRUE(is_const_iterator_v<std::vector<int>::const_iterator>);
    EXPECT_TRUE(is_const_iterator_v<std::string::const_reverse_iterator>);

    EXPECT_FALSE(is_const_iterator_v<int*>);
    EXPECT_FALSE(is_const_iterator_v<std::vector<int>::reverse_iterator>);
    EXPECT_FALSE(is_const_iterator_v<std::string::iterator>);
}

TEST(TEST_CASE_NAME, is_iterable_Test)
{
    EXPECT_TRUE(is_iterable_v<std::string>);
    EXPECT_TRUE(is_iterable_v<std::vector<int>>);
    EXPECT_TRUE((is_iterable_v<std::map<int, int>>));

    EXPECT_FALSE(is_iterable_v<int*>);
    EXPECT_FALSE(is_iterable_v<long>);
}

TEST(TEST_CASE_NAME, is_reverse_iterable_Test)
{
    EXPECT_TRUE(is_reverse_iterable_v<std::string>);
    EXPECT_TRUE(is_reverse_iterable_v<std::set<int>>);

    EXPECT_FALSE(is_reverse_iterable_v<std::forward_list<int>>);
}
