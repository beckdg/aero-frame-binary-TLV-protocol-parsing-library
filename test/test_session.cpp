#include <gtest/gtest.h>
#include "session.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// SessionManager — open, close, find, telemetry append
// -----------------------------------------------------------------------

TEST(SessionTest, OpenAndFind) {
    SessionManager sm;
    EXPECT_TRUE(sm.open_session(42));
    Session* s = sm.find(42);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->id, 42);
    EXPECT_EQ(s->state, SESSION_STATE_OPEN);
}

TEST(SessionTest, OpenDuplicateReturnsFalse) {
    SessionManager sm;
    EXPECT_TRUE(sm.open_session(1));
    EXPECT_FALSE(sm.open_session(1));
}

TEST(SessionTest, FindNonExistentReturnsNull) {
    SessionManager sm;
    EXPECT_EQ(sm.find(999), nullptr);
}

TEST(SessionTest, CloseExisting) {
    SessionManager sm;
    sm.open_session(10);
    EXPECT_TRUE(sm.close_session(10));
    EXPECT_EQ(sm.find(10), nullptr);
}

TEST(SessionTest, CloseNonExistentReturnsFalse) {
    SessionManager sm;
    EXPECT_FALSE(sm.close_session(1));
}

TEST(SessionTest, AppendTelemetry) {
    SessionManager sm;
    sm.open_session(7);
    const uint8_t data[] = {0x01, 0x02, 0x03};
    EXPECT_TRUE(sm.append_telemetry(7, data, sizeof(data)));
    Session* s = sm.find(7);
    ASSERT_NE(s, nullptr);
    ASSERT_NE(s->telemetry_data, nullptr);
    EXPECT_EQ(s->telemetry_data->size(), sizeof(data));
    EXPECT_EQ((*s->telemetry_data)[0], 0x01);
    EXPECT_EQ((*s->telemetry_data)[2], 0x03);
}

TEST(SessionTest, AppendToClosedSessionFails) {
    SessionManager sm;
    const uint8_t data[] = {0xFF};
    EXPECT_FALSE(sm.append_telemetry(99, data, sizeof(data)));
}

TEST(SessionTest, AppendEmptyData) {
    SessionManager sm;
    sm.open_session(3);
    EXPECT_TRUE(sm.append_telemetry(3, nullptr, 0));
}

TEST(SessionTest, MultipleSessions) {
    SessionManager sm;
    EXPECT_TRUE(sm.open_session(1));
    EXPECT_TRUE(sm.open_session(2));
    EXPECT_TRUE(sm.open_session(3));
    EXPECT_NE(sm.find(1), nullptr);
    EXPECT_NE(sm.find(2), nullptr);
    EXPECT_NE(sm.find(3), nullptr);
    EXPECT_TRUE(sm.close_session(2));
    EXPECT_EQ(sm.find(2), nullptr);
    EXPECT_NE(sm.find(1), nullptr);
    EXPECT_NE(sm.find(3), nullptr);
}

TEST(SessionTest, MoveConstructor) {
    SessionManager sm1;
    sm1.open_session(5);
    SessionManager sm2(std::move(sm1));
    EXPECT_NE(sm2.find(5), nullptr);
    EXPECT_EQ(sm1.find(5), nullptr);
}

TEST(SessionTest, MoveAssignment) {
    SessionManager sm1;
    sm1.open_session(5);
    SessionManager sm2;
    sm2.open_session(6);
    sm2 = std::move(sm1);
    EXPECT_NE(sm2.find(5), nullptr);
    EXPECT_EQ(sm2.find(6), nullptr);
}
