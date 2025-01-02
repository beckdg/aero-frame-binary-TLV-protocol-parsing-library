#include <gtest/gtest.h>
#include <vector>
#include "aeroframe.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// Helper: compute CRC16 for a buffer
// -----------------------------------------------------------------------
static uint16_t crc16(const uint8_t* buf, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i)
        crc = (crc << 8) ^ CRC16_TABLE[((crc >> 8) ^ buf[i]) & 0xFF];
    return crc;
}

// -----------------------------------------------------------------------
// Helper: build a valid packet with one TLV
// -----------------------------------------------------------------------
static std::vector<uint8_t> make_packet(uint16_t seq, uint16_t tlv_type,
                                         const uint8_t* tlv_val, size_t tlv_len)
{
    size_t total = HEADER_SIZE + 4 + tlv_len + CRC_SIZE;
    std::vector<uint8_t> pkt(total, 0);

    // Header
    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01; // magic
    pkt[4] = static_cast<uint8_t>(seq >> 8);
    pkt[5] = static_cast<uint8_t>(seq & 0xFF);
    pkt[6] = static_cast<uint8_t>(total >> 8);
    pkt[7] = static_cast<uint8_t>(total & 0xFF);

    // TLV header
    size_t off = HEADER_SIZE;
    pkt[off++] = static_cast<uint8_t>(tlv_type >> 8);
    pkt[off++] = static_cast<uint8_t>(tlv_type & 0xFF);
    pkt[off++] = static_cast<uint8_t>(tlv_len >> 8);
    pkt[off++] = static_cast<uint8_t>(tlv_len & 0xFF);

    // TLV value
    if (tlv_len > 0)
        std::memcpy(pkt.data() + off, tlv_val, tlv_len);

    // CRC
    uint16_t crc = crc16(pkt.data(), total - CRC_SIZE);
    pkt[total - 2] = static_cast<uint8_t>(crc >> 8);
    pkt[total - 1] = static_cast<uint8_t>(crc & 0xFF);

    return pkt;
}

// -----------------------------------------------------------------------
// AeroParser — parse valid and malformed packets
// -----------------------------------------------------------------------

TEST(ParserTest, ValidPacket) {
    const uint8_t val[] = {0x00, 0x01, 0x48, 0x65, 0x6C}; // type=0x01, len=5, "Hel"
    auto pkt = make_packet(1, 0x01, val, 5);
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
    EXPECT_EQ(result.header.magic, MAGIC_HEADER);
    EXPECT_EQ(result.header.sequence_id, 1);
    ASSERT_EQ(result.tlvs.size(), 1);
    EXPECT_EQ(result.tlvs[0].type, 0x01);
    EXPECT_EQ(result.tlvs[0].length, 5);
}

TEST(ParserTest, PacketTooShort) {
    std::vector<uint8_t> data(5, 0); // less than MIN_PKT_SIZE (10)
    AeroParser parser;
    auto result = parser.parse(data);
    EXPECT_EQ(result.error, ErrorCode::PacketTooShort);
}

TEST(ParserTest, InvalidMagic) {
    std::vector<uint8_t> data(10, 0); // wrong magic
    AeroParser parser;
    auto result = parser.parse(data);
    EXPECT_EQ(result.error, ErrorCode::InvalidMagic);
}

TEST(ParserTest, LengthFieldTooSmall) {
    std::vector<uint8_t> data(10, 0);
    data[0] = 0xAF; data[1] = 0x01; data[2] = 0xAF; data[3] = 0x01; // valid magic
    data[6] = 0x00; data[7] = 0x05; // total_length=5 < MIN_PKT_SIZE
    AeroParser parser;
    auto result = parser.parse(data);
    EXPECT_EQ(result.error, ErrorCode::LengthMismatch);
}

TEST(ParserTest, LengthExceedsBuffer) {
    std::vector<uint8_t> data(10, 0);
    data[0] = 0xAF; data[1] = 0x01; data[2] = 0xAF; data[3] = 0x01;
    data[6] = 0x00; data[7] = 0x64; // total_length=100 > data.size()=10
    AeroParser parser;
    auto result = parser.parse(data);
    EXPECT_EQ(result.error, ErrorCode::LengthMismatch);
}

TEST(ParserTest, InvalidCRC) {
    const uint8_t val[] = {'H', 'e', 'l', 'l', 'o'};
    auto pkt = make_packet(1, 0x01, val, 5);
    pkt.back() ^= 0xFF; // corrupt CRC
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::InvalidCRC);
}

TEST(ParserTest, SessionCmdOpen) {
    uint8_t val[] = {0x00, 0x00, 0x00, 0x2A, 0x01}; // session_id=42, action=open
    auto pkt = make_packet(1, TYPE_SESSION_CMD, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, SessionCmdClose) {
    uint8_t val[] = {0x00, 0x00, 0x00, 0x2A, 0x02}; // session_id=42, action=close
    auto pkt = make_packet(1, TYPE_SESSION_CMD, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, CompressedData) {
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x01,   // session_id=1
        0x03, 0x41, 0x42, 0x43,   // RLE: literals "ABC"
    };
    auto pkt = make_packet(1, TYPE_COMPRESSED_DATA, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, FragmentTLV) {
    // [4B msg_id] [2B frag_id] [2B total] [2B offset] [2B length] [payload]
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x01,   // msg_id=1
        0x00, 0x00,               // frag_id=0
        0x00, 0x01,               // total=1
        0x00, 0x00,               // offset=0
        0x00, 0x05,               // length=5
        'H', 'e', 'l', 'l', 'o',
    };
    auto pkt = make_packet(1, TYPE_FRAGMENT, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, RetransmitReq) {
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x01,   // msg_id=1
        0x00, 0x00,               // frag_id=0
    };
    auto pkt = make_packet(1, TYPE_RETRANSMIT_REQ, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, ProtocolMsg) {
    uint8_t val[] = {0x04}; // Heartbeat
    auto pkt = make_packet(1, TYPE_PROTOCOL_MSG, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, CipheredData) {
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x2A,   // session_id=42
        0x01, 0x02, 0x03, 0x04,   // ciphertext (small, < key_len == 16, safe path)
    };
    auto pkt = make_packet(1, TYPE_CIPHERED_DATA, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, RoutingUpdate) {
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x64,   // route_id=100
        0x0A, 0x00,               // dest=0x0A00
        0x02,                     // priority=high
        0x0A,                     // ttl=10
        0x00, 0x01,               // hop_count=1
        0x01, 0x01, 0x01, 0x01,   // hop[0]
    };
    auto pkt = make_packet(1, TYPE_ROUTING_UPDATE, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, QueueStatusEnqueue) {
    uint8_t val[] = {
        0x00,                     // action=enqueue
        0x00, 0x00, 0x00, 0x01,   // msg_id=1
        0x00, 0x05,               // payload_len=5
        'H', 'e', 'l', 'l', 'o',
    };
    auto pkt = make_packet(1, TYPE_QUEUE_STATUS, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, MultiCompress) {
    uint8_t val[] = {
        0x00, 0x00, 0x00, 0x01,   // session_id=1
        0x01,                     // method=RLE
        0x03, 0x41, 0x42, 0x43,   // RLE: literal "ABC"
    };
    auto pkt = make_packet(1, TYPE_MULTI_COMPRESS, val, sizeof(val));
    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
}

TEST(ParserTest, MultipleTLVs) {
    // Two TLVs in one packet: session + fragment
    uint8_t pld[64];
    size_t off = 0;

    // TLV 1: session cmd
    pld[off++] = 0x00; pld[off++] = TYPE_SESSION_CMD;
    pld[off++] = 0x00; pld[off++] = 0x05;
    pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x2A;
    pld[off++] = 0x01;

    // TLV 2: fragment
    pld[off++] = 0x00; pld[off++] = TYPE_FRAGMENT;
    pld[off++] = 0x00; pld[off++] = 0x10;
    pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x02;
    pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x01;
    pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x00; pld[off++] = 0x05;
    const char* frag_payload = "Hello";
    std::memcpy(pld + off, frag_payload, 5); off += 5;

    size_t total = HEADER_SIZE + off + CRC_SIZE;
    std::vector<uint8_t> pkt(total, 0);
    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01;
    pkt[4] = 0x00; pkt[5] = 0x01;
    pkt[6] = static_cast<uint8_t>(total >> 8);
    pkt[7] = static_cast<uint8_t>(total & 0xFF);
    std::memcpy(pkt.data() + HEADER_SIZE, pld, off);
    uint16_t crc = crc16(pkt.data(), total - CRC_SIZE);
    pkt[total - 2] = static_cast<uint8_t>(crc >> 8);
    pkt[total - 1] = static_cast<uint8_t>(crc & 0xFF);

    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
    ASSERT_EQ(result.tlvs.size(), 2);
    EXPECT_EQ(result.tlvs[0].type, TYPE_SESSION_CMD);
    EXPECT_EQ(result.tlvs[1].type, TYPE_FRAGMENT);
}

TEST(ParserTest, TruncatedTLV) {
    std::vector<uint8_t> pkt = {0xAF, 0x01, 0xAF, 0x01, 0x00, 0x01};
    pkt.push_back(0x00); pkt.push_back(0x0F); // total_length=15
    // TLV header says length=20 but only 3 bytes remain in buffer
    pkt.push_back(0x00); pkt.push_back(0x01); // type=1
    pkt.push_back(0x00); pkt.push_back(0x64); // length=100 (too large)
    pkt.resize(12);
    pkt.push_back(0x00); pkt.push_back(0x00); // CRC padding
    uint16_t crc = crc16(pkt.data(), pkt.size() - CRC_SIZE);
    pkt[pkt.size() - 2] = static_cast<uint8_t>(crc >> 8);
    pkt[pkt.size() - 1] = static_cast<uint8_t>(crc & 0xFF);

    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::TruncatedTLV);
}

TEST(ParserTest, EmptyPayload) {
    // No TLVs, just header + CRC
    size_t total = HEADER_SIZE + CRC_SIZE;
    std::vector<uint8_t> pkt(total, 0);
    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01;
    pkt[4] = 0x00; pkt[5] = 0x01;
    pkt[6] = static_cast<uint8_t>(total >> 8);
    pkt[7] = static_cast<uint8_t>(total & 0xFF);
    uint16_t crc = crc16(pkt.data(), total - CRC_SIZE);
    pkt[total - 2] = static_cast<uint8_t>(crc >> 8);
    pkt[total - 1] = static_cast<uint8_t>(crc & 0xFF);

    AeroParser parser;
    auto result = parser.parse(pkt);
    EXPECT_EQ(result.error, ErrorCode::None);
    EXPECT_TRUE(result.tlvs.empty());
}

TEST(ParserTest, ErrorToString) {
    EXPECT_EQ(errorToString(ErrorCode::None), "No error");
    EXPECT_EQ(errorToString(ErrorCode::InvalidMagic), "Invalid magic header");
    EXPECT_EQ(errorToString(ErrorCode::PacketTooShort), "Packet too short");
    EXPECT_EQ(errorToString(ErrorCode::LengthMismatch), "Length field mismatch");
    EXPECT_EQ(errorToString(ErrorCode::InvalidCRC), "CRC validation failed");
    EXPECT_EQ(errorToString(ErrorCode::TruncatedTLV), "Truncated TLV chunk");
    EXPECT_EQ(errorToString(ErrorCode::DecompressFail), "Decompression failed");
    EXPECT_EQ(errorToString(ErrorCode::CipherError), "Cipher operation error");
    EXPECT_EQ(errorToString(ErrorCode::ProtocolError), "Protocol engine error");
    EXPECT_EQ(errorToString(ErrorCode::FragmentError), "Fragment reassembly error");
    auto ec = static_cast<ErrorCode>(0xFF);
    EXPECT_EQ(errorToString(ec), "Unknown error code");
}
