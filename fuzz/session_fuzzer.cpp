#include <cstdint>
#include <cstddef>

#include "session.h"

// Fuzz-driven state-machine: the input is parsed as a sequence of commands,
// each with the following layout:
//
//   [1 byte action]  [4 bytes session_id (big-endian)]  [optional payload]
//
// Actions:
//   0x00  –  open_session(id)
//   0x01  –  append_telemetry(id, payload): the next 2 bytes are a big-endian
//            length, followed by that many payload bytes.
//   0x02  –  close_session(id)
//
// All other action values are silently skipped so the fuzzer can explore
// interleaved sequences without crashing on unknown opcodes.

static uint32_t readBE32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) <<  8) |
            static_cast<uint32_t>(p[3]);
}

static uint16_t readBE16(const uint8_t* p) {
    return (static_cast<uint16_t>(p[0]) << 8) |
            static_cast<uint16_t>(p[1]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    aeroframe::SessionManager sm;
    size_t offset = 0;

    while (offset < size) {
        // Every command needs at least [action(1) + session_id(4)] = 5 bytes.
        if (offset + 5 > size) break;

        uint8_t  action     = data[offset];
        uint32_t session_id = readBE32(data + offset + 1);
        offset += 5;

        switch (action) {
            case 0x00: // open
                sm.open_session(session_id);
                break;

            case 0x01: { // append telemetry
                if (offset + 2 > size) break;
                uint16_t payload_len = readBE16(data + offset);
                offset += 2;

                if (offset + payload_len > size) break;
                sm.append_telemetry(session_id, data + offset, payload_len);
                offset += payload_len;
                break;
            }

            case 0x02: // close
                sm.close_session(session_id);
                break;

            default:
                // Unknown action – skip ahead (no bytes consumed beyond the
                // header we already read), but continue scanning so the
                // fuzzer can still exercise later commands.
                break;
        }
    }

    return 0;
}
