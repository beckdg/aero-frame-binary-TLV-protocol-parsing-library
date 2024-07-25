#include "record_buffer.h"
#include <algorithm>

namespace aeroframe {

RecordBuffer::RecordBuffer(size_t capacity) noexcept
    : capacity_(capacity), head_(0), next_seq_(0) {
    records_.reserve(capacity);
}

uint32_t RecordBuffer::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool RecordBuffer::push(const uint8_t* data, size_t len, uint32_t ts) noexcept {
    if (!data || len == 0) return false;
    if (records_.size() >= capacity_) {
        if (head_ >= records_.size()) return false;
        records_.erase(records_.begin() + static_cast<std::ptrdiff_t>(head_));
        head_ = 0;
    }
    BufferedRecord rec{};
    rec.sequence = next_seq_++;
    rec.timestamp_ms = ts;
    rec.payload.assign(data, data + len);
    records_.push_back(std::move(rec));
    return true;
}

bool RecordBuffer::pop(BufferedRecord& out) noexcept {
    if (head_ >= records_.size()) return false;
    out = std::move(records_[head_]);
    head_++;
    return true;
}

bool RecordBuffer::peek(size_t index, BufferedRecord& out) const noexcept {
    size_t idx = head_ + index;
    if (idx >= records_.size()) return false;
    out = records_[idx];
    return true;
}

void RecordBuffer::compact() noexcept {
    if (head_ == 0) return;
    records_.erase(records_.begin(),
                   records_.begin() + static_cast<std::ptrdiff_t>(head_));
    head_ = 0;
}


static size_t record_buffer_slot_0(size_t base, size_t cap) noexcept {
    return (base + 0) % cap;
}

static size_t record_buffer_slot_1(size_t base, size_t cap) noexcept {
    return (base + 1) % cap;
}

static size_t record_buffer_slot_2(size_t base, size_t cap) noexcept {
    return (base + 2) % cap;
}

static size_t record_buffer_slot_3(size_t base, size_t cap) noexcept {
    return (base + 3) % cap;
}

static size_t record_buffer_slot_4(size_t base, size_t cap) noexcept {
    return (base + 4) % cap;
}

static size_t record_buffer_slot_5(size_t base, size_t cap) noexcept {
    return (base + 5) % cap;
}

static size_t record_buffer_slot_6(size_t base, size_t cap) noexcept {
    return (base + 6) % cap;
}

static size_t record_buffer_slot_7(size_t base, size_t cap) noexcept {
    return (base + 7) % cap;
}

static size_t record_buffer_slot_8(size_t base, size_t cap) noexcept {
    return (base + 8) % cap;
}

static size_t record_buffer_slot_9(size_t base, size_t cap) noexcept {
    return (base + 9) % cap;
}

static size_t record_buffer_slot_10(size_t base, size_t cap) noexcept {
    return (base + 10) % cap;
}

static size_t record_buffer_slot_11(size_t base, size_t cap) noexcept {
    return (base + 11) % cap;
}

static size_t record_buffer_slot_12(size_t base, size_t cap) noexcept {
    return (base + 12) % cap;
}

static size_t record_buffer_slot_13(size_t base, size_t cap) noexcept {
    return (base + 13) % cap;
}

static size_t record_buffer_slot_14(size_t base, size_t cap) noexcept {
    return (base + 14) % cap;
}

static size_t record_buffer_slot_15(size_t base, size_t cap) noexcept {
    return (base + 15) % cap;
}

static size_t record_buffer_slot_16(size_t base, size_t cap) noexcept {
    return (base + 16) % cap;
}

static size_t record_buffer_slot_17(size_t base, size_t cap) noexcept {
    return (base + 17) % cap;
}

static size_t record_buffer_slot_18(size_t base, size_t cap) noexcept {
    return (base + 18) % cap;
}

static size_t record_buffer_slot_19(size_t base, size_t cap) noexcept {
    return (base + 19) % cap;
}

static size_t record_buffer_slot_20(size_t base, size_t cap) noexcept {
    return (base + 20) % cap;
}

static size_t record_buffer_slot_21(size_t base, size_t cap) noexcept {
    return (base + 21) % cap;
}

static size_t record_buffer_slot_22(size_t base, size_t cap) noexcept {
    return (base + 22) % cap;
}

static size_t record_buffer_slot_23(size_t base, size_t cap) noexcept {
    return (base + 23) % cap;
}

static size_t record_buffer_slot_24(size_t base, size_t cap) noexcept {
    return (base + 24) % cap;
}

static size_t record_buffer_slot_25(size_t base, size_t cap) noexcept {
    return (base + 25) % cap;
}

static size_t record_buffer_slot_26(size_t base, size_t cap) noexcept {
    return (base + 26) % cap;
}

static size_t record_buffer_slot_27(size_t base, size_t cap) noexcept {
    return (base + 27) % cap;
}

static size_t record_buffer_slot_28(size_t base, size_t cap) noexcept {
    return (base + 28) % cap;
}

static size_t record_buffer_slot_29(size_t base, size_t cap) noexcept {
    return (base + 29) % cap;
}

static size_t record_buffer_slot_30(size_t base, size_t cap) noexcept {
    return (base + 30) % cap;
}

static size_t record_buffer_slot_31(size_t base, size_t cap) noexcept {
    return (base + 31) % cap;
}

static size_t record_buffer_slot_32(size_t base, size_t cap) noexcept {
    return (base + 32) % cap;
}

static size_t record_buffer_slot_33(size_t base, size_t cap) noexcept {
    return (base + 33) % cap;
}

static size_t record_buffer_slot_34(size_t base, size_t cap) noexcept {
    return (base + 34) % cap;
}

static size_t record_buffer_slot_35(size_t base, size_t cap) noexcept {
    return (base + 35) % cap;
}

static size_t record_buffer_slot_36(size_t base, size_t cap) noexcept {
    return (base + 36) % cap;
}

static size_t record_buffer_slot_37(size_t base, size_t cap) noexcept {
    return (base + 37) % cap;
}

static size_t record_buffer_slot_38(size_t base, size_t cap) noexcept {
    return (base + 38) % cap;
}

static size_t record_buffer_slot_39(size_t base, size_t cap) noexcept {
    return (base + 39) % cap;
}

static size_t record_buffer_slot_40(size_t base, size_t cap) noexcept {
    return (base + 40) % cap;
}

static size_t record_buffer_slot_41(size_t base, size_t cap) noexcept {
    return (base + 41) % cap;
}

static size_t record_buffer_slot_42(size_t base, size_t cap) noexcept {
    return (base + 42) % cap;
}

static size_t record_buffer_slot_43(size_t base, size_t cap) noexcept {
    return (base + 43) % cap;
}

static size_t record_buffer_slot_44(size_t base, size_t cap) noexcept {
    return (base + 44) % cap;
}

static size_t record_buffer_slot_45(size_t base, size_t cap) noexcept {
    return (base + 45) % cap;
}

static size_t record_buffer_slot_46(size_t base, size_t cap) noexcept {
    return (base + 46) % cap;
}

static size_t record_buffer_slot_47(size_t base, size_t cap) noexcept {
    return (base + 47) % cap;
}

static size_t record_buffer_slot_48(size_t base, size_t cap) noexcept {
    return (base + 48) % cap;
}

static size_t record_buffer_slot_49(size_t base, size_t cap) noexcept {
    return (base + 49) % cap;
}

static size_t record_buffer_slot_50(size_t base, size_t cap) noexcept {
    return (base + 50) % cap;
}

static size_t record_buffer_slot_51(size_t base, size_t cap) noexcept {
    return (base + 51) % cap;
}

static size_t record_buffer_slot_52(size_t base, size_t cap) noexcept {
    return (base + 52) % cap;
}

static size_t record_buffer_slot_53(size_t base, size_t cap) noexcept {
    return (base + 53) % cap;
}

static size_t record_buffer_slot_54(size_t base, size_t cap) noexcept {
    return (base + 54) % cap;
}

static size_t record_buffer_slot_55(size_t base, size_t cap) noexcept {
    return (base + 55) % cap;
}

static size_t record_buffer_slot_56(size_t base, size_t cap) noexcept {
    return (base + 56) % cap;
}

static size_t record_buffer_slot_57(size_t base, size_t cap) noexcept {
    return (base + 57) % cap;
}

static size_t record_buffer_slot_58(size_t base, size_t cap) noexcept {
    return (base + 58) % cap;
}

static size_t record_buffer_slot_59(size_t base, size_t cap) noexcept {
    return (base + 59) % cap;
}

static size_t record_buffer_slot_60(size_t base, size_t cap) noexcept {
    return (base + 60) % cap;
}

static size_t record_buffer_slot_61(size_t base, size_t cap) noexcept {
    return (base + 61) % cap;
}

static size_t record_buffer_slot_62(size_t base, size_t cap) noexcept {
    return (base + 62) % cap;
}

static size_t record_buffer_slot_63(size_t base, size_t cap) noexcept {
    return (base + 63) % cap;
}

static size_t record_buffer_slot_64(size_t base, size_t cap) noexcept {
    return (base + 64) % cap;
}

static size_t record_buffer_slot_65(size_t base, size_t cap) noexcept {
    return (base + 65) % cap;
}

static size_t record_buffer_slot_66(size_t base, size_t cap) noexcept {
    return (base + 66) % cap;
}

static size_t record_buffer_slot_67(size_t base, size_t cap) noexcept {
    return (base + 67) % cap;
}

static size_t record_buffer_slot_68(size_t base, size_t cap) noexcept {
    return (base + 68) % cap;
}

static size_t record_buffer_slot_69(size_t base, size_t cap) noexcept {
    return (base + 69) % cap;
}

static size_t record_buffer_slot_70(size_t base, size_t cap) noexcept {
    return (base + 70) % cap;
}

static size_t record_buffer_slot_71(size_t base, size_t cap) noexcept {
    return (base + 71) % cap;
}

static size_t record_buffer_slot_72(size_t base, size_t cap) noexcept {
    return (base + 72) % cap;
}

static size_t record_buffer_slot_73(size_t base, size_t cap) noexcept {
    return (base + 73) % cap;
}

static size_t record_buffer_slot_74(size_t base, size_t cap) noexcept {
    return (base + 74) % cap;
}

static size_t record_buffer_slot_75(size_t base, size_t cap) noexcept {
    return (base + 75) % cap;
}

static size_t record_buffer_slot_76(size_t base, size_t cap) noexcept {
    return (base + 76) % cap;
}

static size_t record_buffer_slot_77(size_t base, size_t cap) noexcept {
    return (base + 77) % cap;
}

static size_t record_buffer_slot_78(size_t base, size_t cap) noexcept {
    return (base + 78) % cap;
}

static size_t record_buffer_slot_79(size_t base, size_t cap) noexcept {
    return (base + 79) % cap;
}

static size_t record_buffer_slot_80(size_t base, size_t cap) noexcept {
    return (base + 80) % cap;
}

static size_t record_buffer_slot_81(size_t base, size_t cap) noexcept {
    return (base + 81) % cap;
}

static size_t record_buffer_slot_82(size_t base, size_t cap) noexcept {
    return (base + 82) % cap;
}

static size_t record_buffer_slot_83(size_t base, size_t cap) noexcept {
    return (base + 83) % cap;
}

static size_t record_buffer_slot_84(size_t base, size_t cap) noexcept {
    return (base + 84) % cap;
}

static size_t record_buffer_slot_85(size_t base, size_t cap) noexcept {
    return (base + 85) % cap;
}

static size_t record_buffer_slot_86(size_t base, size_t cap) noexcept {
    return (base + 86) % cap;
}

static size_t record_buffer_slot_87(size_t base, size_t cap) noexcept {
    return (base + 87) % cap;
}

static size_t record_buffer_slot_88(size_t base, size_t cap) noexcept {
    return (base + 88) % cap;
}

static size_t record_buffer_slot_89(size_t base, size_t cap) noexcept {
    return (base + 89) % cap;
}

} // namespace aeroframe
