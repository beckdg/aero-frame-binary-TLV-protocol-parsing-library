#include "aeroframe.h"
#include "compression.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

// ---------------------------------------------------------------------------
// Constructor — initializes cipher with default key for session encryption
// ---------------------------------------------------------------------------

AeroParser::AeroParser()
    : cipher_(reinterpret_cast<const uint8_t*>(
          "\xDE\xAD\xBE\xEF\xCA\xFE\xBA\xBE"
          "\xFE\xED\xFA\xCE\xDE\xAD\xBE\xEF"), 16)
{
}

// ---------------------------------------------------------------------------
// Big-endian helpers
// ---------------------------------------------------------------------------

uint16_t AeroParser::readBE16(const uint8_t* p) noexcept {
    return (static_cast<uint16_t>(p[0]) << 8) |
            static_cast<uint16_t>(p[1]);
}

uint32_t AeroParser::readBE32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) <<  8) |
            static_cast<uint32_t>(p[3]);
}

// ---------------------------------------------------------------------------
// CRC-16-CCITT (polynomial 0x1021, init 0xFFFF, no final XOR)
// ---------------------------------------------------------------------------

uint16_t AeroParser::crc16(const uint8_t* data, size_t len) noexcept {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = (crc << 8) ^ CRC16_TABLE[((crc >> 8) ^ data[i]) & 0xFF];
    }
    return crc;
}

// ---------------------------------------------------------------------------
// TLV parsing
// ---------------------------------------------------------------------------

bool AeroParser::parseTLVs(const uint8_t* data, size_t len, std::vector<TLVChunk>& tlvs) noexcept {
    size_t offset = 0;
    while (offset < len) {
        if (offset + 4 > len) return false;

        uint16_t type = readBE16(data + offset);
        uint16_t tlv_len = readBE16(data + offset + 2);
        offset += 4;

        if (offset + tlv_len > len) return false;

        TLVChunk chunk;
        chunk.type   = type;
        chunk.length = tlv_len;
        chunk.value.assign(data + offset, data + offset + tlv_len);
        tlvs.push_back(std::move(chunk));

        offset += tlv_len;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Top-level parse
// ---------------------------------------------------------------------------

ParsedPacket AeroParser::parse(const std::vector<uint8_t>& data) noexcept {
    ParsedPacket result{};

    if (data.size() < MIN_PKT_SIZE) {
        result.error = ErrorCode::PacketTooShort;
        return result;
    }

    result.header.magic = readBE32(data.data());
    if (result.header.magic != MAGIC_HEADER) {
        result.error = ErrorCode::InvalidMagic;
        return result;
    }

    result.header.sequence_id  = readBE16(data.data() + 4);
    result.header.total_length = readBE16(data.data() + 6);

    if (result.header.total_length < MIN_PKT_SIZE) {
        result.error = ErrorCode::LengthMismatch;
        return result;
    }
    if (static_cast<size_t>(result.header.total_length) > data.size()) {
        result.error = ErrorCode::LengthMismatch;
        return result;
    }

    size_t payload_len = result.header.total_length - HEADER_SIZE - CRC_SIZE;
    if (payload_len > 0) {
        if (!parseTLVs(data.data() + HEADER_SIZE, payload_len, result.tlvs)) {
            result.error = ErrorCode::TruncatedTLV;
            return result;
        }
    }

    for (const auto& tlv : result.tlvs) {
        switch (tlv.type) {
            case TYPE_SESSION_CMD:      processSessionCmd(tlv);      break;
            case TYPE_COMPRESSED_DATA:  processCompressedData(tlv);  break;
            case TYPE_FRAGMENT:         processFragment(tlv);        break;
            case TYPE_RETRANSMIT_REQ:   processRetransmitReq(tlv);   break;
            case TYPE_PROTOCOL_MSG:     processProtocolMsg(tlv);     break;
            case TYPE_CIPHERED_DATA:    processCipheredData(tlv);    break;
            case TYPE_ROUTING_UPDATE:   processRoutingUpdate(tlv);   break;
            case TYPE_QUEUE_STATUS:     processQueueStatus(tlv);     break;
            case TYPE_MULTI_COMPRESS:   processMultiCompress(tlv);   break;
            case TYPE_TELEMETRY_BATCH:  processTelemetryBatch(tlv);  break;
            case TYPE_INGEST_PACKET:    processIngestPacket(tlv);    break;
            case TYPE_SCHEMA_DEF:       processSchemaDef(tlv);       break;
            case TYPE_VALIDATION_RULES: processValidationRules(tlv); break;
            default: break;
        }
    }

    size_t crc_offset = result.header.total_length - CRC_SIZE;
    result.crc = readBE16(data.data() + crc_offset);

    uint16_t computed = crc16(data.data(), crc_offset);
    if (computed != result.crc) {
        result.error = ErrorCode::InvalidCRC;
        return result;
    }

    return result;
}

// ---------------------------------------------------------------------------
// TYPE_SESSION_CMD (0x02) handler
// ---------------------------------------------------------------------------
void AeroParser::processSessionCmd(const TLVChunk& tlv) noexcept {
    if (tlv.length < 5) return;
    uint32_t id     = readBE32(tlv.value.data());
    uint8_t  action = tlv.value.data()[4];
    switch (action) {
        case 0x01: session_manager_.open_session(id);  break;
        case 0x02: session_manager_.close_session(id); break;
        default:   break;
    }
}

// ---------------------------------------------------------------------------
// TYPE_COMPRESSED_DATA (0x03) handler
// ---------------------------------------------------------------------------
void AeroParser::processCompressedData(const TLVChunk& tlv) noexcept {
    if (tlv.length < 5) return;
    uint32_t       session_id     = readBE32(tlv.value.data());
    const uint8_t* compressed     = tlv.value.data() + 4;
    size_t         compressed_len = tlv.length - 4;

    uint8_t decompress_buf[4096];
    size_t  decompressed_len = 0;

    if (decompress_payload(compressed, compressed_len,
                           decompress_buf, sizeof(decompress_buf),
                           decompressed_len))
    {
        session_manager_.append_telemetry(session_id,
                                          decompress_buf,
                                          decompressed_len);
    }
}

// ---------------------------------------------------------------------------
// TYPE_FRAGMENT (0x04) handler
// Value: [4B msg_id] [2B frag_id] [2B total_frags] [2B offset] [2B length] [payload...]
// ---------------------------------------------------------------------------
void AeroParser::processFragment(const TLVChunk& tlv) noexcept {
    if (tlv.length < 12) return;
    const uint8_t* v = tlv.value.data();
    uint32_t msg_id   = readBE32(v);
    uint16_t frag_id  = readBE16(v + 4);
    uint16_t total    = readBE16(v + 6);
    uint16_t offset   = readBE16(v + 8);
    uint16_t flen     = readBE16(v + 10);
    if (tlv.length < static_cast<size_t>(12) + flen) return;

    fragment_reassembler_.push_fragment(msg_id, frag_id, total, offset,
                                        v + 12, flen);
}

// ---------------------------------------------------------------------------
// TYPE_RETRANSMIT_REQ (0x05) handler
// Value: [4B msg_id] [2B frag_id]
// ---------------------------------------------------------------------------
void AeroParser::processRetransmitReq(const TLVChunk& tlv) noexcept {
    if (tlv.length < 6) return;
    uint32_t msg_id  = readBE32(tlv.value.data());
    uint16_t frag_id = readBE16(tlv.value.data() + 4);
    fragment_reassembler_.request_retransmit(msg_id, frag_id);
}

// ---------------------------------------------------------------------------
// TYPE_PROTOCOL_MSG (0x06) handler
// Value: [1B msg_type] [body...]
// Routes to ProtocolEngine for stateful processing.
// ---------------------------------------------------------------------------
void AeroParser::processProtocolMsg(const TLVChunk& tlv) noexcept {
    if (tlv.length < 1) return;
    ProtocolMessageType msg_type = static_cast<ProtocolMessageType>(tlv.value.data()[0]);
    const uint8_t* body = tlv.value.data() + 1;
    size_t body_len = tlv.length - 1;

    std::vector<uint8_t> response;
    protocol_engine_.handle_message(msg_type, body, body_len, response);
}

// ---------------------------------------------------------------------------
// TYPE_CIPHERED_DATA (0x07) handler
// Value: [4B session_id] [ciphertext...]
// Decrypts using the session's cipher context and appends plaintext.
// ---------------------------------------------------------------------------
void AeroParser::processCipheredData(const TLVChunk& tlv) noexcept {
    if (tlv.length < 5) return;
    uint32_t session_id = readBE32(tlv.value.data());
    const uint8_t* ciphertext = tlv.value.data() + 4;
    size_t ct_len = tlv.length - 4;

    uint8_t plaintext[4096];
    size_t pt_len = sizeof(plaintext);

    if (cipher_.decrypt(ciphertext, ct_len, plaintext, pt_len)) {
        session_manager_.append_telemetry(session_id, plaintext, pt_len);
    }
}

// ---------------------------------------------------------------------------
// TYPE_ROUTING_UPDATE (0x08) handler
// Value: [4B route_id] [2B dest_addr] [1B priority] [1B ttl] [2B hop_count] [hops...]
// ---------------------------------------------------------------------------
void AeroParser::processRoutingUpdate(const TLVChunk& tlv) noexcept {
    if (tlv.length < 10) return;
    const uint8_t* v = tlv.value.data();
    uint32_t route_id  = readBE32(v);
    uint16_t dest_addr = readBE16(v + 4);
    uint8_t  priority  = v[6];
    uint8_t  ttl       = v[7];
    uint16_t hop_count = readBE16(v + 8);

    if (tlv.length < static_cast<size_t>(10) + hop_count * 4) return;
    if (hop_count > 64) return;

    routing_table_.add_route(route_id, dest_addr, priority, ttl,
                             reinterpret_cast<const uint32_t*>(v + 10),
                             hop_count);
}

// ---------------------------------------------------------------------------
// TYPE_QUEUE_STATUS (0x09) handler
// Value: [1B action] [4B msg_id] [2B payload_len] [payload...]
// Actions: 0=enqueue, 1=dequeue, 2=promote, 3=demote
// ---------------------------------------------------------------------------
void AeroParser::processQueueStatus(const TLVChunk& tlv) noexcept {
    if (tlv.length < 7) return;
    const uint8_t* v = tlv.value.data();
    uint8_t action = v[0];
    uint32_t msg_id = readBE32(v + 1);
    uint16_t payload_len = readBE16(v + 5);

    switch (action) {
        case 0: { // enqueue
            if (tlv.length < static_cast<size_t>(7) + payload_len) return;
            QueueMessage msg;
            msg.id = msg_id;
            msg.data = const_cast<uint8_t*>(v + 7);
            msg.length = payload_len;
            msg.priority = QueuePriority::Normal;
            msg.enqueue_time = 0;
            msg.promotion_count = 0;
            priority_queue_.enqueue(msg);
            break;
        }
        case 2: // promote
            priority_queue_.promote(msg_id);
            break;
        case 3: // demote
            priority_queue_.demote(msg_id);
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// TYPE_MULTI_COMPRESS (0x0A) handler
// Value: [4B session_id] [1B method] [compressed payload...]
// Supports multiple compression algorithms via the compress dispatch.
// ---------------------------------------------------------------------------
void AeroParser::processMultiCompress(const TLVChunk& tlv) noexcept {
    if (tlv.length < 6) return;
    const uint8_t* v = tlv.value.data();
    uint32_t session_id  = readBE32(v);
    uint8_t  method_byte = v[4];
    const uint8_t* compressed    = v + 5;
    size_t         compressed_len = tlv.length - 5;

    CompressMethod method = static_cast<CompressMethod>(method_byte);
    uint8_t decompress_buf[8192];
    size_t  decompressed_len = 0;

    if (decompress(method, compressed, compressed_len,
                   decompress_buf, sizeof(decompress_buf),
                   decompressed_len))
    {
        session_manager_.append_telemetry(session_id,
                                          decompress_buf,
                                          decompressed_len);
    }
}

// ---------------------------------------------------------------------------
// TYPE_TELEMETRY_BATCH (0x0B) handler
// Value: batch of length-prefixed telemetry records
// ---------------------------------------------------------------------------
void AeroParser::processTelemetryBatch(const TLVChunk& tlv) noexcept {
    if (tlv.length < 2) return;
    std::vector<DecodedTelemetry> decoded;
    telemetry_decoder_.decode_batch(tlv.value.data(), tlv.value.size(), decoded);
    for (const auto& rec : decoded) {
        if (rec.type == TelemetryRecordType::GpsFix && rec.valid) {
            uint8_t buf[16];
            buf[0] = static_cast<uint8_t>(rec.gps.fix_quality);
            buf[1] = rec.gps.satellites;
            session_manager_.append_telemetry(rec.gps.timestamp_ms & 0xFFFF,
                                              buf, 2);
        }
    }
}

// ---------------------------------------------------------------------------
// TYPE_INGEST_PACKET (0x0C) handler
// Value: full ingest pipeline packet (magic IG...)
// ---------------------------------------------------------------------------
void AeroParser::processIngestPacket(const TLVChunk& tlv) noexcept {
    if (tlv.length < 6) return;
    auto result = ingest_pipeline_.process_packet(tlv.value.data(), tlv.value.size());
    (void)result;
}

// ---------------------------------------------------------------------------
// TYPE_SCHEMA_DEF (0x0D) handler
// ---------------------------------------------------------------------------
void AeroParser::processSchemaDef(const TLVChunk& tlv) noexcept {
    if (tlv.length < 8) return;
    schema_registry_.load_binary_schema(tlv.value.data(), tlv.value.size());
}

// ---------------------------------------------------------------------------
// TYPE_VALIDATION_RULES (0x0E) handler
// ---------------------------------------------------------------------------
void AeroParser::processValidationRules(const TLVChunk& tlv) noexcept {
    if (tlv.length < 4) return;
    binary_validator_.load_rules_binary(tlv.value.data(), tlv.value.size());
    binary_validator_.validate(tlv.value.data(), tlv.value.size());
}

// ---------------------------------------------------------------------------
// Error-code -> string
// ---------------------------------------------------------------------------

std::string_view errorToString(ErrorCode ec) noexcept {
    switch (ec) {
        case ErrorCode::None:           return "No error";
        case ErrorCode::InvalidMagic:   return "Invalid magic header";
        case ErrorCode::PacketTooShort: return "Packet too short";
        case ErrorCode::LengthMismatch: return "Length field mismatch";
        case ErrorCode::InvalidCRC:     return "CRC validation failed";
        case ErrorCode::TruncatedTLV:   return "Truncated TLV chunk";
        case ErrorCode::DecompressFail: return "Decompression failed";
        case ErrorCode::CipherError:    return "Cipher operation error";
        case ErrorCode::ProtocolError:  return "Protocol engine error";
        case ErrorCode::FragmentError:  return "Fragment reassembly error";
        default:                        return "Unknown error code";
    }
}

} // namespace aeroframe
