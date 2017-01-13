#include "test.hpp"

#include "option.hpp"

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME OptionTest

struct test_exception : std::exception
{
    test_exception() noexcept
        : exception("Test exception")
    {
    }
};

struct complex_constructible final
{
    complex_constructible(const int a, const double b) noexcept
        : a(a),
          b(b)
    {}

    bool operator== (const complex_constructible& that) const noexcept
    {
        return (a == that.a) && (b == that.b);
    }

    int a;
    double b;
};

struct non_copyable final
{
    non_copyable() = default;
    non_copyable(const non_copyable&) = delete;
    non_copyable(non_copyable&&) = default;

    non_copyable& operator= (const non_copyable&) = delete;
    non_copyable& operator= (non_copyable&&) = default;
};

struct non_movable final
{
    non_movable() = default;
    non_movable(const non_movable&) = default;
    non_movable(non_movable&&) = delete;

    non_movable& operator= (const non_movable&) = default;
    non_movable& operator= (non_movable&&) = delete;
};

TEST(TEST_CASE_NAME, ConstructorTest)
{
    auto value = some(1);

    EXPECT_THAT(option<int>(), Eq(none()));
    EXPECT_THAT(option<int>(), IsEmpty());
    EXPECT_THAT(option<int>(1), Not(IsEmpty()));
    EXPECT_THAT(option<int>(1).get(), Eq(1));

    EXPECT_THAT(option<int>(value).get(), Eq(1));
    EXPECT_THAT(option<int>(some(1)).get(), Eq(1));

    EXPECT_THAT(option<complex_constructible>(in_place, 0, .0).get(), Eq(complex_constructible(0, .0)));
    EXPECT_THAT(make_option<complex_constructible>(0, .0), Eq(complex_constructible(0, .0)));
}

TEST(TEST_CASE_NAME, AssignmentTest)
{
    option<int> value(none());

    EXPECT_THAT(value, Eq(none()));
    
    value = some(2);
    EXPECT_THAT(value.get(), Eq(2));

    auto value2 = some(4);
    value = value2;
    EXPECT_THAT(value.get(), Eq(4));

    value = 10;
    EXPECT_THAT(value.get(), Eq(10));

    int number = 0;
    value = number;
    EXPECT_THAT(value.get(), Eq(number));

    value = none();
    EXPECT_THAT(value, IsEmpty());
}

TEST(TEST_CASE_NAME, emplace_Test)
{
    option<int> value(none());
    value.emplace(22);
    EXPECT_THAT(value.get(), Eq(22));
}

TEST(TEST_CASE_NAME, ComparisonTest)
{
    auto value = some(1);
    auto value2 = some(4);

    EXPECT_THAT(value, Not(Eq(value2)));
    EXPECT_THAT(value, Eq(value));
    EXPECT_THAT(value, Not(Eq(none())));

    EXPECT_THAT(value, Eq(1));
    EXPECT_THAT(value2, Not(Eq(0)));
}

TEST(TEST_CASE_NAME, size_Test)
{
    option<int> empty(none());
    option<int> nonEmpty(some(1));

    EXPECT_THAT(empty.size(), Eq(0));
    EXPECT_THAT(empty, IsEmpty());
    EXPECT_FALSE(empty.non_empty());

    EXPECT_THAT(nonEmpty.size(), Eq(1));
    EXPECT_THAT(nonEmpty, Not(IsEmpty()));
    EXPECT_TRUE(nonEmpty.non_empty());
}

TEST(TEST_CASE_NAME, get_Test)
{
    option<int> empty(none());

    EXPECT_THAT(some(1).get(), Eq(1));
    EXPECT_THAT(some(2).get_or_else(0), Eq(2));
    EXPECT_THAT(empty.get_or_else(2), Eq(2));
    EXPECT_THAT(some(0).get_or_throw<test_exception>(), Eq(0));
    EXPECT_THAT(some(0).get_or_throw(test_exception()), Eq(0));

#ifdef STREAM_DEBUG
    EXPECT_ANY_DEATH(empty.get());
#endif
    EXPECT_THROW(empty.get_or_throw<test_exception>(), test_exception);
    EXPECT_THROW(empty.get_or_throw(test_exception()), test_exception);
}

TEST(TEST_CASE_NAME, swap_Test)
{
    using std::swap;

    auto value = some(2);
    auto value2 = some(10);

    swap(value, value2);
    EXPECT_THAT(value, Eq(10));
    EXPECT_THAT(value2, Eq(2));

    int intValue = 4;
    swap(value, intValue);
    EXPECT_THAT(value, Eq(4));
    EXPECT_THAT(intValue, Eq(10));
}

TEST(TEST_CASE_NAME, hash_Test)
{
    std::hash<int> intHasher;
    std::hash<option<int>> hasher;
    option<int> empty(none());

    EXPECT_THAT(hasher(empty), Eq(0));
    EXPECT_THAT(hasher(some(2)), Eq(intHasher(2)));
}

TEST(TEST_CASE_NAME, IteratorTest)
{
    option<int> empty(none());
    EXPECT_THAT(empty.begin(), Eq(empty.end()));
    EXPECT_THAT(empty.cbegin(), Eq(empty.cend()));
    EXPECT_THAT(empty.rbegin(), Eq(empty.rend()));
    EXPECT_THAT(empty.crbegin(), Eq(empty.crend()));

    auto nonEmpty = some(1);
    EXPECT_THAT(nonEmpty.begin(), Not(Eq(nonEmpty.end())));
    EXPECT_THAT(nonEmpty.cbegin(), Not(Eq(nonEmpty.cend())));
    EXPECT_THAT(nonEmpty.rbegin(), Not(Eq(nonEmpty.rend())));
    EXPECT_THAT(nonEmpty.crbegin(), Not(Eq(nonEmpty.crend())));
    
    EXPECT_THAT(*nonEmpty.begin(), Eq(1));
    EXPECT_THAT(*nonEmpty.cbegin(), Eq(1));
    EXPECT_THAT(*nonEmpty.rbegin(), Eq(1));
    EXPECT_THAT(*nonEmpty.crbegin(), Eq(1));

    EXPECT_THAT(++nonEmpty.begin(), Eq(nonEmpty.end()));
    EXPECT_THAT(++nonEmpty.cbegin(), Eq(nonEmpty.cend()));
    EXPECT_THAT(++nonEmpty.rbegin(), Eq(nonEmpty.rend()));
    EXPECT_THAT(++nonEmpty.crbegin(), Eq(nonEmpty.crend()));

    EXPECT_THAT(nonEmpty.begin()++, Eq(nonEmpty.begin()));
    EXPECT_THAT(nonEmpty.cbegin()++, Eq(nonEmpty.cbegin()));
    EXPECT_THAT(nonEmpty.rbegin()++, Eq(nonEmpty.rbegin()));
    EXPECT_THAT(nonEmpty.crbegin()++, Eq(nonEmpty.crbegin()));

#ifdef STREAM_DEBUG
    EXPECT_ANY_DEATH(*empty.begin());
    EXPECT_ANY_DEATH(*empty.cbegin());
    EXPECT_ANY_DEATH(*empty.rbegin());
    EXPECT_ANY_DEATH(*empty.crbegin());
#endif
}

TEST(TEST_CASE_NAME, CompilationTest)
{
    non_movable value;

    some(non_copyable{});
    some(value);
}
