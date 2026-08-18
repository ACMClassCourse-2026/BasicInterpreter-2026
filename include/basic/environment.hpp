#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "basic/array_value.hpp"

namespace basic {

struct VariableSlot {
    BasicType type{BasicType::Single};
    bool is_array{false};
    Value scalar;
    std::optional<ArrayValue> array;
};

class Environment {
public:
    Environment();

    void push_scope();
    void pop_scope();
    void clear();
    std::size_t scope_depth() const noexcept;

    void declare_scalar(const std::string& name, BasicType type,
                        const Value& initial_value);
    void declare_fixed_array(const std::string& name, BasicType type,
                             const std::vector<std::size_t>& dimensions);
    void declare_dynamic_array(const std::string& name, BasicType type);
    void assign(const std::string& name, const Value& value);
    void assign_element(const std::string& name,
                        const std::vector<std::int64_t>& indices,
                        const Value& value);
    const Value& read(const std::string& name) const;
    const Value& read_element(const std::string& name,
                              const std::vector<std::int64_t>& indices) const;
    void redim(const std::string& name, std::size_t length, bool preserve);
    void erase_array(const std::string& name);

private:
    VariableSlot& find_slot(const std::string& name);
    const VariableSlot& find_slot(const std::string& name) const;

    std::vector<std::unordered_map<std::string, VariableSlot>> scopes_;
};

}  // namespace basic
