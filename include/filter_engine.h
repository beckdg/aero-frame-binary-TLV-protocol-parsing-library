#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string_view>

namespace aeroframe {

enum class FilterOp : uint8_t {
    Equal, NotEqual, GreaterThan, LessThan,
    GreaterOrEqual, LessOrEqual, BitMask, BitSet, InRange
};

struct FilterCondition {
    uint16_t field_id;
    FilterOp op;
    uint32_t value_a;
    uint32_t value_b;
    uint32_t mask;
};

struct FilterRule {
    uint32_t rule_id;
    uint8_t priority;
    bool enabled;
    std::vector<FilterCondition> conditions;
};

class FilterEngine {
public:
    FilterEngine() noexcept;

    bool add_rule(const FilterRule& rule) noexcept;
    bool load_rules_binary(const uint8_t* data, size_t len) noexcept;
    bool evaluate(uint16_t schema_id, const uint8_t* record,
                  size_t len, std::vector<uint32_t>& matched) noexcept;
    void clear() noexcept;
    size_t rule_count() const noexcept { return rules_.size(); }

private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    bool eval_condition(const FilterCondition& cond,
                        const uint8_t* record, size_t len) const noexcept;
    uint32_t read_field_value(uint16_t field_id,
                              const uint8_t* record, size_t len) const noexcept;

    std::vector<FilterRule> rules_;
    uint32_t eval_count_{0};
};

} // namespace aeroframe
