#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <string_view>
namespace aeroframe {
struct DeviceInfo {
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t firmware_major;
    uint8_t firmware_minor;
    uint8_t status;
    char name[32];
};
class DeviceRegistry {
public:
    DeviceRegistry() noexcept;
    bool register_device(const DeviceInfo& info) noexcept;
    bool load_registry_binary(const uint8_t* data, size_t len) noexcept;
    const DeviceInfo* lookup(uint32_t device_id) const noexcept;
    bool update_status(uint32_t device_id, uint8_t status) noexcept;
    size_t device_count() const noexcept { return devices_.size(); }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::unordered_map<uint32_t, DeviceInfo> devices_;
};
} // namespace aeroframe
