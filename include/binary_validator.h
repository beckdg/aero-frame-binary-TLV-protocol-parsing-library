#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace aeroframe {

enum class ValidationRule : uint8_t {
    MinLength, MaxLength, MagicBytes, CrcMatch,
    FieldRange, RequiredField, SchemaVersion, ChecksumChain
};

struct ValidationSpec {
    ValidationRule rule;
    uint32_t param_a;
    uint32_t param_b;
    std::vector<uint8_t> expected;
};

class BinaryValidator {
public:
    BinaryValidator() noexcept;

    bool add_rule(const ValidationSpec& spec) noexcept;
    bool load_rules_binary(const uint8_t* data, size_t len) noexcept;
    bool validate(const uint8_t* data, size_t len) const noexcept;
    size_t failure_count() const noexcept { return failure_count_; }
    void clear() noexcept;

private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static uint16_t read_u16(const uint8_t* p) noexcept;
    bool check_rule(const ValidationSpec& spec,
                    const uint8_t* data, size_t len) const noexcept;

    std::vector<ValidationSpec> rules_;
    mutable uint32_t failure_count_{0};
};

} // namespace aeroframe
