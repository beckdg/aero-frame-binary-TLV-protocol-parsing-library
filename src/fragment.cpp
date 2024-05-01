#include "fragment.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

FragmentReassembler::FragmentReassembler()
    : retransmit_head_(nullptr)
    , retransmit_tail_(nullptr)
    , retransmit_count_(0)
{
}

FragmentReassembler::~FragmentReassembler() {
    for (auto& msg_kv : fragments_) {
        for (auto& frag_kv : msg_kv.second) {
            free_fragment(frag_kv.second);
        }
    }
    FragmentAck* ack = retransmit_head_;
    while (ack) {
        FragmentAck* next = ack->next;
        delete ack;
        ack = next;
    }
}

void FragmentReassembler::free_fragment(Fragment* f) {
    if (f) {
        delete[] f->data;
        delete f;
    }
}

Fragment* FragmentReassembler::find_fragment(uint32_t msg_id, uint16_t frag_id) {
    auto msg_it = fragments_.find(msg_id);
    if (msg_it == fragments_.end()) return nullptr;
    auto frag_it = msg_it->second.find(frag_id);
    if (frag_it == msg_it->second.end()) return nullptr;
    return frag_it->second;
}

void FragmentReassembler::remove_from_retransmit_queue(Fragment* f) {
    FragmentAck* ack = retransmit_head_;
    while (ack) {
        if (ack->fragment_ptr == f) {
            ack->fragment_ptr = nullptr;
            break;
        }
        ack = ack->next;
    }
}

bool FragmentReassembler::push_fragment(uint32_t msg_id, uint16_t frag_id,
                                        uint16_t total_frags, uint16_t offset,
                                        const uint8_t* data, uint16_t length)
{
    if (length > MAX_FRAGMENT_PAYLOAD) return false;
    if (frag_id >= MAX_FRAGMENTS_PER_MSG) return false;
    if (total_frags > MAX_FRAGMENTS_PER_MSG) return false;

    Fragment* existing = find_fragment(msg_id, frag_id);

    // BUG B1 (use-after-free):
    // When replacing an existing fragment, the old fragment's data is freed
    // but the retransmit queue may still hold a pointer to it.
    // remove_from_retransmit_queue should be called here but is NOT.
    if (existing) {
        auto msg_it = fragments_.find(msg_id);
        msg_it->second.erase(frag_id);
        free_fragment(existing);
    }

    uint8_t* data_copy = new uint8_t[length];
    std::memcpy(data_copy, data, length);

    Fragment* frag = new Fragment();
    frag->msg_id = msg_id;
    frag->fragment_id = frag_id;
    frag->total_fragments = total_frags;
    frag->offset = offset;
    frag->length = length;
    frag->data = data_copy;
    frag->acked = false;

    fragments_[msg_id][frag_id] = frag;
    return true;
}

bool FragmentReassembler::request_retransmit(uint32_t msg_id, uint16_t frag_id) {
    Fragment* frag = find_fragment(msg_id, frag_id);
    if (!frag) return false;

    FragmentAck* ack = new FragmentAck();
    ack->msg_id = msg_id;
    ack->fragment_id = frag_id;
    ack->timestamp = 0;
    ack->fragment_ptr = frag; // stores RAW pointer to fragment
    ack->next = nullptr;

    if (retransmit_tail_) {
        retransmit_tail_->next = ack;
        retransmit_tail_ = ack;
    } else {
        retransmit_head_ = ack;
        retransmit_tail_ = ack;
    }
    retransmit_count_++;
    return true;
}

bool FragmentReassembler::process_retransmit(uint32_t msg_id, uint16_t frag_id) {
    FragmentAck* prev = nullptr;
    FragmentAck* ack = retransmit_head_;

    while (ack) {
        if (ack->msg_id == msg_id && ack->fragment_id == frag_id) {
            if (ack->fragment_ptr) {
                Fragment* frag = ack->fragment_ptr;
                // Uses fragment_ptr — this is the UAF if the fragment
                // has been freed and replaced since request_retransmit.
                if (frag->data && frag->length > 0) {
                    push_fragment(frag->msg_id, frag->fragment_id,
                                  frag->total_fragments, frag->offset,
                                  frag->data, frag->length);
                }
            }

            // Remove this ack entry
            if (prev) prev->next = ack->next;
            else retransmit_head_ = ack->next;
            if (ack == retransmit_tail_) retransmit_tail_ = prev;
            delete ack;
            retransmit_count_--;
            return true;
        }
        prev = ack;
        ack = ack->next;
    }
    return false;
}

bool FragmentReassembler::is_message_complete(uint32_t msg_id) const {
    auto msg_it = fragments_.find(msg_id);
    if (msg_it == fragments_.end()) return false;

    if (msg_it->second.empty()) return false;

    uint16_t total = msg_it->second.begin()->second->total_fragments;
    for (uint16_t i = 0; i < total; ++i) {
        if (msg_it->second.find(i) == msg_it->second.end()) return false;
    }
    return true;
}

bool FragmentReassembler::assemble_message(uint32_t msg_id,
                                           std::vector<uint8_t>& out) const
{
    auto msg_it = fragments_.find(msg_id);
    if (msg_it == fragments_.end()) return false;
    if (!is_message_complete(msg_id)) return false;

    size_t total_size = 0;
    for (auto& kv : msg_it->second) {
        total_size += kv.second->length;
    }

    out.clear();
    out.reserve(total_size);
    for (auto& kv : msg_it->second) {
        Fragment* f = kv.second;
        out.insert(out.end(), f->data, f->data + f->length);
    }
    return true;
}

void FragmentReassembler::discard_message(uint32_t msg_id) {
    auto msg_it = fragments_.find(msg_id);
    if (msg_it == fragments_.end()) return;

    for (auto& kv : msg_it->second) {
        Fragment* f = kv.second;
        remove_from_retransmit_queue(f);
        free_fragment(f);
    }
    fragments_.erase(msg_it);
}

void FragmentReassembler::expire_retransmit_queue(uint32_t now_ms) {
    (void)now_ms;
    FragmentAck* ack = retransmit_head_;
    while (ack) {
        if (ack->fragment_ptr) {
            // Reads fragment_ptr — potential UAF if the fragment
            // was freed and the pointer was not nulled.
            (void)ack->fragment_ptr->msg_id;
        }
        ack = ack->next;
    }
}

} // namespace aeroframe
