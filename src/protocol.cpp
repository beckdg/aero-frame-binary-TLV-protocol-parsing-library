#include "protocol.h"
#include <cstring>

namespace aeroframe {

static uint16_t readBE16(const uint8_t* p) {
    return (static_cast<uint16_t>(p[0]) << 8) |
            static_cast<uint16_t>(p[1]);
}

static uint32_t readBE32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) <<  8) |
            static_cast<uint32_t>(p[3]);
}

ProtocolEngine::ProtocolEngine()
    : state_(ProtocolState::Closed)
    , sequence_counter_(0)
    , capability_count_(0)
    , context_valid_(false)
    , version_(0)
{
    std::memset(capabilities_, 0, sizeof(capabilities_));
    std::memset(&context_, 0, sizeof(context_));
}

bool ProtocolEngine::declare_capability(uint8_t id, uint8_t version, uint16_t flags) {
    if (capability_count_ >= CAPABILITY_TABLE_SIZE) return false;
    if (id >= CAPABILITY_TABLE_SIZE) return false;

    capabilities_[capability_count_].id = id;
    capabilities_[capability_count_].version = version;
    capabilities_[capability_count_].flags = flags;
    capabilities_[capability_count_].active = true;
    capability_count_++;
    return true;
}

bool ProtocolEngine::negotiate_capabilities() {
    if (capability_count_ == 0) return false;
    state_ = ProtocolState::Negotiating;
    return true;
}

void ProtocolEngine::reset_context() {
    context_valid_ = false;
    std::memset(&context_, 0, sizeof(context_));
}

bool ProtocolEngine::handle_message(ProtocolMessageType type,
                                    const uint8_t* body, size_t body_len,
                                    std::vector<uint8_t>& response)
{
    response.clear();

    switch (type) {
        case ProtocolMessageType::VersionDeclare: {
            if (body_len < 1) return false;
            uint8_t ver = body[0];
            if (ver != PROTOCOL_VERSION) return false;
            version_ = ver;
            state_ = ProtocolState::WaitingCapabilities;
            response.push_back(static_cast<uint8_t>(ProtocolMessageType::VersionAccept));
            response.push_back(PROTOCOL_VERSION);
            break;
        }

        case ProtocolMessageType::CapabilityDeclare: {
            if (body_len < 4) return false;
            uint8_t cap_id = body[0];

            if (cap_id >= CAPABILITY_TABLE_SIZE) return false;

            CapabilityEntry entry;
            entry.id = cap_id;
            entry.version = body[1];
            entry.flags = readBE16(body + 2);
            entry.active = true;

            // BUG B2 (type confusion):
            // cap_id is validated < CAPABILITY_TABLE_SIZE (12), but
            // declare_capability also checks cap_id < CAPABILITY_TABLE_SIZE.
            // However, capabilities_[] stores entries sequentially at
            // capability_count_, not at cap_id index. The table below stores
            // entries compactly. This function stores at cap_id directly.
            // When cap_id >= capability_count_, it writes past the "logical"
            // end of the table, creating a gap that other code paths
            // read from using capability_count_ as boundary.

            // Additionally, the context is set here and then used by
            // the DATA message handler without clearing. This allows
            // a DATA message to be dispatched using CapabilityDeclare's
            // context via the dispatch_data_with_context code path.
            capabilities_[capability_count_].id = cap_id;     // stored at compact index
            capabilities_[capability_count_].version = entry.version;
            capabilities_[capability_count_].flags = entry.flags;
            capabilities_[capability_count_].active = true;
            capability_count_++;

            // Also store at direct index — creates two views of the same data
            if (cap_id < CAPABILITY_TABLE_SIZE) {
                capabilities_[cap_id] = entry; // direct-index store
            }

            context_.pending_type = type;
            context_.sequence = readBE16(body + 2) & 0x00FF;
            context_.timestamp = readBE32(body);
            context_.context_valid = true;

            response.push_back(static_cast<uint8_t>(ProtocolMessageType::CapabilityAck));
            response.push_back(cap_id);
            break;
        }

        case ProtocolMessageType::Data: {
            if (body_len < 1) return false;

            // BUG B2 continued:
            // When context_.context_valid is true and the pending_type
            // from a CapabilityDeclare message is still set, this code
            // uses the context which has a sequence and timestamp from
            // the CapabilityDeclare body, not from this Data message.
            // The timestamp field of the CapabilityDeclare body (4 bytes
            // starting at offset 0) is reinterpreted as context data,
            // and the sequence is taken from the flags field of the
            // capability body. This type confusion causes the dispatch
            // to use wrong metadata for processing.
            if (context_valid_) {
                uint8_t dispatch_type = static_cast<uint8_t>(context_.pending_type);
                // dispatch_type is now a CapabilityDeclare control code (0x20),
                // which the calling code may not expect for a Data message.
                response.push_back(dispatch_type);
                response.push_back(static_cast<uint8_t>(context_.sequence & 0xFF));
            }

            // Normal data processing
            response.push_back(static_cast<uint8_t>(type));
            response.insert(response.end(), body, body + body_len);
            break;
        }

        case ProtocolMessageType::Heartbeat: {
            state_ = ProtocolState::Established;
            response.push_back(static_cast<uint8_t>(ProtocolMessageType::Heartbeat));
            response.push_back(0x01);
            break;
        }

        case ProtocolMessageType::Ack: {
            if (body_len < 2) return false;
            response.push_back(static_cast<uint8_t>(ProtocolMessageType::Ack));
            response.insert(response.end(), body, body + body_len);
            break;
        }

        case ProtocolMessageType::Nak: {
            if (body_len < 2) return false;
            response.push_back(static_cast<uint8_t>(ProtocolMessageType::Nak));
            response.insert(response.end(), body, body + body_len);
            break;
        }

        default:
            return false;
    }
    return true;
}

} // namespace aeroframe
