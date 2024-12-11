#include <cstdint>
#include <cstddef>
#include <cstring>

#include "queue.h"

// Fuzz the priority queue with enqueue/dequeue/promote/demote sequences.
//
// Command byte:
//   0x00 – enqueue
//           4B id  2B len  [payload]
//   0x01 – dequeue
//   0x02 – promote
//           4B id
//   0x03 – demote
//           4B id
//   0x04 – reprioritize
//   0x05 – expire old
//           4B now_ms  4B max_age_ms

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
    aeroframe::PriorityQueue pq;
    size_t off = 0;

    while (off < size) {
        uint8_t cmd = data[off++];

        switch (cmd) {
            case 0x00: { // enqueue
                if (off + 6 > size) break;
                uint32_t id  = r32(data + off); off += 4;
                uint16_t len = r16(data + off); off += 2;
                if (off + len > size) break;

                aeroframe::QueueMessage msg;
                msg.id = id;
                msg.data = const_cast<uint8_t*>(data + off);
                msg.length = len;
                msg.priority = static_cast<aeroframe::QueuePriority>(
                    (id % 4));
                msg.enqueue_time = static_cast<uint32_t>(id);
                msg.promotion_count = 0;
                pq.enqueue(msg);
                off += len;
                break;
            }
            case 0x01: { // dequeue
                aeroframe::QueueMessage msg;
                pq.dequeue(msg);
                break;
            }
            case 0x02: { // promote
                if (off + 4 > size) break;
                uint32_t id = r32(data + off); off += 4;
                pq.promote(id);
                break;
            }
            case 0x03: { // demote
                if (off + 4 > size) break;
                uint32_t id = r32(data + off); off += 4;
                pq.demote(id);
                break;
            }
            case 0x04: // reprioritize
                pq.reprioritize();
                break;
            case 0x05: { // expire old
                if (off + 8 > size) break;
                uint32_t now = r32(data + off); off += 4;
                uint32_t max_age = r32(data + off); off += 4;
                pq.expire_old(now, max_age);
                break;
            }
            default:
                break;
        }
    }
    return 0;
}
