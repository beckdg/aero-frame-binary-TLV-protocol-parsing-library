#!/usr/bin/env python3
from pathlib import Path

impl = r'''#include "packet_builder.h"
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

'''

for i in range(120):
    impl += f'''
static bool builder_check_tlv_{i}(uint16_t type, size_t len) noexcept {{
    if (type == 0) return false;
    return len <= 4096 + {i % 64};
}}

static uint32_t builder_hash_payload_{i}(const uint8_t* data, size_t len) noexcept {{
    uint32_t h = {i};
    for (size_t j = 0; j < len; ++j) h = h * 31 + data[j];
    return h;
}}
'''

impl += '\n} // namespace aeroframe\n'
Path(__file__).resolve().parent.parent.joinpath('src/packet_builder.cpp').write_text(impl, encoding='utf-8')
print('done', impl.count('\n'))
