#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
#include <string_view>

#include "session.h"
#include "fragment.h"
#include "protocol.h"
#include "cipher.h"
#include "compress.h"
#include "routing.h"
#include "queue.h"
#include "telemetry_decoder.h"
#include "ingest_pipeline.h"
#include "binary_validator.h"
#include "schema_registry.h"

namespace aeroframe {

constexpr uint32_t MAGIC_HEADER = 0xAF01AF01;
constexpr size_t HEADER_SIZE    = 8;   // magic(4) + seq_id(2) + total_len(2)
constexpr size_t CRC_SIZE       = 2;
constexpr size_t MIN_PKT_SIZE   = HEADER_SIZE + CRC_SIZE; // 10

// TLV chunk type codes
constexpr uint16_t TYPE_SESSION_CMD      = 0x02;
constexpr uint16_t TYPE_COMPRESSED_DATA  = 0x03;
constexpr uint16_t TYPE_FRAGMENT         = 0x04;
constexpr uint16_t TYPE_RETRANSMIT_REQ   = 0x05;
constexpr uint16_t TYPE_PROTOCOL_MSG     = 0x06;
constexpr uint16_t TYPE_CIPHERED_DATA    = 0x07;
constexpr uint16_t TYPE_ROUTING_UPDATE   = 0x08;
constexpr uint16_t TYPE_QUEUE_STATUS     = 0x09;
constexpr uint16_t TYPE_MULTI_COMPRESS   = 0x0A;
constexpr uint16_t TYPE_TELEMETRY_BATCH  = 0x0B;
constexpr uint16_t TYPE_INGEST_PACKET    = 0x0C;
constexpr uint16_t TYPE_SCHEMA_DEF       = 0x0D;
constexpr uint16_t TYPE_VALIDATION_RULES = 0x0E;

enum class ErrorCode : uint8_t {
    None           = 0,
    InvalidMagic   = 1,
    PacketTooShort = 2,
    LengthMismatch = 3,
    InvalidCRC     = 4,
    TruncatedTLV   = 5,
    DecompressFail = 6,
    CipherError    = 7,
    ProtocolError  = 8,
    FragmentError  = 9,
};

struct PacketHeader {
    uint32_t magic;
    uint16_t sequence_id;
    uint16_t total_length;
};

struct TLVChunk {
    uint16_t type;
    uint16_t length; // length of value array
    std::vector<uint8_t> value;
};

struct ParsedPacket {
    PacketHeader          header{};
    std::vector<TLVChunk> tlvs{};
    uint16_t              crc{};
    ErrorCode             error{};
};

class AeroParser {
public:
    AeroParser();
    ParsedPacket parse(const std::vector<uint8_t>& data) noexcept;

private:
    static uint16_t readBE16(const uint8_t* p) noexcept;
    static uint32_t readBE32(const uint8_t* p) noexcept;
    static uint16_t crc16(const uint8_t* data, size_t len) noexcept;
    static bool     parseTLVs(const uint8_t* data, size_t len, std::vector<TLVChunk>& tlvs) noexcept;

    void processSessionCmd(const TLVChunk& tlv) noexcept;
    void processCompressedData(const TLVChunk& tlv) noexcept;
    void processFragment(const TLVChunk& tlv) noexcept;
    void processRetransmitReq(const TLVChunk& tlv) noexcept;
    void processProtocolMsg(const TLVChunk& tlv) noexcept;
    void processCipheredData(const TLVChunk& tlv) noexcept;
    void processRoutingUpdate(const TLVChunk& tlv) noexcept;
    void processQueueStatus(const TLVChunk& tlv) noexcept;
    void processMultiCompress(const TLVChunk& tlv) noexcept;
    void processTelemetryBatch(const TLVChunk& tlv) noexcept;
    void processIngestPacket(const TLVChunk& tlv) noexcept;
    void processSchemaDef(const TLVChunk& tlv) noexcept;
    void processValidationRules(const TLVChunk& tlv) noexcept;

    SessionManager      session_manager_;
    FragmentReassembler fragment_reassembler_;
    ProtocolEngine      protocol_engine_;
    XorCipher           cipher_;
    RoutingTable        routing_table_;
    PriorityQueue       priority_queue_;
    TelemetryDecoder    telemetry_decoder_;
    IngestPipeline      ingest_pipeline_;
    BinaryValidator     binary_validator_;
    SchemaRegistry      schema_registry_;
};

std::string_view errorToString(ErrorCode ec) noexcept;

constexpr std::array<uint16_t, 256> makeCRC16Table() noexcept {
    std::array<uint16_t, 256> table{};
    for (uint16_t i = 0; i < 256; ++i) {
        uint16_t crc = i << 8;
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
        }
        table[i] = crc;
    }
    return table;
}

inline constexpr auto CRC16_TABLE = makeCRC16Table();

} // namespace aeroframe
