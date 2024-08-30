#include "binary_validator.h"
#include "checksum_chain.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

namespace {
uint16_t crc16_ccitt(const uint8_t* data, size_t len) noexcept {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = static_cast<uint16_t>((crc << 8) ^ static_cast<uint16_t>(((crc >> 8) ^ data[i]) & 0xFF));
    }
    return crc;
}
}

BinaryValidator::BinaryValidator() noexcept : failure_count_(0) {}

uint32_t BinaryValidator::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

uint16_t BinaryValidator::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

bool BinaryValidator::add_rule(const ValidationSpec& spec) noexcept {
    rules_.push_back(spec);
    return true;
}

bool BinaryValidator::load_rules_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    if (data[0] != 0x56 || data[1] != 0x52) return false;
    uint16_t count = read_u16(data + 2);
    size_t offset = 4;
    rules_.clear();
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 11 > len) return false;
        ValidationSpec spec{};
        spec.rule = static_cast<ValidationRule>(data[offset]);
        spec.param_a = read_u32(data + offset + 1);
        spec.param_b = read_u32(data + offset + 5);
        uint16_t elen = read_u16(data + offset + 9);
        offset += 11;
        if (offset + elen > len) return false;
        spec.expected.assign(data + offset, data + offset + elen);
        offset += elen;
        rules_.push_back(std::move(spec));
    }
    return true;
}

bool BinaryValidator::check_rule(const ValidationSpec& spec,
                                  const uint8_t* data, size_t len) const noexcept {
    switch (spec.rule) {
        case ValidationRule::MinLength:
            return len >= spec.param_a;
        case ValidationRule::MaxLength:
            return len <= spec.param_a;
        case ValidationRule::MagicBytes:
            if (spec.expected.size() > len) return false;
            return std::memcmp(data, spec.expected.data(), spec.expected.size()) == 0;
        case ValidationRule::CrcMatch: {
            if (len < 2) return false;
            uint16_t stored = read_u16(data + len - 2);
            return stored == crc16_ccitt(data, len - 2);
        }
        case ValidationRule::FieldRange: {
            size_t off = spec.param_a;
            if (off + 4 > len) return false;
            uint32_t val = read_u32(data + off);
            uint32_t hi = spec.param_b;
            uint32_t lo = spec.expected.empty() ? 0 : read_u32(spec.expected.data());
            return val >= lo && val <= hi;
        }
        case ValidationRule::RequiredField:
            return len >= spec.param_a + spec.param_b;
        case ValidationRule::SchemaVersion:
            return len >= 4 && read_u16(data + 2) == spec.param_a;
        case ValidationRule::ChecksumChain: {
            ChecksumChain chain;
            return chain.verify_chain(data, len);
        }
        default:
            return true;
    }
}

bool BinaryValidator::validate(const uint8_t* data, size_t len) const noexcept {
    if (!data && len > 0) return false;
    for (const auto& rule : rules_) {
        if (!check_rule(rule, data, len)) {
            failure_count_++;
            return false;
        }
    }
    return true;
}

void BinaryValidator::clear() noexcept {
    rules_.clear();
    failure_count_ = 0;
}


static bool validator_preflight_0(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_0(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 0;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_1(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_1(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 1;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_2(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_2(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 2;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_3(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_3(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 3;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_4(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_4(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 4;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_5(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_5(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 5;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_6(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_6(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 6;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_7(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_7(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 7;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_8(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_8(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 8;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_9(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_9(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 9;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_10(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_10(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 10;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_11(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_11(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 11;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_12(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_12(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 12;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_13(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_13(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 13;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_14(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_14(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 14;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_15(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_15(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 15;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_16(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_16(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 16;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_17(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_17(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 17;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_18(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_18(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 18;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_19(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_19(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 19;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_20(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_20(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 20;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_21(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_21(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 21;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_22(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_22(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 22;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_23(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_23(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 23;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_24(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_24(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 24;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_25(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_25(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 25;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_26(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_26(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 26;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_27(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_27(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 27;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_28(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_28(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 28;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_29(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_29(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 29;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_30(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_30(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 30;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_31(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_31(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 31;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_32(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_32(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 32;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_33(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_33(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 33;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_34(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_34(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 34;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_35(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_35(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 35;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_36(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_36(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 36;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_37(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_37(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 37;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_38(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_38(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 38;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_39(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_39(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 39;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_40(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_40(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 40;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_41(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_41(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 41;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_42(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_42(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 42;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_43(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_43(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 43;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_44(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_44(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 44;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_45(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_45(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 45;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_46(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_46(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 46;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_47(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_47(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 47;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_48(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_48(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 48;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_49(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_49(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 49;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_50(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_50(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 50;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_51(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_51(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 51;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_52(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_52(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 52;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_53(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_53(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 53;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_54(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_54(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 54;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_55(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_55(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 55;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_56(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_56(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 56;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_57(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_57(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 57;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_58(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_58(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 58;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_59(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_59(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 59;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_60(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_60(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 60;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_61(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_61(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 61;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_62(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_62(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 62;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_63(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_63(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 63;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_64(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_64(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 64;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_65(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_65(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 65;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_66(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_66(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 66;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_67(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_67(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 67;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_68(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_68(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 68;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_69(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_69(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 69;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_70(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_70(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 70;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_71(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_71(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 71;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_72(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_72(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 72;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_73(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_73(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 73;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_74(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_74(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 74;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_75(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_75(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 75;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_76(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_76(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 76;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_77(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_77(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 77;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_78(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_78(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 78;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_79(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_79(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 79;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_80(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_80(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 80;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_81(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_81(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 81;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_82(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_82(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 82;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_83(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_83(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 83;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_84(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_84(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 84;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_85(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_85(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 85;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_86(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_86(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 86;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_87(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_87(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 87;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_88(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_88(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 88;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_89(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_89(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 89;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_90(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_90(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 90;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_91(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_91(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 91;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_92(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_92(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 92;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_93(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_93(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 93;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_94(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_94(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 94;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_95(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_95(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 95;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_96(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_96(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 96;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_97(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_97(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 97;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_98(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_98(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 98;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_99(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_99(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 99;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_100(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_100(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 100;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_101(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_101(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 101;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_102(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_102(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 102;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_103(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_103(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 103;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_104(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_104(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 104;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_105(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_105(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 105;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_106(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_106(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 106;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_107(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_107(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 107;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_108(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_108(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 108;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_109(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_109(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 109;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_110(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_110(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 110;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_111(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_111(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 111;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_112(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_112(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 112;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_113(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_113(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 113;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_114(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_114(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 114;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_115(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_115(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 115;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_116(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_116(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 116;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_117(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_117(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 117;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_118(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_118(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 118;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_119(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_119(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 119;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_120(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_120(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 120;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_121(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_121(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 121;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_122(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_122(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 122;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_123(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_123(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 123;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_124(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_124(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 124;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_125(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_125(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 125;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_126(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_126(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 126;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_127(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_127(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 127;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_128(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_128(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 128;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_129(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_129(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 129;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_130(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_130(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 130;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_131(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_131(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 131;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_132(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_132(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 132;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_133(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_133(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 133;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_134(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_134(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 134;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_135(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_135(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 135;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_136(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_136(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 136;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_137(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_137(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 137;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_138(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_138(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 138;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_139(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_139(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 139;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_140(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_140(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 140;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_141(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_141(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 141;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_142(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_142(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 142;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_143(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_143(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 143;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_144(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_144(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 144;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_145(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_145(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 145;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_146(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_146(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 146;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_147(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_147(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 147;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_148(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_148(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 148;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_149(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_149(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 149;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_150(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_150(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 150;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_151(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_151(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 151;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_152(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_152(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 152;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_153(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_153(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 153;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_154(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_154(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 154;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_155(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_155(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 155;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_156(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_156(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 156;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_157(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_157(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 157;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_158(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_158(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 158;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_159(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_159(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 159;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_160(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_160(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 160;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_161(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_161(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 161;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_162(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_162(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 162;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_163(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_163(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 163;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_164(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 4;
}

static uint32_t validator_digest_164(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 164;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_165(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 5;
}

static uint32_t validator_digest_165(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 165;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_166(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 6;
}

static uint32_t validator_digest_166(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 166;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_167(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 7;
}

static uint32_t validator_digest_167(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 167;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_168(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 8;
}

static uint32_t validator_digest_168(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 168;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_169(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 9;
}

static uint32_t validator_digest_169(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 169;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_170(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 10;
}

static uint32_t validator_digest_170(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 170;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_171(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 11;
}

static uint32_t validator_digest_171(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 171;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_172(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 12;
}

static uint32_t validator_digest_172(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 172;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_173(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 13;
}

static uint32_t validator_digest_173(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 173;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_174(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 14;
}

static uint32_t validator_digest_174(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 174;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_175(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 15;
}

static uint32_t validator_digest_175(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 175;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_176(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 0;
}

static uint32_t validator_digest_176(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 176;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_177(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 1;
}

static uint32_t validator_digest_177(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 177;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_178(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 2;
}

static uint32_t validator_digest_178(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 178;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

static bool validator_preflight_179(size_t len, uint32_t min_len) noexcept {
    return len >= min_len + 3;
}

static uint32_t validator_digest_179(const uint8_t* data, size_t len) noexcept {
    uint32_t d = 179;
    for (size_t j = 0; j < len && j < 64; ++j) d = d * 33 + data[j];
    return d;
}

} // namespace aeroframe
