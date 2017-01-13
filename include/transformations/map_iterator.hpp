#pragma once

#include "transform_iterator.hpp"

namespace cppstream {

template <typename Iterator,
          typename Function,
          typename Meta>
class map_iterator final : public transform_iterator<Iterator>
{
    using function_result = std::result_of_t<const Function&(typename Iterator::reference)>;
public:

    using value_type = remove_cvr_t<function_result>;
    using reference = std::conditional_t<
        std::is_lvalue_reference_v<function_result>,
        function_result,
        std::add_rvalue_reference_t<function_result>
    >;
    using meta = Meta;

    template <typename Allocator>
    explicit map_iterator(const Iterator& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : transform_iterator(iterator),
          function(function)
    {
    }

    template <typename Allocator>
    explicit map_iterator(Iterator&& iterator, const Function& function, const Allocator&) noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        : transform_iterator(std::move(iterator)),
          function(function)
    {
    }

    map_iterator(const map_iterator&) = delete;
    map_iterator(map_iterator&&) = default;

    map_iterator& operator= (const map_iterator&) = delete;
    map_iterator& operator= (map_iterator&&) = delete;

    bool at_end() noexcept(noexcept(std::declval<const Iterator>().at_end()))
    {
        return iterator.at_end();
    }

    void advance() noexcept(noexcept(std::declval<Iterator>().advance()))
    {
        iterator.advance();
    }

    // TODO: replace reference to primitive type with a value type
    reference get_value() noexcept(noexcept(std::declval<const Function&>()(std::declval<typename Iterator::reference>())))
    {
        return function(iterator.get_value());
    }

private:

    const Function& function;
};

} // cppstream namespace
