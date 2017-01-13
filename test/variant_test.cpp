#include "test.hpp"

#include "variant.hpp"

using namespace exstream;
using namespace testing;
using namespace std::string_literals;

#define TEST_CASE_NAME VariantTestCase

struct custom_type final
{
    int a;
    float b;

    custom_type(int a, float b) noexcept
        : a(a),
          b(b)
    {
    }

    bool operator== (const custom_type& that) const noexcept
    {
        return (a == that.a) && (b == that.b);
    }
};

namespace std {

template <>
struct hash<custom_type>
{
    size_t operator() (const custom_type&) const noexcept
    {
        return size_t(42);
    }
};

} // std namespace

struct throw_on_assign
{
    throw_on_assign() = default;
    throw_on_assign(const throw_on_assign&) = default;
    throw_on_assign(throw_on_assign&&) = default;

    throw_on_assign& operator= (const throw_on_assign&)
    {
        throw 0;
    }

    throw_on_assign& operator= (throw_on_assign&&)
    {
        throw 0;
    }
};

using var = variant<int, std::string, custom_type>;
using valueless_var = variant<int, throw_on_assign>;

TEST(TEST_CASE_NAME, ConstructorTest)
{
    EXPECT_THAT(var(type_t<int>{}).get<int>(), Eq(0));
    EXPECT_THAT(var("22"s).get<std::string>(), Eq("22"s));
    EXPECT_THAT(var(in_place_type_t<custom_type>(), 1, 0.f).get<custom_type>(), Eq(custom_type{ 1, 0.f }));

    var value("str"s);
    EXPECT_THAT(var(value).get<std::string>(), Eq("str"s));

    EXPECT_THAT(var(std::move(value)).get<std::string>(), Eq("str"s));
    EXPECT_THAT(value.get<std::string>(), IsEmpty());
}

TEST(TEST_CASE_NAME, AssignmentTest)
{
    {
        var value("str"s);
        var that(4);
        value = that;

        EXPECT_THAT(value.get<int>(), Eq(that.get<int>()));
    }
    
    {
        var value(4);
        var that("str"s);
        value = std::move(that);

        EXPECT_THAT(value.get<std::string>(), Eq("str"s));
        EXPECT_THAT(that.get<std::string>(), Eq(""s));
    }

    {
        var value(3);
        value = "str"s;
        EXPECT_THAT(value.get<std::string>(), Eq("str"s));

        auto str = "some_string"s;
        value = str;
        EXPECT_THAT(value.get<std::string>(), Eq(str));
    }
}

TEST(TEST_CASE_NAME, emplace_Test)
{
    var value(12);

    value.emplace(in_place_type_t<std::string>(), "string");
    EXPECT_THAT(value.get<std::string>(), Eq("string"s));

    value.emplace(in_place_type_t<int>(), 0);
    EXPECT_THAT(value.get<int>(), Eq(0));
}

TEST(TEST_CASE_NAME, valueless_by_exceptions_Test)
{
    {
        valueless_var value(in_place_type_t<throw_on_assign>{});

        EXPECT_THROW(value = throw_on_assign(), int);
        EXPECT_TRUE(value.is_valueless_by_exception());

        value = 3;
        EXPECT_THAT(value.get<int>(), Eq(3));
        EXPECT_FALSE(value.is_valueless_by_exception());
    }

    {
        throw_on_assign that;
        valueless_var value(in_place_type_t<throw_on_assign>{});

        EXPECT_THROW(value = that, int);
        EXPECT_TRUE(value.is_valueless_by_exception());

        value = 42;
        EXPECT_THAT(value.get<int>(), Eq(42));
        EXPECT_FALSE(value.is_valueless_by_exception());
    }
}

TEST(TEST_CASE_NAME, index_Test)
{
    var value(4);
    EXPECT_THAT(value.index(), Eq(0));

    value = "str"s;
    EXPECT_THAT(value.index(), Eq(1));

    value = custom_type(0, 0.f);
    EXPECT_THAT(value.index(), Eq(2));
}

TEST(TEST_CASE_NAME, contains_Test)
{
    var value(4);
    EXPECT_TRUE(value.contains<int>());
    EXPECT_FALSE(value.contains<std::string>());
    EXPECT_FALSE(value.contains<custom_type>());

    value = "str"s;
    EXPECT_FALSE(value.contains<int>());
    EXPECT_TRUE(value.contains<std::string>());
    EXPECT_FALSE(value.contains<custom_type>());

    value = custom_type(0, 0.f);
    EXPECT_FALSE(value.contains<int>());
    EXPECT_FALSE(value.contains<std::string>());
    EXPECT_TRUE(value.contains<custom_type>());
}

TEST(TEST_CASE_NAME, get_Test)
{
    var value("str"s);
    EXPECT_THAT(value.get<std::string>(), Eq("str"s));

#ifdef EXSTREAM_DEBUG
    EXPECT_ANY_DEATH(value.get<int>());
#endif

    EXPECT_THAT(value.get_if<custom_type>(), IsEmpty());
    EXPECT_THAT(value.get_if<std::string>().get(), Eq("str"s));
}

TEST(TEST_CASE_NAME, match_Test)
{
    enum class MatchResult
    {
        Int,
        String,
        Custom
    };

    {
        auto result = var(10).match([](auto&& value)
        {
            using type = std::decay_t<decltype(value)>;
            EXSTREAM_UNUSED(value);

            return constexpr_if<std::is_same_v<type, int>>()
                .then([&](auto) {
                    EXPECT_THAT(value, Eq(10));
                    return MatchResult::Int;
                })
                .else_if<(std::is_same_v<type, std::string>)>()
                    .then([](auto) {
                        return MatchResult::String;
                    })
                    .else_([](auto) {
                        return MatchResult::Custom;
                    })(nothing);
        });

        EXPECT_THAT(result, Eq(MatchResult::Int));
    }

    {
        const var value("str"s);
        auto result = value.match([](auto&& value)
        {
            using type = std::decay_t<decltype(value)>;
            EXSTREAM_UNUSED(value);

            return constexpr_if<std::is_same_v<type, int>>()
                .then([](auto) {
                    return MatchResult::Int;
                })
                .else_if<(std::is_same_v<type, std::string>)>()
                    .then([&](auto) {
                        EXPECT_THAT(value, Eq("str"s));
                        return MatchResult::String;
                    })
                    .else_([](auto) {
                        return MatchResult::Custom;
                    })(nothing);
        });

        EXPECT_THAT(result, Eq(MatchResult::String));
    }
}

TEST(TEST_CASE_NAME, valueless_by_exception_match_Test)
{
    valueless_var value(throw_on_assign{});
    EXPECT_THROW(value = throw_on_assign(), int);
    EXPECT_THROW(value.match([](auto&&) {}), bad_variant_access);
}

TEST(TEST_CASE_NAME, swap_Test)
{
    {
        var value("str"s);
        var that(custom_type(0, 0.f));
        swap(value, that);

        EXPECT_THAT(value.get<custom_type>(), Eq(custom_type(0, 0.f)));
        EXPECT_THAT(that.get<std::string>(), Eq("str"s));
    }

    {
        var value(4);
        var that(3);
        swap(value, that);

        EXPECT_THAT(value.get<int>(), Eq(3));
        EXPECT_THAT(that.get<int>(), Eq(4));
    }
}

TEST(TEST_CASE_NAME, hash_Test)
{
    std::hash<var> hash;
    std::hash<int> intHash;
    std::hash<std::string> strHash;
    std::hash<custom_type> customHash;

    EXPECT_THAT(hash(var(42)), Eq(intHash(42)));
    EXPECT_THAT(hash(var("str"s)), Eq(strHash("str"s)));
    EXPECT_THAT(hash(var(custom_type(0, 1.f))), Eq(customHash(custom_type(0, 1.f))));
}
