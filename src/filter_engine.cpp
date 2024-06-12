#include "filter_engine.h"
#include <algorithm>
#include <cstring>

namespace aeroframe {

FilterEngine::FilterEngine() noexcept : eval_count_(0) {}

uint16_t FilterEngine::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t FilterEngine::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool FilterEngine::add_rule(const FilterRule& rule) noexcept {
    if (rule.conditions.empty()) return false;
    rules_.push_back(rule);
    return true;
}

bool FilterEngine::load_rules_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    if (data[0] != 0x46 || data[1] != 0x52) return false;
    uint16_t rule_count = read_u16(data + 2);
    size_t offset = 4;
    for (uint16_t r = 0; r < rule_count; ++r) {
        if (offset + 8 > len) return false;
        FilterRule rule{};
        rule.rule_id = read_u32(data + offset);
        rule.priority = data[offset + 4];
        rule.enabled = data[offset + 5] != 0;
        uint16_t cond_count = read_u16(data + offset + 6);
        offset += 8;
        for (uint16_t c = 0; c < cond_count; ++c) {
            if (offset + 14 > len) return false;
            FilterCondition cond{};
            cond.field_id = read_u16(data + offset);
            cond.op = static_cast<FilterOp>(data[offset + 2]);
            cond.value_a = read_u32(data + offset + 3);
            cond.value_b = read_u32(data + offset + 7);
            cond.mask = read_u32(data + offset + 11);
            offset += 14;
            rule.conditions.push_back(cond);
        }
        add_rule(rule);
    }
    return true;
}

uint32_t FilterEngine::read_field_value(uint16_t field_id,
                                        const uint8_t* record, size_t len) const noexcept {
    size_t offset = static_cast<size_t>(field_id) * 4;
    if (offset + 4 > len) return 0;
    return read_u32(record + offset);
}

bool FilterEngine::eval_condition(const FilterCondition& cond,
                                   const uint8_t* record, size_t len) const noexcept {
    uint32_t val = read_field_value(cond.field_id, record, len);
    switch (cond.op) {
        case FilterOp::Equal: return val == cond.value_a;
        case FilterOp::NotEqual: return val != cond.value_a;
        case FilterOp::GreaterThan: return val > cond.value_a;
        case FilterOp::LessThan: return val < cond.value_a;
        case FilterOp::GreaterOrEqual: return val >= cond.value_a;
        case FilterOp::LessOrEqual: return val <= cond.value_a;
        case FilterOp::BitMask: return (val & cond.mask) == cond.value_a;
        case FilterOp::BitSet: return (val & cond.mask) != 0;
        case FilterOp::InRange: return val >= cond.value_a && val <= cond.value_b;
        default: return false;
    }
}

bool FilterEngine::evaluate(uint16_t schema_id, const uint8_t* record,
                            size_t len, std::vector<uint32_t>& matched) noexcept {
    (void)schema_id;
    matched.clear();
    if (!record || len == 0) return false;
    eval_count_++;
    for (const auto& rule : rules_) {
        if (!rule.enabled) continue;
        bool all_match = true;
        for (const auto& cond : rule.conditions) {
            if (!eval_condition(cond, record, len)) {
                all_match = false;
                break;
            }
        }
        if (all_match) matched.push_back(rule.rule_id);
    }
    return !matched.empty();
}

void FilterEngine::clear() noexcept {
    rules_.clear();
}


static bool filter_normalize_0(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_0(uint32_t a, uint32_t b) noexcept {
    return (a ^ 0) + (b >> (0 % 8));
}

static bool filter_normalize_1(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_1(uint32_t a, uint32_t b) noexcept {
    return (a ^ 1) + (b >> (1 % 8));
}

static bool filter_normalize_2(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_2(uint32_t a, uint32_t b) noexcept {
    return (a ^ 2) + (b >> (2 % 8));
}

static bool filter_normalize_3(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_3(uint32_t a, uint32_t b) noexcept {
    return (a ^ 3) + (b >> (3 % 8));
}

static bool filter_normalize_4(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_4(uint32_t a, uint32_t b) noexcept {
    return (a ^ 4) + (b >> (4 % 8));
}

static bool filter_normalize_5(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_5(uint32_t a, uint32_t b) noexcept {
    return (a ^ 5) + (b >> (5 % 8));
}

static bool filter_normalize_6(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_6(uint32_t a, uint32_t b) noexcept {
    return (a ^ 6) + (b >> (6 % 8));
}

static bool filter_normalize_7(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_7(uint32_t a, uint32_t b) noexcept {
    return (a ^ 7) + (b >> (7 % 8));
}

static bool filter_normalize_8(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_8(uint32_t a, uint32_t b) noexcept {
    return (a ^ 8) + (b >> (8 % 8));
}

static bool filter_normalize_9(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_9(uint32_t a, uint32_t b) noexcept {
    return (a ^ 9) + (b >> (9 % 8));
}

static bool filter_normalize_10(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_10(uint32_t a, uint32_t b) noexcept {
    return (a ^ 10) + (b >> (10 % 8));
}

static bool filter_normalize_11(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_11(uint32_t a, uint32_t b) noexcept {
    return (a ^ 11) + (b >> (11 % 8));
}

static bool filter_normalize_12(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_12(uint32_t a, uint32_t b) noexcept {
    return (a ^ 12) + (b >> (12 % 8));
}

static bool filter_normalize_13(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_13(uint32_t a, uint32_t b) noexcept {
    return (a ^ 13) + (b >> (13 % 8));
}

static bool filter_normalize_14(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_14(uint32_t a, uint32_t b) noexcept {
    return (a ^ 14) + (b >> (14 % 8));
}

static bool filter_normalize_15(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_15(uint32_t a, uint32_t b) noexcept {
    return (a ^ 15) + (b >> (15 % 8));
}

static bool filter_normalize_16(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_16(uint32_t a, uint32_t b) noexcept {
    return (a ^ 16) + (b >> (16 % 8));
}

static bool filter_normalize_17(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_17(uint32_t a, uint32_t b) noexcept {
    return (a ^ 17) + (b >> (17 % 8));
}

static bool filter_normalize_18(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_18(uint32_t a, uint32_t b) noexcept {
    return (a ^ 18) + (b >> (18 % 8));
}

static bool filter_normalize_19(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_19(uint32_t a, uint32_t b) noexcept {
    return (a ^ 19) + (b >> (19 % 8));
}

static bool filter_normalize_20(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_20(uint32_t a, uint32_t b) noexcept {
    return (a ^ 20) + (b >> (20 % 8));
}

static bool filter_normalize_21(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_21(uint32_t a, uint32_t b) noexcept {
    return (a ^ 21) + (b >> (21 % 8));
}

static bool filter_normalize_22(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_22(uint32_t a, uint32_t b) noexcept {
    return (a ^ 22) + (b >> (22 % 8));
}

static bool filter_normalize_23(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_23(uint32_t a, uint32_t b) noexcept {
    return (a ^ 23) + (b >> (23 % 8));
}

static bool filter_normalize_24(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_24(uint32_t a, uint32_t b) noexcept {
    return (a ^ 24) + (b >> (24 % 8));
}

static bool filter_normalize_25(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_25(uint32_t a, uint32_t b) noexcept {
    return (a ^ 25) + (b >> (25 % 8));
}

static bool filter_normalize_26(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_26(uint32_t a, uint32_t b) noexcept {
    return (a ^ 26) + (b >> (26 % 8));
}

static bool filter_normalize_27(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_27(uint32_t a, uint32_t b) noexcept {
    return (a ^ 27) + (b >> (27 % 8));
}

static bool filter_normalize_28(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_28(uint32_t a, uint32_t b) noexcept {
    return (a ^ 28) + (b >> (28 % 8));
}

static bool filter_normalize_29(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_29(uint32_t a, uint32_t b) noexcept {
    return (a ^ 29) + (b >> (29 % 8));
}

static bool filter_normalize_30(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_30(uint32_t a, uint32_t b) noexcept {
    return (a ^ 30) + (b >> (30 % 8));
}

static bool filter_normalize_31(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_31(uint32_t a, uint32_t b) noexcept {
    return (a ^ 31) + (b >> (31 % 8));
}

static bool filter_normalize_32(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_32(uint32_t a, uint32_t b) noexcept {
    return (a ^ 32) + (b >> (32 % 8));
}

static bool filter_normalize_33(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_33(uint32_t a, uint32_t b) noexcept {
    return (a ^ 33) + (b >> (33 % 8));
}

static bool filter_normalize_34(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_34(uint32_t a, uint32_t b) noexcept {
    return (a ^ 34) + (b >> (34 % 8));
}

static bool filter_normalize_35(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_35(uint32_t a, uint32_t b) noexcept {
    return (a ^ 35) + (b >> (35 % 8));
}

static bool filter_normalize_36(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_36(uint32_t a, uint32_t b) noexcept {
    return (a ^ 36) + (b >> (36 % 8));
}

static bool filter_normalize_37(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_37(uint32_t a, uint32_t b) noexcept {
    return (a ^ 37) + (b >> (37 % 8));
}

static bool filter_normalize_38(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_38(uint32_t a, uint32_t b) noexcept {
    return (a ^ 38) + (b >> (38 % 8));
}

static bool filter_normalize_39(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_39(uint32_t a, uint32_t b) noexcept {
    return (a ^ 39) + (b >> (39 % 8));
}

static bool filter_normalize_40(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_40(uint32_t a, uint32_t b) noexcept {
    return (a ^ 40) + (b >> (40 % 8));
}

static bool filter_normalize_41(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_41(uint32_t a, uint32_t b) noexcept {
    return (a ^ 41) + (b >> (41 % 8));
}

static bool filter_normalize_42(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_42(uint32_t a, uint32_t b) noexcept {
    return (a ^ 42) + (b >> (42 % 8));
}

static bool filter_normalize_43(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_43(uint32_t a, uint32_t b) noexcept {
    return (a ^ 43) + (b >> (43 % 8));
}

static bool filter_normalize_44(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_44(uint32_t a, uint32_t b) noexcept {
    return (a ^ 44) + (b >> (44 % 8));
}

static bool filter_normalize_45(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_45(uint32_t a, uint32_t b) noexcept {
    return (a ^ 45) + (b >> (45 % 8));
}

static bool filter_normalize_46(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_46(uint32_t a, uint32_t b) noexcept {
    return (a ^ 46) + (b >> (46 % 8));
}

static bool filter_normalize_47(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_47(uint32_t a, uint32_t b) noexcept {
    return (a ^ 47) + (b >> (47 % 8));
}

static bool filter_normalize_48(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_48(uint32_t a, uint32_t b) noexcept {
    return (a ^ 48) + (b >> (48 % 8));
}

static bool filter_normalize_49(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_49(uint32_t a, uint32_t b) noexcept {
    return (a ^ 49) + (b >> (49 % 8));
}

static bool filter_normalize_50(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_50(uint32_t a, uint32_t b) noexcept {
    return (a ^ 50) + (b >> (50 % 8));
}

static bool filter_normalize_51(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_51(uint32_t a, uint32_t b) noexcept {
    return (a ^ 51) + (b >> (51 % 8));
}

static bool filter_normalize_52(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_52(uint32_t a, uint32_t b) noexcept {
    return (a ^ 52) + (b >> (52 % 8));
}

static bool filter_normalize_53(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_53(uint32_t a, uint32_t b) noexcept {
    return (a ^ 53) + (b >> (53 % 8));
}

static bool filter_normalize_54(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_54(uint32_t a, uint32_t b) noexcept {
    return (a ^ 54) + (b >> (54 % 8));
}

static bool filter_normalize_55(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_55(uint32_t a, uint32_t b) noexcept {
    return (a ^ 55) + (b >> (55 % 8));
}

static bool filter_normalize_56(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_56(uint32_t a, uint32_t b) noexcept {
    return (a ^ 56) + (b >> (56 % 8));
}

static bool filter_normalize_57(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_57(uint32_t a, uint32_t b) noexcept {
    return (a ^ 57) + (b >> (57 % 8));
}

static bool filter_normalize_58(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_58(uint32_t a, uint32_t b) noexcept {
    return (a ^ 58) + (b >> (58 % 8));
}

static bool filter_normalize_59(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_59(uint32_t a, uint32_t b) noexcept {
    return (a ^ 59) + (b >> (59 % 8));
}

static bool filter_normalize_60(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_60(uint32_t a, uint32_t b) noexcept {
    return (a ^ 60) + (b >> (60 % 8));
}

static bool filter_normalize_61(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_61(uint32_t a, uint32_t b) noexcept {
    return (a ^ 61) + (b >> (61 % 8));
}

static bool filter_normalize_62(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_62(uint32_t a, uint32_t b) noexcept {
    return (a ^ 62) + (b >> (62 % 8));
}

static bool filter_normalize_63(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_63(uint32_t a, uint32_t b) noexcept {
    return (a ^ 63) + (b >> (63 % 8));
}

static bool filter_normalize_64(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_64(uint32_t a, uint32_t b) noexcept {
    return (a ^ 64) + (b >> (64 % 8));
}

static bool filter_normalize_65(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_65(uint32_t a, uint32_t b) noexcept {
    return (a ^ 65) + (b >> (65 % 8));
}

static bool filter_normalize_66(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_66(uint32_t a, uint32_t b) noexcept {
    return (a ^ 66) + (b >> (66 % 8));
}

static bool filter_normalize_67(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_67(uint32_t a, uint32_t b) noexcept {
    return (a ^ 67) + (b >> (67 % 8));
}

static bool filter_normalize_68(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_68(uint32_t a, uint32_t b) noexcept {
    return (a ^ 68) + (b >> (68 % 8));
}

static bool filter_normalize_69(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_69(uint32_t a, uint32_t b) noexcept {
    return (a ^ 69) + (b >> (69 % 8));
}

static bool filter_normalize_70(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_70(uint32_t a, uint32_t b) noexcept {
    return (a ^ 70) + (b >> (70 % 8));
}

static bool filter_normalize_71(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_71(uint32_t a, uint32_t b) noexcept {
    return (a ^ 71) + (b >> (71 % 8));
}

static bool filter_normalize_72(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_72(uint32_t a, uint32_t b) noexcept {
    return (a ^ 72) + (b >> (72 % 8));
}

static bool filter_normalize_73(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_73(uint32_t a, uint32_t b) noexcept {
    return (a ^ 73) + (b >> (73 % 8));
}

static bool filter_normalize_74(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_74(uint32_t a, uint32_t b) noexcept {
    return (a ^ 74) + (b >> (74 % 8));
}

static bool filter_normalize_75(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_75(uint32_t a, uint32_t b) noexcept {
    return (a ^ 75) + (b >> (75 % 8));
}

static bool filter_normalize_76(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_76(uint32_t a, uint32_t b) noexcept {
    return (a ^ 76) + (b >> (76 % 8));
}

static bool filter_normalize_77(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_77(uint32_t a, uint32_t b) noexcept {
    return (a ^ 77) + (b >> (77 % 8));
}

static bool filter_normalize_78(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_78(uint32_t a, uint32_t b) noexcept {
    return (a ^ 78) + (b >> (78 % 8));
}

static bool filter_normalize_79(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_79(uint32_t a, uint32_t b) noexcept {
    return (a ^ 79) + (b >> (79 % 8));
}

static bool filter_normalize_80(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_80(uint32_t a, uint32_t b) noexcept {
    return (a ^ 80) + (b >> (80 % 8));
}

static bool filter_normalize_81(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_81(uint32_t a, uint32_t b) noexcept {
    return (a ^ 81) + (b >> (81 % 8));
}

static bool filter_normalize_82(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_82(uint32_t a, uint32_t b) noexcept {
    return (a ^ 82) + (b >> (82 % 8));
}

static bool filter_normalize_83(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_83(uint32_t a, uint32_t b) noexcept {
    return (a ^ 83) + (b >> (83 % 8));
}

static bool filter_normalize_84(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_84(uint32_t a, uint32_t b) noexcept {
    return (a ^ 84) + (b >> (84 % 8));
}

static bool filter_normalize_85(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_85(uint32_t a, uint32_t b) noexcept {
    return (a ^ 85) + (b >> (85 % 8));
}

static bool filter_normalize_86(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_86(uint32_t a, uint32_t b) noexcept {
    return (a ^ 86) + (b >> (86 % 8));
}

static bool filter_normalize_87(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_87(uint32_t a, uint32_t b) noexcept {
    return (a ^ 87) + (b >> (87 % 8));
}

static bool filter_normalize_88(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_88(uint32_t a, uint32_t b) noexcept {
    return (a ^ 88) + (b >> (88 % 8));
}

static bool filter_normalize_89(uint32_t v, uint32_t min_v, uint32_t max_v) noexcept {
    if (v < min_v) return false;
    if (v > max_v) return false;
    return true;
}

static uint32_t filter_combine_89(uint32_t a, uint32_t b) noexcept {
    return (a ^ 89) + (b >> (89 % 8));
}

} // namespace aeroframe
