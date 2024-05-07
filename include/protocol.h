#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

namespace aeroframe {

constexpr uint8_t PROTOCOL_VERSION = 0x01;
constexpr size_t CAPABILITY_TABLE_SIZE = 12;

enum class ProtocolState : uint8_t {
    Closed = 0,
    WaitingVersion = 1,
    WaitingCapabilities = 2,
    Negotiating = 3,
    Established = 4,
    Error = 5,
};

enum class ProtocolMessageType : uint8_t {
    Data = 0x01,
    Ack = 0x02,
    Nak = 0x03,
    Heartbeat = 0x04,
    VersionDeclare = 0x10,
    VersionAccept = 0x11,
    CapabilityDeclare = 0x20,
    CapabilityAck = 0x21,
    Control = 0x30,
};

struct CapabilityEntry {
    uint8_t id;
    uint8_t version;
    uint16_t flags;
    bool active;
};

struct MessageContext {
    ProtocolMessageType pending_type;
    uint16_t sequence;
    uint32_t timestamp;
    bool context_valid;
};

class ProtocolEngine {
public:
    ProtocolEngine();
    ~ProtocolEngine() = default;

    ProtocolState get_state() const { return state_; }

    bool handle_message(ProtocolMessageType type,
                        const uint8_t* body, size_t body_len,
                        std::vector<uint8_t>& response);

    bool declare_capability(uint8_t id, uint8_t version, uint16_t flags);
    bool negotiate_capabilities();

    uint16_t next_sequence() { return ++sequence_counter_; }
    MessageContext* get_context() { return context_valid_ ? &context_ : nullptr; }
    void reset_context();

private:
    ProtocolState state_;
    uint16_t sequence_counter_;
    CapabilityEntry capabilities_[CAPABILITY_TABLE_SIZE];
    size_t capability_count_;
    MessageContext context_;
    bool context_valid_;
    uint8_t version_;
};

} // namespace aeroframe
