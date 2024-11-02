#include "time_sync.h"

namespace aeroframe {

TimeSyncEngine::TimeSyncEngine() noexcept
    : estimated_offset_(0), estimated_drift_(0), last_sync_ms_(0) {}

uint32_t TimeSyncEngine::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

int32_t TimeSyncEngine::read_i32(const uint8_t* p) noexcept {
    return static_cast<int32_t>(read_u32(p));
}

bool TimeSyncEngine::parse_sync_message(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 20) return false;
    if (data[0] != 0x54 || data[1] != 0x53) return false;
    TimeSyncSample s{};
    s.local_ms = read_u32(data + 2);
    s.remote_ms = read_u32(data + 6);
    s.offset_ms = read_i32(data + 10);
    s.drift_ppm = read_i32(data + 14);
    s.stratum = data[18];
    s.flags = data[19];
    if (s.stratum > 15) return false;
    samples_.push_back(s);
    if (samples_.size() > 64) samples_.erase(samples_.begin());
    estimated_offset_ = s.offset_ms;
    estimated_drift_ = s.drift_ppm;
    last_sync_ms_ = s.local_ms;
    return true;
}

bool TimeSyncEngine::get_offset(int32_t& offset_ms) const noexcept {
    offset_ms = estimated_offset_;
    return samples_.size() > 0;
}

bool TimeSyncEngine::apply_correction(uint32_t& timestamp_ms) const noexcept {
    if (samples_.empty()) return false;
    int64_t corrected = static_cast<int64_t>(timestamp_ms) + estimated_offset_;
    if (corrected < 0) corrected = 0;
    timestamp_ms = static_cast<uint32_t>(corrected);
    return true;
}


static int32_t time_sync_filter_0(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_1(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_2(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_3(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_4(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_5(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_6(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_7(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_8(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_9(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_10(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_11(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_12(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_13(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_14(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_15(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_16(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_17(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_18(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_19(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_20(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_21(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_22(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_23(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_24(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_25(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_26(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_27(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_28(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_29(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_30(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_31(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_32(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_33(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_34(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_35(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_36(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_37(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_38(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_39(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_40(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_41(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_42(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_43(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_44(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_45(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_46(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_47(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_48(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_49(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_50(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_51(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_52(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_53(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_54(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_55(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_56(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_57(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_58(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_59(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_60(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_61(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_62(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_63(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_64(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_65(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_66(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_67(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_68(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_69(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_70(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_71(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_72(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_73(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_74(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_75(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_76(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_77(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_78(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_79(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_80(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_81(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_82(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_83(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_84(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_85(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_86(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_87(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_88(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_89(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_90(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_91(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_92(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_93(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_94(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_95(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_96(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_97(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_98(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_99(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_100(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_101(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_102(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

static int32_t time_sync_filter_103(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 5) / 4;
}

static int32_t time_sync_filter_104(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 6) / 4;
}

static int32_t time_sync_filter_105(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 0) / 4;
}

static int32_t time_sync_filter_106(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 1) / 4;
}

static int32_t time_sync_filter_107(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 2) / 4;
}

static int32_t time_sync_filter_108(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 3) / 4;
}

static int32_t time_sync_filter_109(int32_t offset, int32_t prev) noexcept {
    return (offset * 3 + prev + 4) / 4;
}

} // namespace aeroframe
