#include "device_registry.h"
#include <cstring>

namespace aeroframe {

DeviceRegistry::DeviceRegistry() noexcept {}

uint32_t DeviceRegistry::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool DeviceRegistry::register_device(const DeviceInfo& info) noexcept {
    if (info.device_id == 0) return false;
    devices_[info.device_id] = info;
    return true;
}

bool DeviceRegistry::load_registry_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    if (data[0] != 0x44 || data[1] != 0x52) return false;
    uint16_t count = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    size_t offset = 4;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 44 > len) return false;
        DeviceInfo info{};
        info.device_id = read_u32(data + offset);
        info.vendor_id = (static_cast<uint16_t>(data[offset+4]) << 8) | data[offset+5];
        info.product_id = (static_cast<uint16_t>(data[offset+6]) << 8) | data[offset+7];
        info.firmware_major = data[offset+8];
        info.firmware_minor = data[offset+9];
        info.status = data[offset+10];
        std::memcpy(info.name, data + offset + 11, 32);
        info.name[31] = '\0';
        register_device(info);
        offset += 44;
    }
    return true;
}

const DeviceInfo* DeviceRegistry::lookup(uint32_t device_id) const noexcept {
    auto it = devices_.find(device_id);
    if (it == devices_.end()) return nullptr;
    return &it->second;
}

bool DeviceRegistry::update_status(uint32_t device_id, uint8_t status) noexcept {
    auto it = devices_.find(device_id);
    if (it == devices_.end()) return false;
    it->second.status = status;
    return true;
}


static bool device_validate_id_0(uint32_t id) noexcept {
    return id != 0 && (id >> (0 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_1(uint32_t id) noexcept {
    return id != 0 && (id >> (1 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_2(uint32_t id) noexcept {
    return id != 0 && (id >> (2 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_3(uint32_t id) noexcept {
    return id != 0 && (id >> (3 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_4(uint32_t id) noexcept {
    return id != 0 && (id >> (4 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_5(uint32_t id) noexcept {
    return id != 0 && (id >> (5 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_6(uint32_t id) noexcept {
    return id != 0 && (id >> (6 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_7(uint32_t id) noexcept {
    return id != 0 && (id >> (7 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_8(uint32_t id) noexcept {
    return id != 0 && (id >> (8 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_9(uint32_t id) noexcept {
    return id != 0 && (id >> (9 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_10(uint32_t id) noexcept {
    return id != 0 && (id >> (10 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_11(uint32_t id) noexcept {
    return id != 0 && (id >> (11 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_12(uint32_t id) noexcept {
    return id != 0 && (id >> (12 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_13(uint32_t id) noexcept {
    return id != 0 && (id >> (13 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_14(uint32_t id) noexcept {
    return id != 0 && (id >> (14 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_15(uint32_t id) noexcept {
    return id != 0 && (id >> (15 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_16(uint32_t id) noexcept {
    return id != 0 && (id >> (16 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_17(uint32_t id) noexcept {
    return id != 0 && (id >> (17 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_18(uint32_t id) noexcept {
    return id != 0 && (id >> (18 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_19(uint32_t id) noexcept {
    return id != 0 && (id >> (19 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_20(uint32_t id) noexcept {
    return id != 0 && (id >> (20 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_21(uint32_t id) noexcept {
    return id != 0 && (id >> (21 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_22(uint32_t id) noexcept {
    return id != 0 && (id >> (22 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_23(uint32_t id) noexcept {
    return id != 0 && (id >> (23 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_24(uint32_t id) noexcept {
    return id != 0 && (id >> (24 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_25(uint32_t id) noexcept {
    return id != 0 && (id >> (25 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_26(uint32_t id) noexcept {
    return id != 0 && (id >> (26 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_27(uint32_t id) noexcept {
    return id != 0 && (id >> (27 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_28(uint32_t id) noexcept {
    return id != 0 && (id >> (28 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_29(uint32_t id) noexcept {
    return id != 0 && (id >> (29 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_30(uint32_t id) noexcept {
    return id != 0 && (id >> (30 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_31(uint32_t id) noexcept {
    return id != 0 && (id >> (31 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_32(uint32_t id) noexcept {
    return id != 0 && (id >> (32 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_33(uint32_t id) noexcept {
    return id != 0 && (id >> (33 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_34(uint32_t id) noexcept {
    return id != 0 && (id >> (34 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_35(uint32_t id) noexcept {
    return id != 0 && (id >> (35 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_36(uint32_t id) noexcept {
    return id != 0 && (id >> (36 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_37(uint32_t id) noexcept {
    return id != 0 && (id >> (37 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_38(uint32_t id) noexcept {
    return id != 0 && (id >> (38 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_39(uint32_t id) noexcept {
    return id != 0 && (id >> (39 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_40(uint32_t id) noexcept {
    return id != 0 && (id >> (40 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_41(uint32_t id) noexcept {
    return id != 0 && (id >> (41 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_42(uint32_t id) noexcept {
    return id != 0 && (id >> (42 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_43(uint32_t id) noexcept {
    return id != 0 && (id >> (43 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_44(uint32_t id) noexcept {
    return id != 0 && (id >> (44 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_45(uint32_t id) noexcept {
    return id != 0 && (id >> (45 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_46(uint32_t id) noexcept {
    return id != 0 && (id >> (46 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_47(uint32_t id) noexcept {
    return id != 0 && (id >> (47 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_48(uint32_t id) noexcept {
    return id != 0 && (id >> (48 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_49(uint32_t id) noexcept {
    return id != 0 && (id >> (49 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_50(uint32_t id) noexcept {
    return id != 0 && (id >> (50 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_51(uint32_t id) noexcept {
    return id != 0 && (id >> (51 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_52(uint32_t id) noexcept {
    return id != 0 && (id >> (52 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_53(uint32_t id) noexcept {
    return id != 0 && (id >> (53 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_54(uint32_t id) noexcept {
    return id != 0 && (id >> (54 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_55(uint32_t id) noexcept {
    return id != 0 && (id >> (55 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_56(uint32_t id) noexcept {
    return id != 0 && (id >> (56 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_57(uint32_t id) noexcept {
    return id != 0 && (id >> (57 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_58(uint32_t id) noexcept {
    return id != 0 && (id >> (58 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_59(uint32_t id) noexcept {
    return id != 0 && (id >> (59 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_60(uint32_t id) noexcept {
    return id != 0 && (id >> (60 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_61(uint32_t id) noexcept {
    return id != 0 && (id >> (61 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_62(uint32_t id) noexcept {
    return id != 0 && (id >> (62 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_63(uint32_t id) noexcept {
    return id != 0 && (id >> (63 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_64(uint32_t id) noexcept {
    return id != 0 && (id >> (64 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_65(uint32_t id) noexcept {
    return id != 0 && (id >> (65 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_66(uint32_t id) noexcept {
    return id != 0 && (id >> (66 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_67(uint32_t id) noexcept {
    return id != 0 && (id >> (67 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_68(uint32_t id) noexcept {
    return id != 0 && (id >> (68 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_69(uint32_t id) noexcept {
    return id != 0 && (id >> (69 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_70(uint32_t id) noexcept {
    return id != 0 && (id >> (70 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_71(uint32_t id) noexcept {
    return id != 0 && (id >> (71 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_72(uint32_t id) noexcept {
    return id != 0 && (id >> (72 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_73(uint32_t id) noexcept {
    return id != 0 && (id >> (73 % 16)) != 0xFFFFFFFFu;
}

static bool device_validate_id_74(uint32_t id) noexcept {
    return id != 0 && (id >> (74 % 16)) != 0xFFFFFFFFu;
}

} // namespace aeroframe
