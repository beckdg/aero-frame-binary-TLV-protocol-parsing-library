#include "session_store.h"
#include <cstring>

namespace aeroframe {

SessionStore::SessionStore() noexcept {}

uint32_t SessionStore::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool SessionStore::open(uint32_t session_id, uint32_t timestamp_ms) noexcept {
    SessionRecord rec{};
    rec.session_id = session_id;
    rec.opened_at_ms = timestamp_ms;
    rec.last_active_ms = timestamp_ms;
    rec.state = 1;
    rec.byte_count = 0;
    sessions_[session_id] = std::move(rec);
    return true;
}

bool SessionStore::close(uint32_t session_id) noexcept {
    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) return false;
    it->second.state = 0;
    return true;
}

bool SessionStore::append(uint32_t session_id, const uint8_t* data, size_t len) noexcept {
    auto it = sessions_.find(session_id);
    if (it == sessions_.end() || it->second.state == 0) return false;
    it->second.byte_count += static_cast<uint32_t>(len);
    it->second.metadata.insert(it->second.metadata.end(), data, data + len);
    return true;
}

bool SessionStore::load_store_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 18 > len) return false;
        SessionRecord rec{};
        rec.session_id = read_u32(data + offset);
        rec.opened_at_ms = read_u32(data + offset + 4);
        rec.last_active_ms = read_u32(data + offset + 8);
        rec.state = data[offset + 12];
        rec.byte_count = read_u32(data + offset + 13);
        uint16_t mlen = (static_cast<uint16_t>(data[offset+17]) << 8) | data[offset+18];
        offset += 19;
        if (offset + mlen > len) return false;
        rec.metadata.assign(data + offset, data + offset + mlen);
        offset += mlen;
        sessions_[rec.session_id] = std::move(rec);
    }
    return true;
}

const SessionRecord* SessionStore::lookup(uint32_t session_id) const noexcept {
    auto it = sessions_.find(session_id);
    if (it == sessions_.end()) return nullptr;
    return &it->second;
}

size_t SessionStore::active_count() const noexcept {
    size_t n = 0;
    for (const auto& kv : sessions_) {
        if (kv.second.state != 0) ++n;
    }
    return n;
}


static bool session_store_validate_0(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_1(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_2(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_3(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_4(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_5(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_6(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_7(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_8(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_9(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_10(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_11(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_12(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_13(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_14(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_15(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_16(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_17(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_18(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_19(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_20(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_21(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_22(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_23(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_24(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_25(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_26(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_27(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_28(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_29(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_30(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_31(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_32(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_33(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_34(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_35(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_36(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_37(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_38(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_39(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_40(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_41(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_42(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_43(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_44(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_45(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_46(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_47(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_48(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_49(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_50(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_51(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_52(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_53(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_54(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_55(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_56(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_57(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_58(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_59(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_60(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_61(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_62(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_63(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_64(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_65(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_66(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_67(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_68(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_69(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_70(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_71(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_72(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_73(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_74(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_75(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_76(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_77(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_78(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_79(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_80(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_81(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_82(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_83(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_84(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_85(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_86(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_87(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_88(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_89(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_90(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_91(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_92(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_93(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_94(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_95(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_96(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_97(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_98(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_99(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_100(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_101(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_102(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_103(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_104(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_105(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_106(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_107(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_108(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_109(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_110(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_111(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_112(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_113(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_114(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_115(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_116(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_117(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_118(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_119(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_120(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_121(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_122(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_123(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_124(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_125(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_126(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_127(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_128(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_129(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_130(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_131(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_132(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_133(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_134(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_135(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_136(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_137(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_138(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_139(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_140(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_141(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_142(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_143(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_144(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_145(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

static bool session_store_validate_146(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 2;
}

static bool session_store_validate_147(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 3;
}

static bool session_store_validate_148(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 0;
}

static bool session_store_validate_149(uint32_t id, uint8_t state) noexcept {
    return id != 0 && state <= 1;
}

} // namespace aeroframe
