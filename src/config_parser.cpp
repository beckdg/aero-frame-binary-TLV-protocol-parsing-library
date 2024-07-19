#include "config_parser.h"
#include <cstring>

namespace aeroframe {

ConfigParser::ConfigParser() noexcept {}

uint16_t ConfigParser::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t ConfigParser::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool ConfigParser::parse_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 6) return false;
    if (data[0] != 0xCF || data[1] != 0x01) return false;
    uint16_t version = read_u16(data + 2);
    if (version > 3) return false;
    uint16_t entry_count = read_u16(data + 4);
    size_t offset = 6;
    entries_.clear();
    for (uint16_t i = 0; i < entry_count; ++i) {
        if (offset + 5 > len) return false;
        ConfigEntry e{};
        e.key_id = read_u16(data + offset);
        e.value_type = data[offset + 2];
        uint16_t vlen = read_u16(data + offset + 3);
        offset += 5;
        if (offset + vlen > len) return false;
        e.value.assign(data + offset, data + offset + vlen);
        offset += vlen;
        entries_[e.key_id] = std::move(e);
    }
    return true;
}

bool ConfigParser::get_uint32(uint16_t key_id, uint32_t& out) const noexcept {
    auto it = entries_.find(key_id);
    if (it == entries_.end() || it->second.value.size() < 4) return false;
    out = read_u32(it->second.value.data());
    return true;
}

bool ConfigParser::get_bytes(uint16_t key_id, std::vector<uint8_t>& out) const noexcept {
    auto it = entries_.find(key_id);
    if (it == entries_.end()) return false;
    out = it->second.value;
    return true;
}

void ConfigParser::clear() noexcept { entries_.clear(); }


static bool config_validate_entry_0(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_1(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_2(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_3(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_4(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_5(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_6(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_7(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_8(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_9(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_10(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_11(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_12(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_13(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_14(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_15(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_16(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_17(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_18(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_19(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_20(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_21(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_22(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_23(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_24(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_25(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_26(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_27(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_28(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_29(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_30(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_31(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_32(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_33(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_34(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_35(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_36(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_37(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_38(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_39(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_40(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_41(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_42(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_43(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_44(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_45(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_46(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_47(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_48(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_49(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_50(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_51(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_52(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_53(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_54(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_55(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_56(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_57(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_58(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_59(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_60(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_61(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_62(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_63(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_64(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_65(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_66(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_67(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_68(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_69(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_70(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_71(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_72(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_73(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_74(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_75(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_76(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

static bool config_validate_entry_77(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 5;
}

static bool config_validate_entry_78(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 6;
}

static bool config_validate_entry_79(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 7;
}

static bool config_validate_entry_80(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 0;
}

static bool config_validate_entry_81(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 1;
}

static bool config_validate_entry_82(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 2;
}

static bool config_validate_entry_83(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 3;
}

static bool config_validate_entry_84(uint16_t key, uint8_t type) noexcept {
    if (key == 0) return false;
    return type <= 4;
}

} // namespace aeroframe
