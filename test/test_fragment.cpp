#include <gtest/gtest.h>
#include <vector>
#include "fragment.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// FragmentReassembler — push, assemble, retransmit, expiry
// -----------------------------------------------------------------------

TEST(FragmentTest, PushSingleComplete) {
    FragmentReassembler fr;
    const uint8_t data[] = "Hello";
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, data, sizeof(data)));
    EXPECT_TRUE(fr.is_message_complete(1));
    std::vector<uint8_t> out;
    EXPECT_TRUE(fr.assemble_message(1, out));
    ASSERT_EQ(out.size(), sizeof(data));
    EXPECT_EQ(std::memcmp(out.data(), data, sizeof(data)), 0);
}

TEST(FragmentTest, TwoFragments) {
    FragmentReassembler fr;
    const uint8_t f0[] = "Hello ";
    const uint8_t f1[] = "World";
    EXPECT_TRUE(fr.push_fragment(1, 0, 2, 0, f0, sizeof(f0)));
    EXPECT_FALSE(fr.is_message_complete(1));
    EXPECT_TRUE(fr.push_fragment(1, 1, 2, sizeof(f0), f1, sizeof(f1)));
    EXPECT_TRUE(fr.is_message_complete(1));
    std::vector<uint8_t> out;
    EXPECT_TRUE(fr.assemble_message(1, out));
    ASSERT_EQ(out.size(), sizeof(f0) + sizeof(f1));
    EXPECT_EQ(std::memcmp(out.data(), "Hello World", out.size()), 0);
}

TEST(FragmentTest, DuplicateFragmentIgnored) {
    FragmentReassembler fr;
    const uint8_t data[] = "Dup";
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, data, sizeof(data)));
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, data, sizeof(data)));
    EXPECT_TRUE(fr.is_message_complete(1));
}

TEST(FragmentTest, AssembleIncompleteFails) {
    FragmentReassembler fr;
    const uint8_t data[] = "Part1";
    EXPECT_TRUE(fr.push_fragment(1, 0, 2, 0, data, sizeof(data)));
    EXPECT_FALSE(fr.is_message_complete(1));
    std::vector<uint8_t> out;
    EXPECT_FALSE(fr.assemble_message(1, out));
}

TEST(FragmentTest, DiscardMessage) {
    FragmentReassembler fr;
    const uint8_t data[] = "Hello";
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, data, sizeof(data)));
    EXPECT_TRUE(fr.is_message_complete(1));
    fr.discard_message(1);
    EXPECT_FALSE(fr.is_message_complete(1));
}

TEST(FragmentTest, RequestRetransmit) {
    FragmentReassembler fr;
    EXPECT_TRUE(fr.request_retransmit(1, 0));
    // Process the retransmit
    const uint8_t data[] = "Retransmitted";
    EXPECT_TRUE(fr.process_retransmit(1, 0));
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, data, sizeof(data)));
    EXPECT_TRUE(fr.is_message_complete(1));
    std::vector<uint8_t> out;
    EXPECT_TRUE(fr.assemble_message(1, out));
    EXPECT_EQ(out.size(), sizeof(data));
}

TEST(FragmentTest, RequestRetransmitNonexistent) {
    FragmentReassembler fr;
    EXPECT_TRUE(fr.request_retransmit(99, 0));
    EXPECT_FALSE(fr.process_retransmit(99, 0));
}

TEST(FragmentTest, ExpireRetransmitQueue) {
    FragmentReassembler fr;
    fr.request_retransmit(1, 0);
    // Expire with timestamp 0 — items added with timestamp 0 should be expired
    fr.expire_retransmit_queue(0);
    // No crash expected
}

TEST(FragmentTest, MultipleMessagesIndependent) {
    FragmentReassembler fr;
    const uint8_t msg1[] = "MsgOne";
    const uint8_t msg2[] = "MsgTwo";
    EXPECT_TRUE(fr.push_fragment(1, 0, 1, 0, msg1, sizeof(msg1)));
    EXPECT_TRUE(fr.push_fragment(2, 0, 1, 0, msg2, sizeof(msg2)));
    EXPECT_TRUE(fr.is_message_complete(1));
    EXPECT_TRUE(fr.is_message_complete(2));
    std::vector<uint8_t> out;
    EXPECT_TRUE(fr.assemble_message(1, out));
    EXPECT_EQ(out.size(), sizeof(msg1));
    out.clear();
    EXPECT_TRUE(fr.assemble_message(2, out));
    EXPECT_EQ(out.size(), sizeof(msg2));
}
