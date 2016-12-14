#pragma once

namespace stream {

class cstring final
{
public:

    constexpr cstring() noexcept
        : string(nullptr),
          strSize(0)
    {
    }

    template <size_t Size>
    explicit constexpr cstring(const char(&string)[Size]) noexcept
        : string(string),
          strSize(Size - 1)
    {
    }

    constexpr cstring(const cstring&) noexcept = default;
    constexpr cstring(cstring&&) noexcept = default;

    cstring& operator= (const cstring&) = delete;
    cstring& operator= (cstring&&) = delete;

    constexpr size_t size() const noexcept
    {
        return strSize;
    }

    constexpr bool is_empty() const noexcept
    {
        return size() == 0;
    }

    constexpr bool non_empty() const noexcept
    {
        return size() != 0;
    }

    constexpr const char* value() const noexcept
    {
        return string;
    }

private:

    const char* string;
    const size_t strSize;
};

} // stream namespace
