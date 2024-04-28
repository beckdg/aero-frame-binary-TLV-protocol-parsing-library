#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <map>

namespace aeroframe {

constexpr size_t MAX_FRAGMENT_PAYLOAD = 1024;
constexpr size_t MAX_FRAGMENTS_PER_MSG = 64;
constexpr uint32_t FRAGMENT_TIMEOUT_MS = 5000;

struct Fragment {
    uint32_t msg_id;
    uint16_t fragment_id;
    uint16_t total_fragments;
    uint16_t offset;
    uint16_t length;
    uint8_t* data;
    bool acked;
};

struct FragmentAck {
    uint32_t msg_id;
    uint16_t fragment_id;
    uint32_t timestamp;
    Fragment* fragment_ptr;
    FragmentAck* next;
};

class FragmentReassembler {
public:
    FragmentReassembler();
    ~FragmentReassembler();

    FragmentReassembler(const FragmentReassembler&) = delete;
    FragmentReassembler& operator=(const FragmentReassembler&) = delete;

    bool push_fragment(uint32_t msg_id, uint16_t frag_id,
                       uint16_t total_frags, uint16_t offset,
                       const uint8_t* data, uint16_t length);

    bool request_retransmit(uint32_t msg_id, uint16_t frag_id);
    bool process_retransmit(uint32_t msg_id, uint16_t frag_id);

    bool is_message_complete(uint32_t msg_id) const;
    bool assemble_message(uint32_t msg_id, std::vector<uint8_t>& out) const;

    void discard_message(uint32_t msg_id);
    void expire_retransmit_queue(uint32_t now_ms);

private:
    Fragment* find_fragment(uint32_t msg_id, uint16_t frag_id);
    void free_fragment(Fragment* f);
    void remove_from_retransmit_queue(Fragment* f);

    std::map<uint32_t, std::map<uint16_t, Fragment*>> fragments_;
    FragmentAck* retransmit_head_;
    FragmentAck* retransmit_tail_;
    uint32_t retransmit_count_;
};

} // namespace aeroframe
