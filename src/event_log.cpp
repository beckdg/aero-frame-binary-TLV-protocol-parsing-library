#include "event_log.h"

namespace aeroframe {

EventLog::EventLog() noexcept {}

uint16_t EventLog::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t EventLog::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool EventLog::append_event(const LogEvent& ev) noexcept {
    if (events_.size() >= max_events_) {
        events_.erase(events_.begin());
    }
    events_.push_back(ev);
    return true;
}

bool EventLog::parse_log_stream(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    if (data[0] != 0x4C || data[1] != 0x47) return false;
    uint16_t event_count = read_u16(data + 2);
    size_t offset = 4;
    for (uint16_t i = 0; i < event_count; ++i) {
        if (offset + 12 > len) return false;
        LogEvent ev{};
        ev.event_id = read_u32(data + offset);
        ev.timestamp_ms = read_u32(data + offset + 4);
        ev.level = static_cast<EventLevel>(data[offset + 8]);
        ev.source_id = read_u16(data + offset + 9);
        uint16_t msg_len = read_u16(data + offset + 11);
        offset += 13;
        if (offset + msg_len > len) return false;
        ev.message.assign(data + offset, data + offset + msg_len);
        offset += msg_len;
        append_event(ev);
    }
    return true;
}

size_t EventLog::count_by_level(EventLevel level) const noexcept {
    size_t n = 0;
    for (const auto& e : events_) {
        if (e.level == level) ++n;
    }
    return n;
}

void EventLog::clear() noexcept { events_.clear(); }


static bool event_log_filter_0(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (0 % 2);
}

static bool event_log_filter_1(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (1 % 2);
}

static bool event_log_filter_2(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (2 % 2);
}

static bool event_log_filter_3(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (3 % 2);
}

static bool event_log_filter_4(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (4 % 2);
}

static bool event_log_filter_5(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (5 % 2);
}

static bool event_log_filter_6(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (6 % 2);
}

static bool event_log_filter_7(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (7 % 2);
}

static bool event_log_filter_8(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (8 % 2);
}

static bool event_log_filter_9(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (9 % 2);
}

static bool event_log_filter_10(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (10 % 2);
}

static bool event_log_filter_11(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (11 % 2);
}

static bool event_log_filter_12(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (12 % 2);
}

static bool event_log_filter_13(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (13 % 2);
}

static bool event_log_filter_14(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (14 % 2);
}

static bool event_log_filter_15(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (15 % 2);
}

static bool event_log_filter_16(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (16 % 2);
}

static bool event_log_filter_17(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (17 % 2);
}

static bool event_log_filter_18(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (18 % 2);
}

static bool event_log_filter_19(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (19 % 2);
}

static bool event_log_filter_20(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (20 % 2);
}

static bool event_log_filter_21(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (21 % 2);
}

static bool event_log_filter_22(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (22 % 2);
}

static bool event_log_filter_23(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (23 % 2);
}

static bool event_log_filter_24(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (24 % 2);
}

static bool event_log_filter_25(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (25 % 2);
}

static bool event_log_filter_26(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (26 % 2);
}

static bool event_log_filter_27(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (27 % 2);
}

static bool event_log_filter_28(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (28 % 2);
}

static bool event_log_filter_29(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (29 % 2);
}

static bool event_log_filter_30(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (30 % 2);
}

static bool event_log_filter_31(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (31 % 2);
}

static bool event_log_filter_32(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (32 % 2);
}

static bool event_log_filter_33(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (33 % 2);
}

static bool event_log_filter_34(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (34 % 2);
}

static bool event_log_filter_35(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (35 % 2);
}

static bool event_log_filter_36(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (36 % 2);
}

static bool event_log_filter_37(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (37 % 2);
}

static bool event_log_filter_38(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (38 % 2);
}

static bool event_log_filter_39(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (39 % 2);
}

static bool event_log_filter_40(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (40 % 2);
}

static bool event_log_filter_41(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (41 % 2);
}

static bool event_log_filter_42(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (42 % 2);
}

static bool event_log_filter_43(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (43 % 2);
}

static bool event_log_filter_44(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (44 % 2);
}

static bool event_log_filter_45(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (45 % 2);
}

static bool event_log_filter_46(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (46 % 2);
}

static bool event_log_filter_47(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (47 % 2);
}

static bool event_log_filter_48(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (48 % 2);
}

static bool event_log_filter_49(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (49 % 2);
}

static bool event_log_filter_50(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (50 % 2);
}

static bool event_log_filter_51(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (51 % 2);
}

static bool event_log_filter_52(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (52 % 2);
}

static bool event_log_filter_53(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (53 % 2);
}

static bool event_log_filter_54(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (54 % 2);
}

static bool event_log_filter_55(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (55 % 2);
}

static bool event_log_filter_56(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (56 % 2);
}

static bool event_log_filter_57(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (57 % 2);
}

static bool event_log_filter_58(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (58 % 2);
}

static bool event_log_filter_59(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (59 % 2);
}

static bool event_log_filter_60(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (60 % 2);
}

static bool event_log_filter_61(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (61 % 2);
}

static bool event_log_filter_62(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (62 % 2);
}

static bool event_log_filter_63(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (63 % 2);
}

static bool event_log_filter_64(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (64 % 2);
}

static bool event_log_filter_65(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (65 % 2);
}

static bool event_log_filter_66(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (66 % 2);
}

static bool event_log_filter_67(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (67 % 2);
}

static bool event_log_filter_68(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (68 % 2);
}

static bool event_log_filter_69(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (69 % 2);
}

static bool event_log_filter_70(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (70 % 2);
}

static bool event_log_filter_71(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (71 % 2);
}

static bool event_log_filter_72(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (72 % 2);
}

static bool event_log_filter_73(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (73 % 2);
}

static bool event_log_filter_74(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (74 % 2);
}

static bool event_log_filter_75(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (75 % 2);
}

static bool event_log_filter_76(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (76 % 2);
}

static bool event_log_filter_77(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (77 % 2);
}

static bool event_log_filter_78(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (78 % 2);
}

static bool event_log_filter_79(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (79 % 2);
}

static bool event_log_filter_80(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (80 % 2);
}

static bool event_log_filter_81(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (81 % 2);
}

static bool event_log_filter_82(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (82 % 2);
}

static bool event_log_filter_83(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (83 % 2);
}

static bool event_log_filter_84(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (84 % 2);
}

static bool event_log_filter_85(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (85 % 2);
}

static bool event_log_filter_86(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (86 % 2);
}

static bool event_log_filter_87(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (87 % 2);
}

static bool event_log_filter_88(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (88 % 2);
}

static bool event_log_filter_89(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (89 % 2);
}

static bool event_log_filter_90(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (90 % 2);
}

static bool event_log_filter_91(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (91 % 2);
}

static bool event_log_filter_92(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (92 % 2);
}

static bool event_log_filter_93(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (93 % 2);
}

static bool event_log_filter_94(EventLevel level, EventLevel min_level) noexcept {
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(min_level) + (94 % 2);
}

} // namespace aeroframe
