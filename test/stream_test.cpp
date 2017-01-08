#include "test.hpp"

#include "stream_of.hpp"

using namespace cppstream;

#define TEST_CASE_NAME StreamTest

TEST(TEST_CASE_NAME, _Test)
{
    // TODO: check errors on wrong input
    std::vector<int> a = { 1, 2, 3 };

    stream_of(0).map([](auto e) { return e + 1; });

    /*stream_of(a)
        .map([](int x) noexcept { return x + 1; })
        .map([](int x) noexcept { return x + 2; })
        .flat_map([](int x) { return std::vector<int>{ 0, 1, 2, x }; });*/
}
