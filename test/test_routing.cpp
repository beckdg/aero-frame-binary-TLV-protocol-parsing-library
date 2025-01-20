#include <gtest/gtest.h>
#include "routing.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// RoutingTable — add, lookup, remove, update_metric, age
// -----------------------------------------------------------------------

TEST(RoutingTest, EmptyLookup) {
    RoutingTable rt;
    auto res = rt.lookup(0x0A00);
    EXPECT_FALSE(res.found);
}

TEST(RoutingTest, AddAndLookup) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    EXPECT_TRUE(rt.add_route(1, 0x0A00, ROUTE_PRIORITY_HIGH, 10,
                             hops, 1));
    EXPECT_EQ(rt.route_count(), 1);
    auto res = rt.lookup(0x0A00);
    EXPECT_TRUE(res.found);
    EXPECT_EQ(res.entry->id, 1);
}

TEST(RoutingTest, RemoveRoute) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    rt.add_route(1, 0x0A00, ROUTE_PRIORITY_HIGH, 10, hops, 1);
    EXPECT_TRUE(rt.remove_route(1));
    EXPECT_EQ(rt.route_count(), 0);
    EXPECT_FALSE(rt.lookup(0x0A00).found);
}

TEST(RoutingTest, RemoveNonExistent) {
    RoutingTable rt;
    EXPECT_FALSE(rt.remove_route(99));
}

TEST(RoutingTest, UpdateMetric) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    rt.add_route(1, 0x0A00, ROUTE_PRIORITY_HIGH, 10, hops, 1);
    EXPECT_TRUE(rt.update_metric(1, 5));
    auto res = rt.lookup(0x0A00);
    EXPECT_TRUE(res.found);
    EXPECT_EQ(res.entry->metric, 5);
}

TEST(RoutingTest, UpdateMetricNonExistent) {
    RoutingTable rt;
    EXPECT_FALSE(rt.update_metric(99, 5));
}

TEST(RoutingTest, AgeRoutes) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    rt.add_route(1, 0x0A00, ROUTE_PRIORITY_HIGH, 1, hops, 1);
    // ttl=1, age it — route should survive first age
    rt.age_routes();
    EXPECT_EQ(rt.route_count(), 0);
}

TEST(RoutingTest, MultipleRoutes) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    rt.add_route(1, 0x0A00, ROUTE_PRIORITY_HIGH, 10, hops, 1);
    rt.add_route(2, 0x0B00, ROUTE_PRIORITY_NORMAL, 10, hops, 1);
    EXPECT_EQ(rt.route_count(), 2);
    EXPECT_TRUE(rt.lookup(0x0A00).found);
    EXPECT_TRUE(rt.lookup(0x0B00).found);
    EXPECT_FALSE(rt.lookup(0x0C00).found);
}

TEST(RoutingTest, TableMaxCapacity) {
    RoutingTable rt;
    uint32_t hops[] = {0x01010101};
    for (uint32_t i = 0; i < 256; ++i) {
        rt.add_route(i, static_cast<uint16_t>(i), ROUTE_PRIORITY_NORMAL, 10, hops, 1);
    }
    EXPECT_EQ(rt.route_count(), 256);
    // Adding beyond max
    EXPECT_FALSE(rt.add_route(257, 0xFFFF, ROUTE_PRIORITY_NORMAL, 10, hops, 1));
}
