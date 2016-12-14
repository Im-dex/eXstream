#include "test.hpp"

#include "detail/traits.hpp"
#include "iterable.hpp"

using namespace stream;

#define TEST_CASE_NAME TraitsTest

struct Comparable
{
    bool operator== (const Comparable&) const noexcept { return true; }
};

struct Incrementable
{
    Incrementable& operator++ (int) { return *this; }
    Incrementable operator++ () { return *this; }
};

struct Dereferencable
{
    int operator* () { return 0; }
};

struct MyIterator : public Comparable, public Incrementable, public Dereferencable
{
};

namespace std {

template <>
struct iterator_traits<MyIterator>
{
    using value_type = int;
};

} // std namespace

TEST(TEST_CASE_NAME, is_range_iterator_Test)
{
    EXPECT_TRUE(is_range_iterator_v<int*>);
    EXPECT_TRUE(is_range_iterator_v<MyIterator>);
    EXPECT_TRUE(is_range_iterator_v<std::vector<int>::iterator>);
    EXPECT_TRUE(is_range_iterator_v<std::vector<int>::const_iterator>);
    EXPECT_TRUE((is_range_iterator_v<std::map<int, float>::iterator>));
    EXPECT_TRUE((is_range_iterator_v<std::map<int, float>::const_iterator>));

    EXPECT_FALSE(is_range_iterator_v<int>);
    EXPECT_FALSE(is_range_iterator_v<Comparable>);
    EXPECT_FALSE(is_range_iterator_v<Incrementable>);
    EXPECT_FALSE(is_range_iterator_v<Dereferencable>);
}

TEST(TEST_CASE_NAME, is_range_Test)
{
    struct NonStreamable {};

    EXPECT_TRUE(is_range_v<std::vector<int>>);
    EXPECT_TRUE(is_range_v<std::string>);
    EXPECT_TRUE(is_range_v<std::initializer_list<int>>);

    EXPECT_FALSE(is_range_v<int>);
    EXPECT_FALSE(is_range_v<NonStreamable>);
}

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

template <bool Const>
struct custom_iterator : public iterator<int, Const>
{
    int number = 0;

    bool is_empty() const override { return false; }
    bool non_empty() const override { return true; }
    std::conditional_t<Const, const int&, int&> value() override { return number; }
    const int& value() const override { return number; }
    void advance() override {}

    bool operator== (const custom_iterator&) { return true; }
};

TEST(TEST_CASE_NAME, is_iterator_Test)
{
    struct non_comparable_iterator : public iterator<int, true>
    {
        bool is_empty() const override { return false; }
        bool non_empty() const override { return true; }
        const int& value() override { return 0; }
        const int& value() const override { return 0; }
        void advance() override {}
    };

    EXPECT_TRUE(is_iterator_v<custom_iterator<true>>);
    EXPECT_TRUE(is_iterator_v<custom_iterator<false>>);

    EXPECT_FALSE(is_iterator_v<std::string::iterator>);
    EXPECT_FALSE(is_iterator_v<int*>);
    EXPECT_FALSE(is_iterator_v<non_comparable_iterator>);
}

TEST(TEST_CASE_NAME, is_const_iterator_Test)
{
    EXPECT_TRUE(is_const_iterator_v<custom_iterator<true>>);
    EXPECT_FALSE(is_const_iterator_v<custom_iterator<false>>);

    EXPECT_FALSE(is_const_iterator_v<std::string::iterator>);
    EXPECT_FALSE(is_const_iterator_v<int*>);
}

TEST(TEST_CASE_NAME, is_iterable_Test)
{
    struct custom_iterable : public iterable<int, custom_iterator<false>, custom_iterator<true>>
    {
        custom_iterator<false> iterator() override { return custom_iterator<false>(); }
        custom_iterator<true> iterator() const override { return custom_iterator<true>(); }
        custom_iterator<true> const_iterator() override { return custom_iterator<true>(); }
    };

    EXPECT_TRUE(is_iterable_v<custom_iterable>);

    EXPECT_FALSE(is_iterable_v<std::string>);
    EXPECT_FALSE(is_iterable_v<std::vector<int>>);
    EXPECT_FALSE(is_iterable_v<int*>);
}
