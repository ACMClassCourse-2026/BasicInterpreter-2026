#pragma once

#include <cstddef>
#include <vector>

#include "basic/value.hpp"

namespace basic {

class ArrayValue {
public:
    ArrayValue(BasicType element_type, bool dynamic,
               const std::vector<std::size_t>& dimensions = {});

    BasicType element_type() const noexcept;
    bool is_dynamic() const noexcept;
    bool is_dimensioned() const noexcept;
    const std::vector<std::size_t>& dimensions() const noexcept;
    const Value& at(const std::vector<std::int64_t>& indices) const;
    Value& at(const std::vector<std::int64_t>& indices);
    void redim(std::size_t length, bool preserve);
    void erase();

private:
    std::size_t flat_index(const std::vector<std::int64_t>& indices) const;

    BasicType element_type_;
    bool dynamic_{false};
    bool dimensioned_{false};
    std::vector<std::size_t> dimensions_;
    std::vector<Value> elements_;
};

}  // namespace basic
