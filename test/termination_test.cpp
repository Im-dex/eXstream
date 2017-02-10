#include "test.hpp"

#include "stream_of.hpp"
#include "make_array.hpp"
#include "collectors/collectors.hpp"

EXSTREAM_SUPPRESS_ALL_WARNINGS
#include <forward_list>
#include <deque>
#include <queue>
#include <stack>
EXSTREAM_RESTORE_ALL_WARNINGS

using namespace exstream;
using namespace testing;

#define TEST_CASE_NAME TerminationTest

static const auto test_values = make_array(4, 10, 2, 9, 4, 0);

TEST(TEST_CASE_NAME, count_Test)
{
    EXPECT_THAT(stream_of(test_values).count(), Eq(test_values.size()));

    const auto result = stream_of(test_values)
        .filter([](auto x) { return x > 2; })
        .count();

    EXPECT_THAT(result, Eq(4));
}

TEST(TEST_CASE_NAME, fill_Test)
{
    std::vector<int> vector;

    stream_of(test_values).fill(std::back_inserter(vector));
    EXPECT_THAT(vector, ElementsAreArray(test_values));

    std::set<int> set;

    stream_of(test_values)
        .distinct()
        .fill(std::inserter(set, std::begin(set)));
    EXPECT_THAT(set, ElementsAre(0, 2, 4, 9, 10));
}

TEST(TEST_CASE_NAME, foreach_Test)
{
    std::vector<int> values;

    stream_of(test_values)
        .foreach([&](auto x)
        {
            values.push_back(x);
        });

    EXPECT_THAT(values, ElementsAreArray(test_values));
}

template <typename T>
struct custom_builder
{
    custom_builder() = default;
    custom_builder(const custom_builder&) = delete;
    custom_builder& operator= (const custom_builder&) = delete;

    MOCK_METHOD1_T(reserve, void(size_t));

    MOCK_METHOD1_T(append, void(const T&));

    void append(T&& value) // NOTE: Gmock lack of rvalue support
    {
        append_rvalue(value);
    }

    MOCK_METHOD1_T(append_rvalue, void(const T&));

    MOCK_METHOD0_T(build, int());
};

// NOTE: GMock mock is noncopyable and nonmovable, so the wrapper solve that issue
template <typename T>
struct custom_builder_wrapper
{
    explicit custom_builder_wrapper(custom_builder<T>& builder)
        : builder(&builder)
    {
    }

    void reserve(const size_t size)
    {
        builder->reserve(size);
    }

    void append(const T& value)
    {
        builder->append(value);
    }

    void append(T&& value)
    {
        builder->append(std::move(value));
    }

    T build()
    {
        return builder->build();
    }

    custom_builder<T>* builder;
};

struct custom_collector
{
    custom_collector() = default;
    custom_collector(const custom_collector&) = delete;
    custom_collector& operator= (const custom_collector&) = delete;

    template <typename T>
    custom_builder_wrapper<T> builder(type_t<T>);

    template <>
    custom_builder_wrapper<int> builder(type_t<int>)
    {
        return int_builder();
    }

    MOCK_CONST_METHOD0(int_builder, custom_builder_wrapper<int>());
};

TEST(TEST_CASE_NAME, collect_Test)
{
    using testing::StrictMock;

    custom_builder<int> builder;
    {
        EXPECT_CALL(builder, reserve(_))
            .Times(Exactly(1));

        EXPECT_CALL(builder, append(An<const int&>()))
            .Times(Exactly(static_cast<int>(test_values.size())));

        EXPECT_CALL(builder, append_rvalue(_))
            .Times(Exactly(0));

        EXPECT_CALL(builder, build())
            .Times(Exactly(1))
            .WillOnce(Return(42));
    }

    custom_collector collector;
    {
        EXPECT_CALL(collector, int_builder())
            .Times(Exactly(1))
            .WillOnce(Return(custom_builder_wrapper<int>(builder)));
    }

    auto result = stream_of(test_values).collect(collector);
    EXPECT_THAT(result, Eq(42));
}

namespace std {

template <typename T, typename Container, typename Compare>
bool operator== (priority_queue<T, Container, Compare> lhs, priority_queue<T, Container, Compare> rhs)
{
    if (lhs.size() != rhs.size()) return false;

    while (!lhs.empty())
    {
        if (lhs.top() != rhs.top()) return false;
        lhs.pop();
        rhs.pop();
    }

    return true;
}

} // std namespace

TEST(TEST_CASE_NAME, collectors_Test)
{
    EXPECT_THAT(stream_of(test_values).collect(to_vector()), ElementsAreArray(test_values));
    EXPECT_THAT(stream_of(test_values).collect(to_list()), ElementsAreArray(test_values));
    EXPECT_THAT(stream_of(test_values).collect(to_forward_list()), ElementsAreArray(test_values));
    EXPECT_THAT(stream_of(test_values).collect(to_deque()), ElementsAreArray(test_values));

    EXPECT_THAT(stream_of(test_values).collect(to_set()), ElementsAre(0, 2, 4, 9, 10));
    EXPECT_THAT(stream_of(test_values).collect(to_multiset()), ElementsAre(0, 2, 4, 4, 9, 10));
    EXPECT_THAT(stream_of(test_values).collect(to_unordered_set()), UnorderedElementsAre(0, 2, 4, 9, 10));
    EXPECT_THAT(stream_of(test_values).collect(to_unordered_multiset()), UnorderedElementsAreArray(test_values));

    // TODO: map test (depends on zip)

    {
        std::queue<int> queue;
        for (const auto value : test_values) queue.push(value);
        EXPECT_THAT(stream_of(test_values).collect(to_queue()), Eq(queue));
    }

    {
        std::stack<int> stack;
        for (const auto value : test_values) stack.push(value);
        EXPECT_THAT(stream_of(test_values).collect(to_stack()), Eq(stack));
    }

    {
        std::priority_queue<int> queue;
        for (const auto value : test_values) queue.push(value);
        EXPECT_THAT(stream_of(test_values).collect(to_priority_queue()), Eq(queue));
    }
}

TEST(TEST_CASE_NAME, collectors_with_arg_Test)
{
    // TODO:
}
