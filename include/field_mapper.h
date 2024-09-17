#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
namespace aeroframe {
struct FieldMapping {
    uint16_t src_field_id;
    uint16_t dst_field_id;
    uint8_t transform_id;
    double scale;
    double offset;
};
class FieldMapper {
public:
    FieldMapper() noexcept;
    bool add_mapping(const FieldMapping& m) noexcept;
    bool load_mappings_binary(const uint8_t* data, size_t len) noexcept;
    bool map_record(const uint8_t* src, size_t src_len,
                    std::vector<uint8_t>& dst) const noexcept;
    void clear() noexcept;
    size_t mapping_count() const noexcept { return mappings_.size(); }
private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static void write_u32(uint8_t* p, uint32_t v) noexcept;
    double apply_transform(uint8_t tid, double v, double scale, double offset) const noexcept;
    std::vector<FieldMapping> mappings_;
};
} // namespace aeroframe
