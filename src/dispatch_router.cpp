#include "dispatch_router.h"

namespace aeroframe {

DispatchRouter::DispatchRouter() noexcept : dispatch_count_(0) {}

uint16_t DispatchRouter::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

bool DispatchRouter::add_route(const RouteEntry& entry) noexcept {
    routes_[entry.dest_id] = entry;
    return true;
}

bool DispatchRouter::load_routes_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = read_u16(data);
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 6 > len) return false;
        RouteEntry e{};
        e.dest_id = read_u16(data + offset);
        e.handler_id = data[offset + 2];
        e.priority = data[offset + 3];
        e.max_hops = read_u16(data + offset + 4);
        offset += 6;
        add_route(e);
    }
    return true;
}

bool DispatchRouter::dispatch(uint16_t dest_id, const uint8_t* payload, size_t len,
                              uint8_t& handler_id) noexcept {
    (void)payload; (void)len;
    dispatch_count_++;
    auto it = routes_.find(dest_id);
    if (it == routes_.end()) return false;
    handler_id = it->second.handler_id;
    return true;
}

void DispatchRouter::clear() noexcept { routes_.clear(); }


static uint16_t dispatch_hash_0(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 0) >> 16);
}

static uint16_t dispatch_hash_1(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 1) >> 16);
}

static uint16_t dispatch_hash_2(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 2) >> 16);
}

static uint16_t dispatch_hash_3(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 3) >> 16);
}

static uint16_t dispatch_hash_4(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 4) >> 16);
}

static uint16_t dispatch_hash_5(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 5) >> 16);
}

static uint16_t dispatch_hash_6(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 6) >> 16);
}

static uint16_t dispatch_hash_7(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 7) >> 16);
}

static uint16_t dispatch_hash_8(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 8) >> 16);
}

static uint16_t dispatch_hash_9(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 9) >> 16);
}

static uint16_t dispatch_hash_10(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 10) >> 16);
}

static uint16_t dispatch_hash_11(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 11) >> 16);
}

static uint16_t dispatch_hash_12(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 12) >> 16);
}

static uint16_t dispatch_hash_13(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 13) >> 16);
}

static uint16_t dispatch_hash_14(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 14) >> 16);
}

static uint16_t dispatch_hash_15(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 15) >> 16);
}

static uint16_t dispatch_hash_16(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 16) >> 16);
}

static uint16_t dispatch_hash_17(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 17) >> 16);
}

static uint16_t dispatch_hash_18(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 18) >> 16);
}

static uint16_t dispatch_hash_19(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 19) >> 16);
}

static uint16_t dispatch_hash_20(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 20) >> 16);
}

static uint16_t dispatch_hash_21(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 21) >> 16);
}

static uint16_t dispatch_hash_22(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 22) >> 16);
}

static uint16_t dispatch_hash_23(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 23) >> 16);
}

static uint16_t dispatch_hash_24(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 24) >> 16);
}

static uint16_t dispatch_hash_25(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 25) >> 16);
}

static uint16_t dispatch_hash_26(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 26) >> 16);
}

static uint16_t dispatch_hash_27(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 27) >> 16);
}

static uint16_t dispatch_hash_28(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 28) >> 16);
}

static uint16_t dispatch_hash_29(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 29) >> 16);
}

static uint16_t dispatch_hash_30(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 30) >> 16);
}

static uint16_t dispatch_hash_31(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 31) >> 16);
}

static uint16_t dispatch_hash_32(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 32) >> 16);
}

static uint16_t dispatch_hash_33(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 33) >> 16);
}

static uint16_t dispatch_hash_34(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 34) >> 16);
}

static uint16_t dispatch_hash_35(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 35) >> 16);
}

static uint16_t dispatch_hash_36(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 36) >> 16);
}

static uint16_t dispatch_hash_37(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 37) >> 16);
}

static uint16_t dispatch_hash_38(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 38) >> 16);
}

static uint16_t dispatch_hash_39(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 39) >> 16);
}

static uint16_t dispatch_hash_40(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 40) >> 16);
}

static uint16_t dispatch_hash_41(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 41) >> 16);
}

static uint16_t dispatch_hash_42(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 42) >> 16);
}

static uint16_t dispatch_hash_43(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 43) >> 16);
}

static uint16_t dispatch_hash_44(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 44) >> 16);
}

static uint16_t dispatch_hash_45(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 45) >> 16);
}

static uint16_t dispatch_hash_46(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 46) >> 16);
}

static uint16_t dispatch_hash_47(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 47) >> 16);
}

static uint16_t dispatch_hash_48(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 48) >> 16);
}

static uint16_t dispatch_hash_49(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 49) >> 16);
}

static uint16_t dispatch_hash_50(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 50) >> 16);
}

static uint16_t dispatch_hash_51(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 51) >> 16);
}

static uint16_t dispatch_hash_52(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 52) >> 16);
}

static uint16_t dispatch_hash_53(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 53) >> 16);
}

static uint16_t dispatch_hash_54(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 54) >> 16);
}

static uint16_t dispatch_hash_55(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 55) >> 16);
}

static uint16_t dispatch_hash_56(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 56) >> 16);
}

static uint16_t dispatch_hash_57(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 57) >> 16);
}

static uint16_t dispatch_hash_58(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 58) >> 16);
}

static uint16_t dispatch_hash_59(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 59) >> 16);
}

static uint16_t dispatch_hash_60(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 60) >> 16);
}

static uint16_t dispatch_hash_61(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 61) >> 16);
}

static uint16_t dispatch_hash_62(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 62) >> 16);
}

static uint16_t dispatch_hash_63(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 63) >> 16);
}

static uint16_t dispatch_hash_64(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 64) >> 16);
}

static uint16_t dispatch_hash_65(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 65) >> 16);
}

static uint16_t dispatch_hash_66(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 66) >> 16);
}

static uint16_t dispatch_hash_67(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 67) >> 16);
}

static uint16_t dispatch_hash_68(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 68) >> 16);
}

static uint16_t dispatch_hash_69(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 69) >> 16);
}

static uint16_t dispatch_hash_70(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 70) >> 16);
}

static uint16_t dispatch_hash_71(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 71) >> 16);
}

static uint16_t dispatch_hash_72(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 72) >> 16);
}

static uint16_t dispatch_hash_73(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 73) >> 16);
}

static uint16_t dispatch_hash_74(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 74) >> 16);
}

static uint16_t dispatch_hash_75(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 75) >> 16);
}

static uint16_t dispatch_hash_76(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 76) >> 16);
}

static uint16_t dispatch_hash_77(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 77) >> 16);
}

static uint16_t dispatch_hash_78(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 78) >> 16);
}

static uint16_t dispatch_hash_79(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 79) >> 16);
}

static uint16_t dispatch_hash_80(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 80) >> 16);
}

static uint16_t dispatch_hash_81(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 81) >> 16);
}

static uint16_t dispatch_hash_82(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 82) >> 16);
}

static uint16_t dispatch_hash_83(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 83) >> 16);
}

static uint16_t dispatch_hash_84(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 84) >> 16);
}

static uint16_t dispatch_hash_85(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 85) >> 16);
}

static uint16_t dispatch_hash_86(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 86) >> 16);
}

static uint16_t dispatch_hash_87(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 87) >> 16);
}

static uint16_t dispatch_hash_88(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 88) >> 16);
}

static uint16_t dispatch_hash_89(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 89) >> 16);
}

static uint16_t dispatch_hash_90(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 90) >> 16);
}

static uint16_t dispatch_hash_91(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 91) >> 16);
}

static uint16_t dispatch_hash_92(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 92) >> 16);
}

static uint16_t dispatch_hash_93(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 93) >> 16);
}

static uint16_t dispatch_hash_94(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 94) >> 16);
}

static uint16_t dispatch_hash_95(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 95) >> 16);
}

static uint16_t dispatch_hash_96(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 96) >> 16);
}

static uint16_t dispatch_hash_97(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 97) >> 16);
}

static uint16_t dispatch_hash_98(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 98) >> 16);
}

static uint16_t dispatch_hash_99(uint16_t dest) noexcept {
    return static_cast<uint16_t>((dest * 2654435761u + 99) >> 16);
}

} // namespace aeroframe
