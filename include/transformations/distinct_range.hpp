#pragma once

#include "transform_range.hpp"

CPPSTREAM_SUPPRESS_ALL_WARNINGS
#include <unordered_set>
CPPSTREAM_RESTORE_ALL_WARNINGS

namespace cppstream {

// TODO: stable for ordered ranges
// TODO: possibility to provide comparator
// TODO: possibility to provide hash
template <typename Range, typename Allocator>
class distinct_range final : public transform_range<Range>
{
public:

    using value_type = typename Range::value_type;

private:

    using set_value = remove_cvr_t<value_type>;
    using set_type = std::unordered_set<set_value, std::hash<set_value>, std::equal_to<set_value>, Allocator>;
    using set_iterator = typename set_type::iterator;

public:

    explicit distinct_range(const Range& range, const Allocator& alloc)
        : transform_range(range),
          selected(false),
          iterator(),
          set(0, alloc)
    {
    }

    explicit distinct_range(Range&& range, const Allocator& alloc)
        : transform_range(std::move(range)),
          selected(false),
          iterator(),
          set(0, alloc)
    {
    }

    bool at_end()
    {
        select();
        return iterator == std::end(set);
    }

    void advance()
    {
        select();
        ++iterator;
    }

    value_type get_value()
    {
        select();
        return std::move(*iterator);
    }

private:

    // TODO: if Range is sizable then rehash with the proper bucket_count

    void select()
    {
        if (selected) return;

        while (!range.at_end())
        {
            set.insert(range.get_value());
        }

        iterator = std::begin(set);
        selected = true;
    }

    bool selected;
    set_iterator iterator;
    set_type set;
};

} // cppstream namespace
