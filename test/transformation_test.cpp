#include "test.hpp"

#include "stream_of.hpp"
#include "make_array.hpp"
#include "collectors/vector_collector.hpp"

using namespace exstream;
using namespace testing;

#define TEST_CASE_NAME TransformationTest

static const auto test_values = make_array(0, 3, 4, 0, 1, 5, 5, 4);

TEST(TEST_CASE_NAME, distinct_Test)
{
    auto result = stream_of(test_values)
        .distinct()
        .collect(to_vector());

    EXPECT_THAT(result, UnorderedElementsAre(0, 3, 4, 1, 5));
}

TEST(TEST_CASE_NAME, filter_Test)
{
    auto result = stream_of(test_values)
        .filter([](auto x) { return x > 2; })
        .collect(to_vector());

    EXPECT_THAT(result, UnorderedElementsAre(3, 4, 5, 5, 4));
}

TEST(TEST_CASE_NAME, flat_map_Test)
{
    auto result = stream_of(test_values)
        .flat_map([](auto x) { return make_array(1, x); })
        .collect(to_vector());

    const auto expected = make_array(1, 0, 1, 3, 1, 4, 1, 0, 1, 1, 1, 5, 1, 5, 1, 4);
    EXPECT_THAT(result, UnorderedElementsAreArray(expected));
}

TEST(TEST_CASE_NAME, map_Test)
{
    auto result = stream_of(test_values)
        .map([](auto x) { return x + 2; })
        .collect(to_vector());

    EXPECT_THAT(result, ElementsAre(2, 5, 6, 2, 3, 7, 7, 6));
}
