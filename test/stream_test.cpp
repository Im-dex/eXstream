#include "test.hpp"

#include "stream_of.hpp"
#include "collectors/collectors.hpp"

using namespace exstream;
using namespace testing;

#define TEST_CASE_NAME StreamTest

TEST(TEST_CASE_NAME, _Test)
{
    // TODO: check errors on wrong input
    std::vector<int> a = { 1, 2, 3 };

    //stream_of(0).map([](auto e) { return e + 1; });

    auto e = stream_of(a)
        .map([](int x) noexcept { return x + 1; })
        .map([](int x) noexcept { return x + 2; })
        .flat_map([](int x) { return std::vector<int>{ 0, 1, 2, x }; })
        .filter([](int x) { return x > 0; })
        .distinct()
        //.count();
        .collect(to_unordered_multiset());

    EXPECT_THAT(e, UnorderedElementsAre(1, 2, 4, 5, 6));
}
