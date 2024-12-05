#include <cstdint>
#include <cstddef>

#include "fragment.h"

// Fuzz the fragment reassembly engine directly.
// Input format is a sequence of commands.
//
// Command byte determines operation:
//   0x00 – push fragment
//           4B msg_id  2B frag_id  2B total  2B offset  2B len  [payload]
//   0x01 – request retransmit
//           4B msg_id  2B frag_id
//   0x02 – process retransmit
//           4B msg_id  2B frag_id
//   0x03 – discard message
//           4B msg_id
//   0x04 – assemble message
//           4B msg_id
//   0xFF – end

static uint32_t r32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) <<  8) |
            static_cast<uint32_t>(p[3]);
}

static uint16_t r16(const uint8_t* p) {
    return (static_cast<uint16_t>(p[0]) << 8) |
            static_cast<uint16_t>(p[1]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    aeroframe::FragmentReassembler fr;
    size_t off = 0;

    while (off < size) {
        uint8_t cmd = data[off++];

        switch (cmd) {
            case 0x00: { // push fragment
                if (off + 12 > size) break;
                uint32_t msg_id  = r32(data + off); off += 4;
                uint16_t frag_id = r16(data + off); off += 2;
                uint16_t total   = r16(data + off); off += 2;
                uint16_t offset  = r16(data + off); off += 2;
                uint16_t len     = r16(data + off); off += 2;
                if (off + len > size) break;
                fr.push_fragment(msg_id, frag_id, total, offset,
                                 data + off, len);
                off += len;
                break;
            }
            case 0x01: { // request retransmit
                if (off + 6 > size) break;
                uint32_t msg_id  = r32(data + off); off += 4;
                uint16_t frag_id = r16(data + off); off += 2;
                fr.request_retransmit(msg_id, frag_id);
                break;
            }
            case 0x02: { // process retransmit
                if (off + 6 > size) break;
                uint32_t msg_id  = r32(data + off); off += 4;
                uint16_t frag_id = r16(data + off); off += 2;
                fr.process_retransmit(msg_id, frag_id);
                break;
            }
            case 0x03: { // discard message
                if (off + 4 > size) break;
                uint32_t msg_id = r32(data + off); off += 4;
                fr.discard_message(msg_id);
                break;
            }
            case 0x04: { // assemble message
                if (off + 4 > size) break;
                uint32_t msg_id = r32(data + off); off += 4;
                std::vector<uint8_t> out;
                fr.assemble_message(msg_id, out);
                volatile size_t n = out.size();
                (void)n;
                break;
            }
            default:
                break;
        }
    }
    return 0;
}
