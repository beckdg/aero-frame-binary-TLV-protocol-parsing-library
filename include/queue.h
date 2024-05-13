#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace aeroframe {

constexpr size_t QUEUE_PRIORITY_LEVELS = 4;
constexpr size_t QUEUE_MAX_MESSAGES = 65535;
constexpr uint16_t QUEUE_PROMOTION_THRESHOLD = 100;

enum class QueuePriority : uint8_t {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3,
};

struct QueueMessage {
    uint32_t id;
    uint8_t* data;
    size_t length;
    QueuePriority priority;
    uint32_t enqueue_time;
    uint16_t promotion_count;
};

class PriorityQueue {
public:
    PriorityQueue();
    ~PriorityQueue();

    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    bool enqueue(QueueMessage msg);
    bool dequeue(QueueMessage& msg);

    bool promote(uint32_t msg_id);
    bool demote(uint32_t msg_id);

    size_t count() const { return total_messages_; }
    size_t count_for_priority(QueuePriority p) const;

    void expire_old(uint32_t now_ms, uint32_t max_age_ms);
    void reprioritize();

private:
    QueueMessage* find_message(uint32_t id, size_t& out_level);

    std::vector<QueueMessage> queues_[QUEUE_PRIORITY_LEVELS];
    size_t total_messages_;
    uint64_t total_priority_moves_;
};

} // namespace aeroframe
