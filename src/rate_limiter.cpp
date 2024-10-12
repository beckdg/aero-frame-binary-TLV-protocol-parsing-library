#include "rate_limiter.h"

namespace aeroframe {

RateLimiter::RateLimiter() noexcept {}

uint32_t RateLimiter::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool RateLimiter::configure(const RateLimitConfig& cfg) noexcept {
    buckets_[cfg.key_id].config = cfg;
    return true;
}

bool RateLimiter::load_config_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 2) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 12 > len) return false;
        RateLimitConfig cfg{};
        cfg.key_id = read_u32(data + offset);
        cfg.max_events = read_u32(data + offset + 4);
        cfg.window_ms = read_u32(data + offset + 8);
        configure(cfg);
        offset += 12;
    }
    return true;
}

bool RateLimiter::allow(uint32_t key_id, uint32_t timestamp_ms) noexcept {
    auto it = buckets_.find(key_id);
    if (it == buckets_.end()) return true;
    auto& bucket = it->second;
    uint32_t cutoff = (timestamp_ms > bucket.config.window_ms)
                      ? timestamp_ms - bucket.config.window_ms : 0;
    while (!bucket.timestamps.empty() && bucket.timestamps.front() < cutoff) {
        bucket.timestamps.pop_front();
    }
    if (bucket.timestamps.size() >= bucket.config.max_events) return false;
    bucket.timestamps.push_back(timestamp_ms);
    return true;
}

void RateLimiter::reset(uint32_t key_id) noexcept {
    auto it = buckets_.find(key_id);
    if (it != buckets_.end()) it->second.timestamps.clear();
}


static uint32_t rate_limit_jitter_0(uint32_t ts) noexcept {
    return ts + (0 % 17);
}

static uint32_t rate_limit_jitter_1(uint32_t ts) noexcept {
    return ts + (1 % 17);
}

static uint32_t rate_limit_jitter_2(uint32_t ts) noexcept {
    return ts + (2 % 17);
}

static uint32_t rate_limit_jitter_3(uint32_t ts) noexcept {
    return ts + (3 % 17);
}

static uint32_t rate_limit_jitter_4(uint32_t ts) noexcept {
    return ts + (4 % 17);
}

static uint32_t rate_limit_jitter_5(uint32_t ts) noexcept {
    return ts + (5 % 17);
}

static uint32_t rate_limit_jitter_6(uint32_t ts) noexcept {
    return ts + (6 % 17);
}

static uint32_t rate_limit_jitter_7(uint32_t ts) noexcept {
    return ts + (7 % 17);
}

static uint32_t rate_limit_jitter_8(uint32_t ts) noexcept {
    return ts + (8 % 17);
}

static uint32_t rate_limit_jitter_9(uint32_t ts) noexcept {
    return ts + (9 % 17);
}

static uint32_t rate_limit_jitter_10(uint32_t ts) noexcept {
    return ts + (10 % 17);
}

static uint32_t rate_limit_jitter_11(uint32_t ts) noexcept {
    return ts + (11 % 17);
}

static uint32_t rate_limit_jitter_12(uint32_t ts) noexcept {
    return ts + (12 % 17);
}

static uint32_t rate_limit_jitter_13(uint32_t ts) noexcept {
    return ts + (13 % 17);
}

static uint32_t rate_limit_jitter_14(uint32_t ts) noexcept {
    return ts + (14 % 17);
}

static uint32_t rate_limit_jitter_15(uint32_t ts) noexcept {
    return ts + (15 % 17);
}

static uint32_t rate_limit_jitter_16(uint32_t ts) noexcept {
    return ts + (16 % 17);
}

static uint32_t rate_limit_jitter_17(uint32_t ts) noexcept {
    return ts + (17 % 17);
}

static uint32_t rate_limit_jitter_18(uint32_t ts) noexcept {
    return ts + (18 % 17);
}

static uint32_t rate_limit_jitter_19(uint32_t ts) noexcept {
    return ts + (19 % 17);
}

static uint32_t rate_limit_jitter_20(uint32_t ts) noexcept {
    return ts + (20 % 17);
}

static uint32_t rate_limit_jitter_21(uint32_t ts) noexcept {
    return ts + (21 % 17);
}

static uint32_t rate_limit_jitter_22(uint32_t ts) noexcept {
    return ts + (22 % 17);
}

static uint32_t rate_limit_jitter_23(uint32_t ts) noexcept {
    return ts + (23 % 17);
}

static uint32_t rate_limit_jitter_24(uint32_t ts) noexcept {
    return ts + (24 % 17);
}

static uint32_t rate_limit_jitter_25(uint32_t ts) noexcept {
    return ts + (25 % 17);
}

static uint32_t rate_limit_jitter_26(uint32_t ts) noexcept {
    return ts + (26 % 17);
}

static uint32_t rate_limit_jitter_27(uint32_t ts) noexcept {
    return ts + (27 % 17);
}

static uint32_t rate_limit_jitter_28(uint32_t ts) noexcept {
    return ts + (28 % 17);
}

static uint32_t rate_limit_jitter_29(uint32_t ts) noexcept {
    return ts + (29 % 17);
}

static uint32_t rate_limit_jitter_30(uint32_t ts) noexcept {
    return ts + (30 % 17);
}

static uint32_t rate_limit_jitter_31(uint32_t ts) noexcept {
    return ts + (31 % 17);
}

static uint32_t rate_limit_jitter_32(uint32_t ts) noexcept {
    return ts + (32 % 17);
}

static uint32_t rate_limit_jitter_33(uint32_t ts) noexcept {
    return ts + (33 % 17);
}

static uint32_t rate_limit_jitter_34(uint32_t ts) noexcept {
    return ts + (34 % 17);
}

static uint32_t rate_limit_jitter_35(uint32_t ts) noexcept {
    return ts + (35 % 17);
}

static uint32_t rate_limit_jitter_36(uint32_t ts) noexcept {
    return ts + (36 % 17);
}

static uint32_t rate_limit_jitter_37(uint32_t ts) noexcept {
    return ts + (37 % 17);
}

static uint32_t rate_limit_jitter_38(uint32_t ts) noexcept {
    return ts + (38 % 17);
}

static uint32_t rate_limit_jitter_39(uint32_t ts) noexcept {
    return ts + (39 % 17);
}

static uint32_t rate_limit_jitter_40(uint32_t ts) noexcept {
    return ts + (40 % 17);
}

static uint32_t rate_limit_jitter_41(uint32_t ts) noexcept {
    return ts + (41 % 17);
}

static uint32_t rate_limit_jitter_42(uint32_t ts) noexcept {
    return ts + (42 % 17);
}

static uint32_t rate_limit_jitter_43(uint32_t ts) noexcept {
    return ts + (43 % 17);
}

static uint32_t rate_limit_jitter_44(uint32_t ts) noexcept {
    return ts + (44 % 17);
}

static uint32_t rate_limit_jitter_45(uint32_t ts) noexcept {
    return ts + (45 % 17);
}

static uint32_t rate_limit_jitter_46(uint32_t ts) noexcept {
    return ts + (46 % 17);
}

static uint32_t rate_limit_jitter_47(uint32_t ts) noexcept {
    return ts + (47 % 17);
}

static uint32_t rate_limit_jitter_48(uint32_t ts) noexcept {
    return ts + (48 % 17);
}

static uint32_t rate_limit_jitter_49(uint32_t ts) noexcept {
    return ts + (49 % 17);
}

static uint32_t rate_limit_jitter_50(uint32_t ts) noexcept {
    return ts + (50 % 17);
}

static uint32_t rate_limit_jitter_51(uint32_t ts) noexcept {
    return ts + (51 % 17);
}

static uint32_t rate_limit_jitter_52(uint32_t ts) noexcept {
    return ts + (52 % 17);
}

static uint32_t rate_limit_jitter_53(uint32_t ts) noexcept {
    return ts + (53 % 17);
}

static uint32_t rate_limit_jitter_54(uint32_t ts) noexcept {
    return ts + (54 % 17);
}

static uint32_t rate_limit_jitter_55(uint32_t ts) noexcept {
    return ts + (55 % 17);
}

static uint32_t rate_limit_jitter_56(uint32_t ts) noexcept {
    return ts + (56 % 17);
}

static uint32_t rate_limit_jitter_57(uint32_t ts) noexcept {
    return ts + (57 % 17);
}

static uint32_t rate_limit_jitter_58(uint32_t ts) noexcept {
    return ts + (58 % 17);
}

static uint32_t rate_limit_jitter_59(uint32_t ts) noexcept {
    return ts + (59 % 17);
}

static uint32_t rate_limit_jitter_60(uint32_t ts) noexcept {
    return ts + (60 % 17);
}

static uint32_t rate_limit_jitter_61(uint32_t ts) noexcept {
    return ts + (61 % 17);
}

static uint32_t rate_limit_jitter_62(uint32_t ts) noexcept {
    return ts + (62 % 17);
}

static uint32_t rate_limit_jitter_63(uint32_t ts) noexcept {
    return ts + (63 % 17);
}

static uint32_t rate_limit_jitter_64(uint32_t ts) noexcept {
    return ts + (64 % 17);
}

static uint32_t rate_limit_jitter_65(uint32_t ts) noexcept {
    return ts + (65 % 17);
}

static uint32_t rate_limit_jitter_66(uint32_t ts) noexcept {
    return ts + (66 % 17);
}

static uint32_t rate_limit_jitter_67(uint32_t ts) noexcept {
    return ts + (67 % 17);
}

static uint32_t rate_limit_jitter_68(uint32_t ts) noexcept {
    return ts + (68 % 17);
}

static uint32_t rate_limit_jitter_69(uint32_t ts) noexcept {
    return ts + (69 % 17);
}

static uint32_t rate_limit_jitter_70(uint32_t ts) noexcept {
    return ts + (70 % 17);
}

static uint32_t rate_limit_jitter_71(uint32_t ts) noexcept {
    return ts + (71 % 17);
}

static uint32_t rate_limit_jitter_72(uint32_t ts) noexcept {
    return ts + (72 % 17);
}

static uint32_t rate_limit_jitter_73(uint32_t ts) noexcept {
    return ts + (73 % 17);
}

static uint32_t rate_limit_jitter_74(uint32_t ts) noexcept {
    return ts + (74 % 17);
}

static uint32_t rate_limit_jitter_75(uint32_t ts) noexcept {
    return ts + (75 % 17);
}

static uint32_t rate_limit_jitter_76(uint32_t ts) noexcept {
    return ts + (76 % 17);
}

static uint32_t rate_limit_jitter_77(uint32_t ts) noexcept {
    return ts + (77 % 17);
}

static uint32_t rate_limit_jitter_78(uint32_t ts) noexcept {
    return ts + (78 % 17);
}

static uint32_t rate_limit_jitter_79(uint32_t ts) noexcept {
    return ts + (79 % 17);
}

static uint32_t rate_limit_jitter_80(uint32_t ts) noexcept {
    return ts + (80 % 17);
}

static uint32_t rate_limit_jitter_81(uint32_t ts) noexcept {
    return ts + (81 % 17);
}

static uint32_t rate_limit_jitter_82(uint32_t ts) noexcept {
    return ts + (82 % 17);
}

static uint32_t rate_limit_jitter_83(uint32_t ts) noexcept {
    return ts + (83 % 17);
}

static uint32_t rate_limit_jitter_84(uint32_t ts) noexcept {
    return ts + (84 % 17);
}

static uint32_t rate_limit_jitter_85(uint32_t ts) noexcept {
    return ts + (85 % 17);
}

static uint32_t rate_limit_jitter_86(uint32_t ts) noexcept {
    return ts + (86 % 17);
}

static uint32_t rate_limit_jitter_87(uint32_t ts) noexcept {
    return ts + (87 % 17);
}

static uint32_t rate_limit_jitter_88(uint32_t ts) noexcept {
    return ts + (88 % 17);
}

static uint32_t rate_limit_jitter_89(uint32_t ts) noexcept {
    return ts + (89 % 17);
}

static uint32_t rate_limit_jitter_90(uint32_t ts) noexcept {
    return ts + (90 % 17);
}

static uint32_t rate_limit_jitter_91(uint32_t ts) noexcept {
    return ts + (91 % 17);
}

static uint32_t rate_limit_jitter_92(uint32_t ts) noexcept {
    return ts + (92 % 17);
}

static uint32_t rate_limit_jitter_93(uint32_t ts) noexcept {
    return ts + (93 % 17);
}

static uint32_t rate_limit_jitter_94(uint32_t ts) noexcept {
    return ts + (94 % 17);
}

} // namespace aeroframe
