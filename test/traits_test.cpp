#include "test.hpp"

#include "detail/traits.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <forward_list>
#include <unordered_map>
#include <unordered_set>
EXSTREAM_RESTORE_ALL_WARNINGS

using namespace exstream;

#define TEST_CASE_NAME TraitsTest

TEST(TEST_CASE_NAME, is_sizable_Test)
{
    struct sizable
    {
        std::size_t size() const { return 1; }
    };

    struct non_const_sizable
    {
        std::size_t size() { return 1; }
    };

    struct non_integer_sizable
    {
        float size() { return 1.0f; }
    };

    EXPECT_TRUE(is_sizable_v<std::string>);
    EXPECT_TRUE(is_sizable_v<std::vector<int>>);
    EXPECT_TRUE(is_sizable_v<sizable>);

    EXPECT_FALSE(is_sizable_v<non_const_sizable>);
    EXPECT_FALSE(is_sizable_v<int>);
    EXPECT_FALSE(is_sizable_v<non_integer_sizable>);
}

struct my_ordered
{
};

struct my_distinct
{
};

namespace exstream {

template <>
struct container_traits<my_ordered>
{
    static constexpr bool is_ordered = true;
    static constexpr Order order = Order::Ascending;
};

template <>
struct container_traits<my_distinct>
{
    static constexpr bool is_ordered = false;
    static constexpr bool is_distinct = true;
};

} // exstream namespace

TEST(TEST_CASE_NAME, is_ordered_Test)
{
    struct unordered {};

    EXPECT_TRUE((is_ordered_v<std::map<int, int>>));
    EXPECT_TRUE((is_ordered_v<std::set<int, float>>));
    EXPECT_TRUE(is_ordered_v<my_ordered>);

    EXPECT_FALSE(is_ordered_v<int>);
    EXPECT_FALSE(is_ordered_v<std::string>);
    EXPECT_FALSE(is_ordered_v<std::vector<float>>);
    EXPECT_FALSE(is_ordered_v<unordered>);
}

TEST(TEST_CASE_NAME, is_distinct_Test)
{
    struct non_distinct {};

    EXPECT_TRUE((is_distinct_v<std::unordered_map<int, int>>));
    EXPECT_TRUE((is_distinct_v<std::unordered_set<int, int>>));
    EXPECT_TRUE((is_distinct_v<std::map<int, int>>));
    EXPECT_TRUE((is_distinct_v<std::set<int, int>>));
    EXPECT_TRUE((is_distinct_v<my_distinct>));

    EXPECT_FALSE((is_distinct_v<int>));
    EXPECT_FALSE((is_distinct_v<std::string>));
    EXPECT_FALSE((is_distinct_v<std::multimap<int, int>>));
    EXPECT_FALSE((is_distinct_v<non_distinct>));
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
    EXPECT_TRUE(is_forward_iterator_v<std::list<int>::iterator>);
    EXPECT_TRUE(is_forward_iterator_v<std::set<int>::reverse_iterator>);

    EXPECT_FALSE(is_forward_iterator_v<int>);
}

TEST(TEST_CASE_NAME, is_random_access_iterator_Test)
{
    EXPECT_TRUE(is_random_access_iterator_v<int*>);
    EXPECT_TRUE(is_random_access_iterator_v<std::string::iterator>);
    EXPECT_TRUE(is_random_access_iterator_v<std::vector<int>::iterator>);

    EXPECT_FALSE(is_random_access_iterator_v<int>);
    EXPECT_FALSE(is_random_access_iterator_v<std::list<int>>);
    EXPECT_FALSE(is_random_access_iterator_v<std::forward_list<int>>);
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
