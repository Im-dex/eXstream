#include "test.hpp"

#include "detail/scope_guard.hpp"

using namespace cppstream;
using namespace testing;

#define TEST_CASE_NAME ScopeGuardTest

struct call_mock : Mock
{
    call_mock() = default;
    call_mock(const call_mock&) = delete;
    call_mock& operator= (const call_mock&) = delete;

    MOCK_CONST_METHOD0(expected, void());
    MOCK_CONST_METHOD0(unexpected, void());
};

TEST(TEST_CASE_NAME, scope_exit_Test)
{
    call_mock mock;
    EXPECT_CALL(mock, expected()).Times(Exactly(2));

    {
        CPPSTREAM_SCOPE_EXIT { mock.expected(); };
    }

    EXPECT_ANY_THROW(
        CPPSTREAM_SCOPE_EXIT{ mock.expected(); };
        throw 0;
    );
}

TEST(TEST_CASE_NAME, scope_success_Test)
{
    call_mock mock;
    EXPECT_CALL(mock, expected()).Times(Exactly(1));
    EXPECT_CALL(mock, unexpected()).Times(Exactly(0));

    {
        CPPSTREAM_SCOPE_SUCCESS { mock.expected(); };
    }

    EXPECT_ANY_THROW(
        CPPSTREAM_SCOPE_SUCCESS{ mock.unexpected(); };
        throw 0;
    );
}

TEST(TEST_CASE_NAME, scope_fail_Test)
{
    call_mock mock;
    EXPECT_CALL(mock, expected()).Times(Exactly(1));
    EXPECT_CALL(mock, unexpected()).Times(Exactly(0));

    {
        CPPSTREAM_SCOPE_FAIL{ mock.unexpected(); };
    }

    EXPECT_ANY_THROW(
        CPPSTREAM_SCOPE_FAIL{ mock.expected(); };
        throw 0;
    );

    CPPSTREAM_MSVC_SUPPRESS_WARNINGS_PUSH(4297)

    EXPECT_ANY_DEATH(
        CPPSTREAM_SCOPE_FAIL{ throw 1; };
        throw 0;
    );

    CPPSTREAM_MSVC_WARNINGS_POP
}
