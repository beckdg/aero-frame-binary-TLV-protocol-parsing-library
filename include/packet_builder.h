#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "aeroframe.h"

namespace aeroframe {

class PacketBuilder {
public:
    PacketBuilder() noexcept;

    void reset() noexcept;
    void set_sequence(uint16_t seq) noexcept;
    bool append_tlv(uint16_t type, const uint8_t* value, size_t len) noexcept;
    bool append_tlv(uint16_t type, const std::vector<uint8_t>& value) noexcept;
    bool finalize(std::vector<uint8_t>& out) noexcept;

    size_t tlv_count() const noexcept { return tlv_payload_.size(); }

private:
    static void write_be16(uint8_t* p, uint16_t v) noexcept;
    static void write_be32(uint8_t* p, uint32_t v) noexcept;
    static uint16_t crc16(const uint8_t* data, size_t len) noexcept;

    uint16_t sequence_id_;
    std::vector<uint8_t> tlv_payload_;
};

} // namespace aeroframe
