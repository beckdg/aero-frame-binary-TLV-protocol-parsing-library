#include "state_store.h"
#include <algorithm>

namespace aeroframe {

StateStore::StateStore() noexcept : latest_id_(0) {}

uint32_t StateStore::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool StateStore::save_snapshot(const StateSnapshot& snap) noexcept {
    if (snap.snapshot_id == 0) return false;
    snapshots_[snap.snapshot_id] = snap;
    if (snap.snapshot_id > latest_id_) latest_id_ = snap.snapshot_id;
    return true;
}

bool StateStore::load_snapshot_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 14) return false;
    if (data[0] != 0x53 || data[1] != 0x54) return false;
    uint16_t count = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    size_t offset = 4;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 12 > len) return false;
        StateSnapshot snap{};
        snap.snapshot_id = read_u32(data + offset);
        snap.timestamp_ms = read_u32(data + offset + 4);
        snap.version = read_u32(data + offset + 8);
        uint16_t blen = (static_cast<uint16_t>(data[offset+12]) << 8) | data[offset+13];
        offset += 14;
        if (offset + blen > len) return false;
        snap.blob.assign(data + offset, data + offset + blen);
        offset += blen;
        save_snapshot(snap);
    }
    return true;
}

bool StateStore::get_snapshot(uint32_t id, StateSnapshot& out) const noexcept {
    auto it = snapshots_.find(id);
    if (it == snapshots_.end()) return false;
    out = it->second;
    return true;
}

bool StateStore::get_latest(StateSnapshot& out) const noexcept {
    return get_snapshot(latest_id_, out);
}

void StateStore::prune_before(uint32_t timestamp_ms) noexcept {
    for (auto it = snapshots_.begin(); it != snapshots_.end(); ) {
        if (it->second.timestamp_ms < timestamp_ms) it = snapshots_.erase(it);
        else ++it;
    }
}


static uint32_t state_store_hash_0(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 0;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_1(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 1;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_2(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 2;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_3(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 3;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_4(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 4;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_5(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 5;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_6(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 6;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_7(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 7;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_8(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 8;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_9(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 9;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_10(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 10;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_11(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 11;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_12(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 12;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_13(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 13;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_14(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 14;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_15(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 15;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_16(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 16;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_17(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 17;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_18(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 18;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_19(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 19;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_20(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 20;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_21(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 21;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_22(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 22;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_23(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 23;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_24(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 24;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_25(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 25;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_26(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 26;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_27(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 27;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_28(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 28;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_29(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 29;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_30(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 30;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_31(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 31;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_32(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 32;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_33(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 33;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_34(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 34;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_35(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 35;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_36(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 36;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_37(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 37;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_38(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 38;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_39(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 39;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_40(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 40;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_41(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 41;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_42(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 42;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_43(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 43;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_44(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 44;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_45(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 45;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_46(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 46;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_47(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 47;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_48(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 48;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_49(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 49;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_50(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 50;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_51(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 51;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_52(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 52;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_53(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 53;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_54(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 54;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_55(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 55;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_56(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 56;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_57(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 57;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_58(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 58;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_59(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 59;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_60(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 60;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_61(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 61;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_62(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 62;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_63(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 63;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_64(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 64;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_65(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 65;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_66(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 66;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_67(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 67;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_68(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 68;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_69(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 69;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_70(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 70;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_71(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 71;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_72(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 72;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_73(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 73;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_74(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 74;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_75(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 75;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_76(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 76;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_77(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 77;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_78(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 78;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_79(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 79;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_80(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 80;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_81(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 81;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_82(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 82;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_83(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 83;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_84(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 84;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_85(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 85;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_86(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 86;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_87(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 87;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_88(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 88;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_89(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 89;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_90(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 90;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_91(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 91;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_92(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 92;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_93(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 93;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_94(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 94;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_95(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 95;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_96(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 96;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_97(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 97;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_98(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 98;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_99(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 99;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_100(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 100;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_101(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 101;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_102(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 102;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_103(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 103;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_104(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 104;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_105(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 105;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_106(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 106;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_107(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 107;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_108(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 108;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_109(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 109;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_110(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 110;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_111(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 111;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_112(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 112;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_113(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 113;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_114(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 114;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_115(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 115;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_116(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 116;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_117(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 117;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_118(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 118;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_119(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 119;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_120(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 120;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_121(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 121;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_122(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 122;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_123(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 123;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_124(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 124;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_125(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 125;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_126(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 126;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_127(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 127;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_128(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 128;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_129(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 129;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_130(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 130;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_131(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 131;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_132(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 132;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_133(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 133;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_134(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 134;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_135(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 135;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_136(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 136;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_137(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 137;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_138(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 138;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_139(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 139;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_140(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 140;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_141(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 141;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_142(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 142;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_143(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 143;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_144(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 144;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_145(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 145;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_146(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 146;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_147(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 147;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_148(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 148;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static uint32_t state_store_hash_149(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 149;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

} // namespace aeroframe
