#include "packet_builder.h"
#include <cstring>

namespace aeroframe {

PacketBuilder::PacketBuilder() noexcept : sequence_id_(0) {}

void PacketBuilder::reset() noexcept {
    sequence_id_ = 0;
    tlv_payload_.clear();
}

void PacketBuilder::set_sequence(uint16_t seq) noexcept {
    sequence_id_ = seq;
}

void PacketBuilder::write_be16(uint8_t* p, uint16_t v) noexcept {
    p[0] = static_cast<uint8_t>(v >> 8);
    p[1] = static_cast<uint8_t>(v);
}

void PacketBuilder::write_be32(uint8_t* p, uint32_t v) noexcept {
    p[0] = static_cast<uint8_t>(v >> 24);
    p[1] = static_cast<uint8_t>(v >> 16);
    p[2] = static_cast<uint8_t>(v >> 8);
    p[3] = static_cast<uint8_t>(v);
}

uint16_t PacketBuilder::crc16(const uint8_t* data, size_t len) noexcept {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = static_cast<uint16_t>((crc << 8) ^ CRC16_TABLE[((crc >> 8) ^ data[i]) & 0xFF]);
    }
    return crc;
}

bool PacketBuilder::append_tlv(uint16_t type, const uint8_t* value, size_t len) noexcept {
    if (!value && len > 0) return false;
    if (len > 0xFFFF) return false;
    size_t old = tlv_payload_.size();
    tlv_payload_.resize(old + 4 + len);
    write_be16(tlv_payload_.data() + old, type);
    write_be16(tlv_payload_.data() + old + 2, static_cast<uint16_t>(len));
    if (len > 0) {
        std::memcpy(tlv_payload_.data() + old + 4, value, len);
    }
    return true;
}

bool PacketBuilder::append_tlv(uint16_t type, const std::vector<uint8_t>& value) noexcept {
    return append_tlv(type, value.data(), value.size());
}

bool PacketBuilder::finalize(std::vector<uint8_t>& out) noexcept {
    size_t total = HEADER_SIZE + tlv_payload_.size() + CRC_SIZE;
    if (total > 0xFFFF) return false;
    out.resize(total);
    write_be32(out.data(), MAGIC_HEADER);
    write_be16(out.data() + 4, sequence_id_);
    write_be16(out.data() + 6, static_cast<uint16_t>(total));
    if (!tlv_payload_.empty()) {
        std::memcpy(out.data() + HEADER_SIZE, tlv_payload_.data(), tlv_payload_.size());
    }
    uint16_t crc = crc16(out.data(), total - CRC_SIZE);
    write_be16(out.data() + total - CRC_SIZE, crc);
    return true;
}


static bool builder_check_tlv_0(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 0;
}

static uint32_t builder_hash_payload_0(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 0;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_1(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 1;
}

static uint32_t builder_hash_payload_1(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 1;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_2(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 2;
}

static uint32_t builder_hash_payload_2(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 2;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_3(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 3;
}

static uint32_t builder_hash_payload_3(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 3;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_4(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 4;
}

static uint32_t builder_hash_payload_4(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 4;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_5(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 5;
}

static uint32_t builder_hash_payload_5(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 5;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_6(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 6;
}

static uint32_t builder_hash_payload_6(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 6;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_7(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 7;
}

static uint32_t builder_hash_payload_7(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 7;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_8(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 8;
}

static uint32_t builder_hash_payload_8(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 8;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_9(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 9;
}

static uint32_t builder_hash_payload_9(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 9;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_10(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 10;
}

static uint32_t builder_hash_payload_10(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 10;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_11(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 11;
}

static uint32_t builder_hash_payload_11(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 11;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_12(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 12;
}

static uint32_t builder_hash_payload_12(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 12;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_13(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 13;
}

static uint32_t builder_hash_payload_13(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 13;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_14(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 14;
}

static uint32_t builder_hash_payload_14(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 14;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_15(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 15;
}

static uint32_t builder_hash_payload_15(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 15;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_16(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 16;
}

static uint32_t builder_hash_payload_16(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 16;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_17(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 17;
}

static uint32_t builder_hash_payload_17(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 17;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_18(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 18;
}

static uint32_t builder_hash_payload_18(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 18;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_19(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 19;
}

static uint32_t builder_hash_payload_19(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 19;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_20(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 20;
}

static uint32_t builder_hash_payload_20(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 20;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_21(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 21;
}

static uint32_t builder_hash_payload_21(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 21;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_22(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 22;
}

static uint32_t builder_hash_payload_22(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 22;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_23(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 23;
}

static uint32_t builder_hash_payload_23(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 23;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_24(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 24;
}

static uint32_t builder_hash_payload_24(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 24;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_25(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 25;
}

static uint32_t builder_hash_payload_25(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 25;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_26(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 26;
}

static uint32_t builder_hash_payload_26(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 26;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_27(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 27;
}

static uint32_t builder_hash_payload_27(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 27;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_28(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 28;
}

static uint32_t builder_hash_payload_28(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 28;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_29(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 29;
}

static uint32_t builder_hash_payload_29(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 29;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_30(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 30;
}

static uint32_t builder_hash_payload_30(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 30;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_31(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 31;
}

static uint32_t builder_hash_payload_31(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 31;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_32(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 32;
}

static uint32_t builder_hash_payload_32(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 32;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_33(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 33;
}

static uint32_t builder_hash_payload_33(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 33;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_34(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 34;
}

static uint32_t builder_hash_payload_34(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 34;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_35(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 35;
}

static uint32_t builder_hash_payload_35(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 35;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_36(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 36;
}

static uint32_t builder_hash_payload_36(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 36;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_37(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 37;
}

static uint32_t builder_hash_payload_37(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 37;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_38(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 38;
}

static uint32_t builder_hash_payload_38(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 38;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_39(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 39;
}

static uint32_t builder_hash_payload_39(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 39;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_40(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 40;
}

static uint32_t builder_hash_payload_40(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 40;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_41(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 41;
}

static uint32_t builder_hash_payload_41(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 41;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_42(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 42;
}

static uint32_t builder_hash_payload_42(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 42;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_43(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 43;
}

static uint32_t builder_hash_payload_43(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 43;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_44(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 44;
}

static uint32_t builder_hash_payload_44(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 44;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_45(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 45;
}

static uint32_t builder_hash_payload_45(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 45;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_46(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 46;
}

static uint32_t builder_hash_payload_46(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 46;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_47(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 47;
}

static uint32_t builder_hash_payload_47(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 47;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_48(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 48;
}

static uint32_t builder_hash_payload_48(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 48;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_49(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 49;
}

static uint32_t builder_hash_payload_49(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 49;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_50(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 50;
}

static uint32_t builder_hash_payload_50(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 50;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_51(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 51;
}

static uint32_t builder_hash_payload_51(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 51;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_52(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 52;
}

static uint32_t builder_hash_payload_52(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 52;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_53(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 53;
}

static uint32_t builder_hash_payload_53(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 53;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_54(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 54;
}

static uint32_t builder_hash_payload_54(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 54;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_55(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 55;
}

static uint32_t builder_hash_payload_55(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 55;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_56(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 56;
}

static uint32_t builder_hash_payload_56(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 56;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_57(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 57;
}

static uint32_t builder_hash_payload_57(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 57;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_58(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 58;
}

static uint32_t builder_hash_payload_58(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 58;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_59(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 59;
}

static uint32_t builder_hash_payload_59(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 59;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_60(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 60;
}

static uint32_t builder_hash_payload_60(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 60;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_61(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 61;
}

static uint32_t builder_hash_payload_61(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 61;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_62(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 62;
}

static uint32_t builder_hash_payload_62(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 62;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_63(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 63;
}

static uint32_t builder_hash_payload_63(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 63;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_64(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 0;
}

static uint32_t builder_hash_payload_64(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 64;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_65(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 1;
}

static uint32_t builder_hash_payload_65(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 65;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_66(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 2;
}

static uint32_t builder_hash_payload_66(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 66;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_67(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 3;
}

static uint32_t builder_hash_payload_67(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 67;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_68(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 4;
}

static uint32_t builder_hash_payload_68(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 68;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_69(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 5;
}

static uint32_t builder_hash_payload_69(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 69;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_70(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 6;
}

static uint32_t builder_hash_payload_70(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 70;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_71(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 7;
}

static uint32_t builder_hash_payload_71(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 71;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_72(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 8;
}

static uint32_t builder_hash_payload_72(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 72;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_73(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 9;
}

static uint32_t builder_hash_payload_73(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 73;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_74(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 10;
}

static uint32_t builder_hash_payload_74(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 74;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_75(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 11;
}

static uint32_t builder_hash_payload_75(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 75;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_76(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 12;
}

static uint32_t builder_hash_payload_76(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 76;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_77(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 13;
}

static uint32_t builder_hash_payload_77(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 77;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_78(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 14;
}

static uint32_t builder_hash_payload_78(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 78;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_79(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 15;
}

static uint32_t builder_hash_payload_79(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 79;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_80(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 16;
}

static uint32_t builder_hash_payload_80(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 80;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_81(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 17;
}

static uint32_t builder_hash_payload_81(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 81;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_82(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 18;
}

static uint32_t builder_hash_payload_82(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 82;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_83(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 19;
}

static uint32_t builder_hash_payload_83(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 83;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_84(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 20;
}

static uint32_t builder_hash_payload_84(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 84;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_85(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 21;
}

static uint32_t builder_hash_payload_85(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 85;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_86(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 22;
}

static uint32_t builder_hash_payload_86(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 86;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_87(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 23;
}

static uint32_t builder_hash_payload_87(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 87;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_88(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 24;
}

static uint32_t builder_hash_payload_88(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 88;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_89(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 25;
}

static uint32_t builder_hash_payload_89(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 89;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_90(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 26;
}

static uint32_t builder_hash_payload_90(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 90;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_91(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 27;
}

static uint32_t builder_hash_payload_91(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 91;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_92(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 28;
}

static uint32_t builder_hash_payload_92(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 92;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_93(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 29;
}

static uint32_t builder_hash_payload_93(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 93;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_94(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 30;
}

static uint32_t builder_hash_payload_94(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 94;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_95(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 31;
}

static uint32_t builder_hash_payload_95(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 95;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_96(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 32;
}

static uint32_t builder_hash_payload_96(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 96;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_97(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 33;
}

static uint32_t builder_hash_payload_97(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 97;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_98(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 34;
}

static uint32_t builder_hash_payload_98(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 98;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_99(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 35;
}

static uint32_t builder_hash_payload_99(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 99;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_100(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 36;
}

static uint32_t builder_hash_payload_100(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 100;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_101(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 37;
}

static uint32_t builder_hash_payload_101(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 101;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_102(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 38;
}

static uint32_t builder_hash_payload_102(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 102;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_103(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 39;
}

static uint32_t builder_hash_payload_103(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 103;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_104(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 40;
}

static uint32_t builder_hash_payload_104(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 104;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_105(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 41;
}

static uint32_t builder_hash_payload_105(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 105;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_106(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 42;
}

static uint32_t builder_hash_payload_106(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 106;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_107(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 43;
}

static uint32_t builder_hash_payload_107(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 107;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_108(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 44;
}

static uint32_t builder_hash_payload_108(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 108;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_109(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 45;
}

static uint32_t builder_hash_payload_109(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 109;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_110(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 46;
}

static uint32_t builder_hash_payload_110(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 110;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_111(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 47;
}

static uint32_t builder_hash_payload_111(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 111;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_112(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 48;
}

static uint32_t builder_hash_payload_112(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 112;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_113(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 49;
}

static uint32_t builder_hash_payload_113(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 113;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_114(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 50;
}

static uint32_t builder_hash_payload_114(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 114;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_115(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 51;
}

static uint32_t builder_hash_payload_115(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 115;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_116(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 52;
}

static uint32_t builder_hash_payload_116(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 116;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_117(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 53;
}

static uint32_t builder_hash_payload_117(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 117;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_118(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 54;
}

static uint32_t builder_hash_payload_118(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 118;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

static bool builder_check_tlv_119(uint16_t type, size_t len) noexcept {
    if (type == 0) return false;
    return len <= 4096 + 55;
}

static uint32_t builder_hash_payload_119(const uint8_t* data, size_t len) noexcept {
    uint32_t h = 119;
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}

} // namespace aeroframe
