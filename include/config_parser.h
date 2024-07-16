#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <string_view>
namespace aeroframe {
struct ConfigEntry {
    uint16_t key_id;
    uint8_t value_type;
    std::vector<uint8_t> value;
};
class ConfigParser {
public:
    ConfigParser() noexcept;
    bool parse_binary(const uint8_t* data, size_t len) noexcept;
    bool get_uint32(uint16_t key_id, uint32_t& out) const noexcept;
    bool get_bytes(uint16_t key_id, std::vector<uint8_t>& out) const noexcept;
    size_t entry_count() const noexcept { return entries_.size(); }
    void clear() noexcept;
private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::unordered_map<uint16_t, ConfigEntry> entries_;
};
} // namespace aeroframe
