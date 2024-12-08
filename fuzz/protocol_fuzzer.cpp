#include <cstdint>
#include <cstddef>
#include <vector>

#include "protocol.h"

// Fuzz the protocol engine state machine.
// Input format:
//   [1B message_type] [body...]
//
// Message types mapped from input byte:
//   0x01 -> Data
//   0x02 -> Ack
//   0x03 -> Nak
//   0x04 -> Heartbeat
//   0x10 -> VersionDeclare
//   0x20 -> CapabilityDeclare
// Other values -> invalid (tests error handling)

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    aeroframe::ProtocolEngine engine;

    // Pre-declare some capabilities so the fuzzer has state to work with
    engine.declare_capability(0, 1, 0x0001);
    engine.declare_capability(1, 1, 0x0002);
    engine.declare_capability(2, 2, 0x0004);

    size_t off = 0;
    while (off < size) {
        uint8_t msg_byte = data[off++];
        size_t body_len = (off < size) ? (size - off) : 0;

        // Use a sliding window: each iteration uses the remainder of the
        // buffer as the message body, so the fuzzer explores varying body sizes.
        auto msg_type = static_cast<aeroframe::ProtocolMessageType>(msg_byte);

        std::vector<uint8_t> response;
        engine.handle_message(msg_type, data + off, body_len, response);

        // Consume one byte per iteration so the fuzzer advances through
        // different message-type/body combinations.
        off += 1;
        if (off > size) break;
    }

    // Exercise negotiation paths
    engine.negotiate_capabilities();
    engine.reset_context();

    // Read final state to keep the engine live
    volatile auto st = engine.get_state();
    (void)st;

    return 0;
}
