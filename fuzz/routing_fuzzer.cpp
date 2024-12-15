#include <cstdint>
#include <cstddef>

#include "routing.h"

// Fuzz the routing table with route add/remove/lookup operations.
//
// Command byte:
//   0x00 – add route
//           4B id  2B dest  1B prio  1B ttl  2B hop_count  [hop_count * 4B hops]
//   0x01 – remove route
//           4B id
//   0x02 – lookup
//           2B dest_addr
//   0x03 – update metric
//           4B id  2B delta
//   0x04 – age routes

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
    aeroframe::RoutingTable rt;
    size_t off = 0;

    while (off < size) {
        uint8_t cmd = data[off++];

        switch (cmd) {
            case 0x00: { // add route
                if (off + 10 > size) break;
                uint32_t id       = r32(data + off); off += 4;
                uint16_t dest     = r16(data + off); off += 2;
                uint8_t  prio     = data[off++];
                uint8_t  ttl      = data[off++];
                uint16_t hop_cnt  = r16(data + off); off += 2;
                if (hop_cnt > 32) break;
                if (off + hop_cnt * 4 > size) break;

                // Build hop array on stack
                uint32_t hops[32];
                for (uint16_t i = 0; i < hop_cnt; ++i) {
                    hops[i] = r32(data + off);
                    off += 4;
                }
                rt.add_route(id, dest, prio, ttl, hops, hop_cnt);
                break;
            }
            case 0x01: { // remove route
                if (off + 4 > size) break;
                uint32_t id = r32(data + off); off += 4;
                rt.remove_route(id);
                break;
            }
            case 0x02: { // lookup
                if (off + 2 > size) break;
                uint16_t dest = r16(data + off); off += 2;
                volatile auto res = rt.lookup(dest);
                (void)res;
                break;
            }
            case 0x03: { // update metric
                if (off + 6 > size) break;
                uint32_t id    = r32(data + off); off += 4;
                uint16_t delta = r16(data + off); off += 2;
                rt.update_metric(id, delta);
                break;
            }
            case 0x04: // age routes
                rt.age_routes();
                break;
            default:
                break;
        }
    }
    return 0;
}
