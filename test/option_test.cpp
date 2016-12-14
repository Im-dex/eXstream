#include "test.hpp"

#include "option.hpp"

using namespace stream;

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
    ComplexConstructible(const int a, const double b)
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

    EXPECT_EQ(option<int>{}, none());
    EXPECT_TRUE(option<int>{}.is_empty());
    EXPECT_EQ(option<int>(1).get(), 1);
    EXPECT_TRUE(option<int>(1).non_empty());

    EXPECT_EQ(option<int>(value).get(), 1);
    EXPECT_EQ(option<int>(some(1)).get(), 1);

    EXPECT_EQ(option<ComplexConstructible>(in_place{}, 0, .0).get(), ComplexConstructible(0, .0));
    EXPECT_EQ(make_option<ComplexConstructible>(0, .0), ComplexConstructible(0, .0));
}

TEST(TEST_CASE_NAME, AssignmentTest)
{
    option<int> value(none());

    EXPECT_EQ(value, none());
    
    value = some(2);
    EXPECT_EQ(value.get(), 2);

    auto value2 = some(4);
    value = value2;
    EXPECT_EQ(value.get(), 4);

    value = 10;
    EXPECT_EQ(value.get(), 10);

    int number = 0;
    value = number;
    EXPECT_EQ(value.get(), number);

    value = none();
    EXPECT_TRUE(value.is_empty());
}

TEST(TEST_CASE_NAME, ComparisonTest)
{
    auto value = some(1);
    auto value2 = some(4);

    EXPECT_NE(value, value2);
    EXPECT_EQ(value, value);
    EXPECT_NE(value, none());

    EXPECT_EQ(value, 1);
    EXPECT_NE(value2, 0);
}

TEST(TEST_CASE_NAME, SizeTest)
{
    option<int> empty(none());
    option<int> nonEmpty(some(1));

    EXPECT_EQ(empty.size(), 0);
    EXPECT_TRUE(empty.is_empty());
    EXPECT_FALSE(empty.non_empty());

    EXPECT_EQ(nonEmpty.size(), 1);
    EXPECT_FALSE(nonEmpty.is_empty());
    EXPECT_TRUE(nonEmpty.non_empty());
}

TEST(TEST_CASE_NAME, GetTest)
{
    option<int> empty(none());

    EXPECT_EQ(some(1).get(), 1);
    EXPECT_EQ(some(2).get_or_else(0), 2);
    EXPECT_EQ(empty.get_or_else(2), 2);
    EXPECT_EQ(some(0).get_or_throw<TestException>(), 0);
    EXPECT_EQ(some(0).get_or_throw(TestException()), 0);

    EXPECT_THROW(empty.get(), no_such_element_exception);
    EXPECT_THROW(empty.get_or_throw<TestException>(), TestException);
    EXPECT_THROW(empty.get_or_throw(TestException()), TestException);
}

TEST(TEST_CASE_NAME, SwapTest)
{
    auto value = some(2);
    auto value2 = some(10);

    std::swap(value, value2);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(value2, 2);

    int intValue = 4;
    std::swap(value, intValue);
    EXPECT_EQ(value, 4);
    EXPECT_EQ(intValue, 10);
}

TEST(TEST_CASE_NAME, HashTest)
{
    std::hash<int> intHasher;
    std::hash<option<int>> hasher;
    option<int> empty(none());

    EXPECT_EQ(hasher(empty), 0);
    EXPECT_EQ(hasher(some(2)), intHasher(2));
}

TEST(TEST_CASE_NAME, IteratorTest)
{
    option<int> empty(none());
    EXPECT_TRUE(empty.iterator().is_empty());
    EXPECT_TRUE(empty.const_iterator().is_empty());
    EXPECT_TRUE(empty.reverse_iterator().is_empty());

    EXPECT_FALSE(empty.iterator().non_empty());
    EXPECT_FALSE(empty.const_iterator().non_empty());
    EXPECT_FALSE(empty.reverse_iterator().non_empty());

    auto nonEmpty = some(1);
    EXPECT_FALSE(nonEmpty.iterator().is_empty());
    EXPECT_FALSE(nonEmpty.const_iterator().is_empty());
    EXPECT_FALSE(nonEmpty.reverse_iterator().is_empty());

    EXPECT_TRUE(nonEmpty.iterator().non_empty());
    EXPECT_TRUE(nonEmpty.const_iterator().non_empty());
    EXPECT_TRUE(nonEmpty.reverse_iterator().non_empty());
    
    EXPECT_EQ(nonEmpty.iterator().value(), 1);
    EXPECT_EQ(nonEmpty.const_iterator().value(), 1);
    EXPECT_EQ(nonEmpty.reverse_iterator().value(), 1);

    {
        auto iter = nonEmpty.iterator();
        iter.advance();
        EXPECT_TRUE(iter.is_empty());

        auto constIter = nonEmpty.const_iterator();
        constIter.advance();
        EXPECT_TRUE(constIter.is_empty());

        auto reverseIter = nonEmpty.reverse_iterator();
        reverseIter.advance();
        EXPECT_TRUE(reverseIter.is_empty());
    }

    EXPECT_NO_FATAL_FAILURE(empty.iterator().advance());
    EXPECT_NO_FATAL_FAILURE(empty.const_iterator().advance());
    EXPECT_NO_FATAL_FAILURE(empty.reverse_iterator().advance());

#ifdef STREAM_DEBUG
    EXPECT_ANY_DEATH(empty.iterator().value());
    EXPECT_ANY_DEATH(empty.const_iterator().value());
    EXPECT_ANY_DEATH(empty.reverse_iterator().value());
#endif
}

TEST(TEST_CASE_NAME, CompilationTest)
{
    NonMovable value;

    some(NonCopyable{});
    some(value);
}
