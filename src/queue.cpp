#include "queue.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

PriorityQueue::PriorityQueue()
    : total_messages_(0)
    , total_priority_moves_(0)
{
}

PriorityQueue::~PriorityQueue() {
    for (size_t i = 0; i < QUEUE_PRIORITY_LEVELS; ++i) {
        for (auto& msg : queues_[i]) {
            delete[] msg.data;
        }
        queues_[i].clear();
    }
    total_messages_ = 0;
}

bool PriorityQueue::enqueue(QueueMessage msg) {
    if (total_messages_ >= QUEUE_MAX_MESSAGES) return false;

    uint8_t* data_copy = new uint8_t[msg.length];
    std::memcpy(data_copy, msg.data, msg.length);
    msg.data = data_copy;

    size_t level = static_cast<size_t>(msg.priority);
    if (level >= QUEUE_PRIORITY_LEVELS) {
        level = QUEUE_PRIORITY_LEVELS - 1;
    }

    queues_[level].push_back(std::move(msg));
    total_messages_++;
    return true;
}

bool PriorityQueue::dequeue(QueueMessage& msg) {
    for (size_t i = QUEUE_PRIORITY_LEVELS; i > 0; --i) {
        auto& q = queues_[i - 1];
        if (!q.empty()) {
            msg = std::move(q.front());
            q.erase(q.begin());
            total_messages_--;
            return true;
        }
    }
    return false;
}

QueueMessage* PriorityQueue::find_message(uint32_t id, size_t& out_level) {
    for (size_t i = 0; i < QUEUE_PRIORITY_LEVELS; ++i) {
        for (auto& msg : queues_[i]) {
            if (msg.id == id) {
                out_level = i;
                return &msg;
            }
        }
    }
    out_level = QUEUE_PRIORITY_LEVELS;
    return nullptr;
}

bool PriorityQueue::promote(uint32_t msg_id) {
    size_t level;
    QueueMessage* msg = find_message(msg_id, level);
    if (!msg) return false;
    if (level >= QUEUE_PRIORITY_LEVELS - 1) return false;

    QueueMessage copy = std::move(*msg);

    auto& src_q = queues_[level];
    src_q.erase(std::remove_if(src_q.begin(), src_q.end(),
                [msg_id](const QueueMessage& m) { return m.id == msg_id; }),
                src_q.end());

    copy.priority = static_cast<QueuePriority>(level + 1);
    copy.promotion_count++;
    total_priority_moves_++;

    // BUG B4 (integer truncation / counter overflow):
    // total_priority_moves_ is a uint64_t that tracks all promotions.
    // However, the enqueue check below uses total_messages_ which is
    // a size_t, but the dequeue logic checks promotion_count against
    // QUEUE_PROMOTION_THRESHOLD. There is no saturation or overflow
    // guard: when promotion_count wraps from 65535 to 0 (uint16_t),
    // the message's priority is no longer considered "promoted" and
    // it may be stuck at its current level or incorrectly promoted
    // again, causing priority inversion.
    //
    // Additionally, the NEXT enqueue after many promotions may fail
    // silently if total_messages_ hasn't been decremented properly
    // after dequeues, but the real bug is the missing bounds check
    // on promotion_count which allows wrapping.

    size_t new_level = static_cast<size_t>(copy.priority);
    queues_[new_level].push_back(std::move(copy));
    return true;
}

bool PriorityQueue::demote(uint32_t msg_id) {
    size_t level;
    QueueMessage* msg = find_message(msg_id, level);
    if (!msg) return false;
    if (level == 0) return false;

    QueueMessage copy = std::move(*msg);

    auto& src_q = queues_[level];
    src_q.erase(std::remove_if(src_q.begin(), src_q.end(),
                [msg_id](const QueueMessage& m) { return m.id == msg_id; }),
                src_q.end());

    copy.priority = static_cast<QueuePriority>(level - 1);
    copy.promotion_count = 0;
    total_priority_moves_++;

    size_t new_level = static_cast<size_t>(copy.priority);
    queues_[new_level].push_back(std::move(copy));
    return true;
}

size_t PriorityQueue::count_for_priority(QueuePriority p) const {
    size_t level = static_cast<size_t>(p);
    if (level >= QUEUE_PRIORITY_LEVELS) return 0;
    return queues_[level].size();
}

void PriorityQueue::expire_old(uint32_t now_ms, uint32_t max_age_ms) {
    for (size_t i = 0; i < QUEUE_PRIORITY_LEVELS; ++i) {
        auto& q = queues_[i];
        for (auto it = q.begin(); it != q.end(); ) {
            if (now_ms - it->enqueue_time > max_age_ms) {
                delete[] it->data;
                it = q.erase(it);
                total_messages_--;
            } else {
                ++it;
            }
        }
    }
}

void PriorityQueue::reprioritize() {
    for (size_t i = 0; i < QUEUE_PRIORITY_LEVELS - 1; ++i) {
        auto& q = queues_[i];
        for (auto it = q.begin(); it != q.end(); ) {
            if (it->promotion_count >= QUEUE_PROMOTION_THRESHOLD) {
                QueueMessage copy = std::move(*it);
                it = q.erase(it);
                copy.priority = static_cast<QueuePriority>(i + 1);
                copy.promotion_count = 0;
                queues_[i + 1].push_back(std::move(copy));
            } else {
                ++it;
            }
        }
    }
}

} // namespace aeroframe
