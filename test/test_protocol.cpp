#include <gtest/gtest.h>
#include <vector>
#include "protocol.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// ProtocolEngine — state machine, capabilities, message handling
// -----------------------------------------------------------------------

TEST(ProtocolTest, InitialState) {
    ProtocolEngine pe;
    EXPECT_EQ(pe.get_state(), ProtocolState::Closed);
}

TEST(ProtocolTest, VersionDeclare) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x01}; // version 1
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::VersionDeclare,
                                  body, sizeof(body), resp));
    EXPECT_EQ(pe.get_state(), ProtocolState::WaitingCapabilities);
}

TEST(ProtocolTest, Heartbeat) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::Heartbeat,
                                  nullptr, 0, resp));
}

TEST(ProtocolTest, DeclareCapability) {
    ProtocolEngine pe;
    EXPECT_TRUE(pe.declare_capability(0, 1, 0x0001));
    EXPECT_TRUE(pe.declare_capability(1, 1, 0x0002));
}

TEST(ProtocolTest, CapabilityDeclareMessage) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x00, 0x01, 0x00, 0x01}; // id=0 v=1 flags=1
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::CapabilityDeclare,
                                  body, sizeof(body), resp));
}

TEST(ProtocolTest, NegotiateCapabilities) {
    ProtocolEngine pe;
    pe.declare_capability(0, 1, 0x0001);
    pe.declare_capability(1, 1, 0x0002);
    EXPECT_TRUE(pe.negotiate_capabilities());
    EXPECT_EQ(pe.get_state(), ProtocolState::Established);
}

TEST(ProtocolTest, DataMessage) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {'H', 'e', 'l', 'l', 'o'};
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::Data,
                                  body, sizeof(body), resp));
}

TEST(ProtocolTest, AckMessage) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x00, 0x01}; // ack seq=1
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::Ack,
                                  body, sizeof(body), resp));
}

TEST(ProtocolTest, NakMessage) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x00, 0x01}; // nak seq=1
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::Nak,
                                  body, sizeof(body), resp));
}

TEST(ProtocolTest, ResetContext) {
    ProtocolEngine pe;
    pe.reset_context();
    EXPECT_EQ(pe.get_state(), ProtocolState::Closed);
}

TEST(ProtocolTest, NextSequence) {
    ProtocolEngine pe;
    uint16_t s1 = pe.next_sequence();
    uint16_t s2 = pe.next_sequence();
    EXPECT_EQ(s2, s1 + 1);
}

TEST(ProtocolTest, InvalidMessageType) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x00};
    // 0x00 is not a valid ProtocolMessageType
    auto bad = static_cast<ProtocolMessageType>(0x00);
    EXPECT_FALSE(pe.handle_message(bad, body, sizeof(body), resp));
}

TEST(ProtocolTest, ControlMessage) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;
    uint8_t body[] = {0x00};
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::Control,
                                  body, sizeof(body), resp));
}

TEST(ProtocolTest, FullHandshake) {
    ProtocolEngine pe;
    std::vector<uint8_t> resp;

    uint8_t ver[] = {0x01};
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::VersionDeclare, ver, sizeof(ver), resp));

    uint8_t cap[] = {0x00, 0x01, 0x00, 0x01};
    EXPECT_TRUE(pe.handle_message(ProtocolMessageType::CapabilityDeclare, cap, sizeof(cap), resp));

    EXPECT_TRUE(pe.negotiate_capabilities());
    EXPECT_EQ(pe.get_state(), ProtocolState::Established);
}
