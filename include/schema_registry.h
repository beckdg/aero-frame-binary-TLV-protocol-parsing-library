#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string_view>
#include <unordered_map>

namespace aeroframe {

enum class FieldType : uint8_t {
    UInt8, UInt16, UInt32, Int8, Int16, Int32,
    Float32, Float64, Bool, Bytes, String, Nested
};

struct FieldDescriptor {
    uint16_t field_id;
    FieldType type;
    uint16_t offset;
    uint16_t size;
    bool required;
    uint8_t flags;
};

struct SchemaDefinition {
    uint16_t schema_id;
    uint16_t version;
    uint16_t record_size;
    std::vector<FieldDescriptor> fields;
};

class SchemaRegistry {
public:
    SchemaRegistry() noexcept;

    bool register_schema(const SchemaDefinition& schema) noexcept;
    bool load_binary_schema(const uint8_t* data, size_t len) noexcept;
    const SchemaDefinition* lookup(uint16_t schema_id) const noexcept;

    bool validate_record(uint16_t schema_id,
                         const uint8_t* data, size_t len) const noexcept;
    bool extract_field(uint16_t schema_id, uint16_t field_id,
                       const uint8_t* data, size_t len,
                       std::vector<uint8_t>& out) const noexcept;

    size_t schema_count() const noexcept { return schemas_.size(); }

private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    bool parse_field_descriptor(const uint8_t* data, size_t len,
                                size_t& offset, FieldDescriptor& out) const noexcept;

    std::unordered_map<uint16_t, SchemaDefinition> schemas_;
    uint32_t load_count_{0};
};

} // namespace aeroframe
