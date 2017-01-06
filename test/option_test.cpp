#include "test.hpp"

#include "option.hpp"

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME OptionTest

struct TestException : public std::exception
{
    TestException() noexcept
        : exception("Test exception")
    {
    }
};

struct ComplexConstructible final
{
    ComplexConstructible(const int a, const double b) noexcept
        : a(a),
          b(b)
    {}

    bool operator== (const ComplexConstructible& that) const noexcept
    {
        return (a == that.a) && (b == that.b);
    }

    int a;
    double b;
};

struct NonCopyable final
{
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;

    NonCopyable& operator= (const NonCopyable&) = delete;
    NonCopyable& operator= (NonCopyable&&) = default;
};

struct NonMovable final
{
    NonMovable() = default;
    NonMovable(const NonMovable&) = default;
    NonMovable(NonMovable&&) = delete;

    NonMovable& operator= (const NonMovable&) = default;
    NonMovable& operator= (NonMovable&&) = delete;
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

    EXPECT_THAT(option<ComplexConstructible>(in_place, 0, .0).get(), Eq(ComplexConstructible(0, .0)));
    EXPECT_THAT(make_option<ComplexConstructible>(0, .0), Eq(ComplexConstructible(0, .0)));
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

TEST(TEST_CASE_NAME, SizeTest)
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

TEST(TEST_CASE_NAME, GetTest)
{
    option<int> empty(none());

    EXPECT_THAT(some(1).get(), Eq(1));
    EXPECT_THAT(some(2).get_or_else(0), Eq(2));
    EXPECT_THAT(empty.get_or_else(2), Eq(2));
    EXPECT_THAT(some(0).get_or_throw<TestException>(), Eq(0));
    EXPECT_THAT(some(0).get_or_throw(TestException()), Eq(0));

#ifdef STREAM_DEBUG
    EXPECT_ANY_DEATH(empty.get());
#endif
    EXPECT_THROW(empty.get_or_throw<TestException>(), TestException);
    EXPECT_THROW(empty.get_or_throw(TestException()), TestException);
}

TEST(TEST_CASE_NAME, SwapTest)
{
    auto value = some(2);
    auto value2 = some(10);

    std::swap(value, value2);
    EXPECT_THAT(value, Eq(10));
    EXPECT_THAT(value2, Eq(2));

    int intValue = 4;
    std::swap(value, intValue);
    EXPECT_THAT(value, Eq(4));
    EXPECT_THAT(intValue, Eq(10));
}

TEST(TEST_CASE_NAME, HashTest)
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
    NonMovable value;

    some(NonCopyable{});
    some(value);
}
