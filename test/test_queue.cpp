#include <gtest/gtest.h>
#include "queue.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// PriorityQueue — enqueue, dequeue, promote, demote, expiry
// -----------------------------------------------------------------------

TEST(QueueTest, EmptyQueue) {
    PriorityQueue pq;
    EXPECT_EQ(pq.count(), 0);
    QueueMessage msg;
    EXPECT_FALSE(pq.dequeue(msg));
}

TEST(QueueTest, EnqueueAndDequeue) {
    PriorityQueue pq;
    uint8_t payload[] = "Hello";
    QueueMessage msg;
    msg.id = 1;
    msg.data = payload;
    msg.length = sizeof(payload);
    msg.priority = QueuePriority::Normal;
    msg.enqueue_time = 100;
    msg.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(msg));
    EXPECT_EQ(pq.count(), 1);

    QueueMessage out;
    EXPECT_TRUE(pq.dequeue(out));
    EXPECT_EQ(out.id, 1);
    EXPECT_EQ(pq.count(), 0);
}

TEST(QueueTest, PriorityOrder) {
    PriorityQueue pq;
    uint8_t d1[] = "low", d2[] = "high";
    QueueMessage low;
    low.id = 1; low.data = d1; low.length = sizeof(d1);
    low.priority = QueuePriority::Low; low.enqueue_time = 0; low.promotion_count = 0;
    QueueMessage high;
    high.id = 2; high.data = d2; high.length = sizeof(d2);
    high.priority = QueuePriority::High; high.enqueue_time = 0; high.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(low));
    EXPECT_TRUE(pq.enqueue(high));
    EXPECT_EQ(pq.count(), 2);

    QueueMessage out;
    EXPECT_TRUE(pq.dequeue(out));
    EXPECT_EQ(out.id, 2); // high priority dequeued first
    EXPECT_TRUE(pq.dequeue(out));
    EXPECT_EQ(out.id, 1);
}

TEST(QueueTest, Promote) {
    PriorityQueue pq;
    uint8_t d[] = "data";
    QueueMessage msg;
    msg.id = 1; msg.data = d; msg.length = sizeof(d);
    msg.priority = QueuePriority::Normal; msg.enqueue_time = 0; msg.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(msg));
    EXPECT_TRUE(pq.promote(1));
    EXPECT_EQ(pq.count_for_priority(QueuePriority::High), 1);
}

TEST(QueueTest, Demote) {
    PriorityQueue pq;
    uint8_t d[] = "data";
    QueueMessage msg;
    msg.id = 1; msg.data = d; msg.length = sizeof(d);
    msg.priority = QueuePriority::High; msg.enqueue_time = 0; msg.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(msg));
    EXPECT_TRUE(pq.demote(1));
    EXPECT_EQ(pq.count_for_priority(QueuePriority::Normal), 1);
}

TEST(QueueTest, PromoteNonExistent) {
    PriorityQueue pq;
    EXPECT_FALSE(pq.promote(99));
}

TEST(QueueTest, DemoteNonExistent) {
    PriorityQueue pq;
    EXPECT_FALSE(pq.demote(99));
}

TEST(QueueTest, CountByPriority) {
    PriorityQueue pq;
    uint8_t d[] = "data";
    for (int i = 0; i < 3; ++i) {
        QueueMessage msg;
        msg.id = i; msg.data = d; msg.length = sizeof(d);
        msg.priority = static_cast<QueuePriority>(i);
        msg.enqueue_time = 0; msg.promotion_count = 0;
        EXPECT_TRUE(pq.enqueue(msg));
    }
    EXPECT_EQ(pq.count_for_priority(QueuePriority::Low), 1);
    EXPECT_EQ(pq.count_for_priority(QueuePriority::Normal), 1);
    EXPECT_EQ(pq.count_for_priority(QueuePriority::High), 1);
    EXPECT_EQ(pq.count(), 3);
}

TEST(QueueTest, ExpireOld) {
    PriorityQueue pq;
    uint8_t d[] = "old";
    QueueMessage msg;
    msg.id = 1; msg.data = d; msg.length = sizeof(d);
    msg.priority = QueuePriority::Normal; msg.enqueue_time = 0; msg.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(msg));
    pq.expire_old(5000, 100); // expire messages older than 100ms
    EXPECT_EQ(pq.count(), 0);
}

TEST(QueueTest, Reprioritize) {
    PriorityQueue pq;
    uint8_t d[] = "data";
    QueueMessage msg;
    msg.id = 1; msg.data = d; msg.length = sizeof(d);
    msg.priority = QueuePriority::Low; msg.enqueue_time = 0; msg.promotion_count = 0;
    EXPECT_TRUE(pq.enqueue(msg));
    pq.reprioritize(); // Should not crash
}

TEST(QueueTest, DequeueFromEmptyReturnsFalse) {
    PriorityQueue pq;
    QueueMessage msg;
    EXPECT_FALSE(pq.dequeue(msg));
}
