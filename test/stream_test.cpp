#include "test.hpp"

#include "source.hpp"

using namespace cppstream;

#define TEST_CASE_NAME StreamTest

TEST(TEST_CASE_NAME, _Test)
{
    std::vector<int> a = { 1, 2, 3 };
    source(a).map([](auto x) { return x + 1; });

    /*source(a).map([](auto v) { return v + 1; })
             .map([](auto x) { return x + 1; });*/
}
