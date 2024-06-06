#include "schema_registry.h"
#include <cstring>

namespace aeroframe {

SchemaRegistry::SchemaRegistry() noexcept : load_count_(0) {}

uint16_t SchemaRegistry::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t SchemaRegistry::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool SchemaRegistry::register_schema(const SchemaDefinition& schema) noexcept {
    if (schema.schema_id == 0) return false;
    if (schema.fields.empty()) return false;
    schemas_[schema.schema_id] = schema;
    return true;
}

bool SchemaRegistry::parse_field_descriptor(const uint8_t* data, size_t len,
                                            size_t& offset,
                                            FieldDescriptor& out) const noexcept {
    if (offset + 8 > len) return false;
    out.field_id = read_u16(data + offset);
    out.type = static_cast<FieldType>(data[offset + 2]);
    out.offset = read_u16(data + offset + 3);
    out.size = read_u16(data + offset + 5);
    out.required = data[offset + 7] != 0;
    out.flags = (offset + 8 < len) ? data[offset + 8] : 0;
    offset += 8;
    return true;
}

bool SchemaRegistry::load_binary_schema(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 8) return false;
    if (data[0] != 0x53 || data[1] != 0x43) return false; // "SC"
    uint16_t schema_id = read_u16(data + 2);
    uint16_t version = read_u16(data + 4);
    uint16_t field_count = read_u16(data + 6);
    size_t offset = 8;
    SchemaDefinition def{};
    def.schema_id = schema_id;
    def.version = version;
    def.record_size = 0;
    for (uint16_t i = 0; i < field_count; ++i) {
        FieldDescriptor fd{};
        if (!parse_field_descriptor(data, len, offset, fd)) return false;
        def.record_size = std::max(def.record_size,
                                   static_cast<uint16_t>(fd.offset + fd.size));
        def.fields.push_back(fd);
    }
    load_count_++;
    return register_schema(def);
}

const SchemaDefinition* SchemaRegistry::lookup(uint16_t schema_id) const noexcept {
    auto it = schemas_.find(schema_id);
    if (it == schemas_.end()) return nullptr;
    return &it->second;
}

bool SchemaRegistry::validate_record(uint16_t schema_id,
                                     const uint8_t* data, size_t len) const noexcept {
    const SchemaDefinition* schema = lookup(schema_id);
    if (!schema) return false;
    if (len < schema->record_size) return false;
    for (const auto& field : schema->fields) {
        if (field.required && field.offset + field.size > len) return false;
        switch (field.type) {
            case FieldType::UInt8:
            case FieldType::Int8:
            case FieldType::Bool:
                if (field.size != 1) return false;
                break;
            case FieldType::UInt16:
            case FieldType::Int16:
                if (field.size != 2) return false;
                break;
            case FieldType::UInt32:
            case FieldType::Int32:
            case FieldType::Float32:
                if (field.size != 4) return false;
                break;
            case FieldType::Float64:
                if (field.size != 8) return false;
                break;
            default:
                break;
        }
    }
    return true;
}

bool SchemaRegistry::extract_field(uint16_t schema_id, uint16_t field_id,
                                   const uint8_t* data, size_t len,
                                   std::vector<uint8_t>& out) const noexcept {
    const SchemaDefinition* schema = lookup(schema_id);
    if (!schema || !data) return false;
    for (const auto& field : schema->fields) {
        if (field.field_id != field_id) continue;
        if (field.offset + field.size > len) return false;
        out.assign(data + field.offset, data + field.offset + field.size);
        return true;
    }
    return false;
}


static bool schema_compat_check_0(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_0(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 0;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_1(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_1(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 1;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_2(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_2(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 2;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_3(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_3(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 3;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_4(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_4(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 4;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_5(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_5(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 5;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_6(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_6(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 6;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_7(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_7(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 7;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_8(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_8(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 8;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_9(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_9(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 9;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_10(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_10(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 10;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_11(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_11(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 11;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_12(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_12(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 12;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_13(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_13(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 13;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_14(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_14(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 14;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_15(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_15(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 15;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_16(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_16(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 16;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_17(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_17(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 17;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_18(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_18(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 18;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_19(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_19(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 19;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_20(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_20(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 20;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_21(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_21(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 21;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_22(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_22(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 22;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_23(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_23(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 23;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_24(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_24(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 24;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_25(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_25(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 25;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_26(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_26(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 26;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_27(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_27(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 27;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_28(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_28(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 28;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_29(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_29(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 29;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_30(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_30(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 30;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_31(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_31(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 31;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_32(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_32(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 32;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_33(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_33(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 33;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_34(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_34(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 34;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_35(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_35(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 35;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_36(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_36(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 36;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_37(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_37(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 37;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_38(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_38(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 38;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_39(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_39(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 39;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_40(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_40(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 40;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_41(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_41(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 41;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_42(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_42(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 42;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_43(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_43(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 43;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_44(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_44(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 44;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_45(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_45(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 45;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_46(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_46(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 46;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_47(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_47(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 47;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_48(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_48(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 48;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_49(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_49(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 49;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_50(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_50(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 50;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_51(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_51(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 51;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_52(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_52(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 52;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_53(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_53(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 53;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_54(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_54(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 54;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_55(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_55(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 55;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_56(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_56(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 56;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_57(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_57(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 57;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_58(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_58(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 58;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_59(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_59(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 59;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_60(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_60(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 60;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_61(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_61(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 61;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_62(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_62(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 62;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_63(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_63(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 63;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_64(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_64(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 64;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_65(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_65(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 65;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_66(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_66(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 66;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_67(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_67(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 67;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_68(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_68(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 68;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_69(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_69(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 69;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_70(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_70(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 70;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_71(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_71(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 71;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_72(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_72(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 72;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_73(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_73(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 73;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_74(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_74(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 74;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_75(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_75(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 75;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_76(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_76(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 76;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_77(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_77(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 77;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_78(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_78(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 78;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_79(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_79(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 79;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_80(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_80(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 80;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_81(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_81(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 81;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_82(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_82(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 82;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_83(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_83(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 83;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_84(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_84(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 84;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_85(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_85(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 85;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_86(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_86(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 86;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_87(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_87(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 87;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_88(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_88(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 88;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_89(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_89(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 89;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_90(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_90(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 90;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_91(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_91(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 91;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_92(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_92(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 92;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_93(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_93(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 93;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_94(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_94(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 94;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_95(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_95(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 95;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_96(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_96(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 96;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_97(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_97(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 97;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_98(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_98(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 98;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

static bool schema_compat_check_99(uint16_t a, uint16_t b) noexcept {
    return a <= b || (a - b) < 256;
}

static uint32_t schema_field_checksum_99(const uint8_t* data, size_t len) noexcept {
    uint32_t sum = 99;
    for (size_t j = 0; j < len; ++j) sum = (sum * 31) + data[j];
    return sum;
}

} // namespace aeroframe
