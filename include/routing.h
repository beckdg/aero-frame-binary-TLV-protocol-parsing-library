#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>

namespace aeroframe {

constexpr size_t ROUTING_TABLE_MAX = 256;
constexpr uint8_t ROUTE_PRIORITY_LOW = 0;
constexpr uint8_t ROUTE_PRIORITY_NORMAL = 1;
constexpr uint8_t ROUTE_PRIORITY_HIGH = 2;
constexpr uint8_t ROUTE_PRIORITY_CRITICAL = 3;

struct RouteEntry {
    uint32_t id;
    uint16_t dest_addr;
    uint8_t priority;
    uint8_t ttl;
    uint16_t metric;
    uint32_t* next_hops;
    size_t hop_count;
    bool active;
    bool local;
};

struct RouteLookupResult {
    const RouteEntry* entry;
    uint16_t next_hop;
    bool found;
};

class RoutingTable {
public:
    RoutingTable();
    ~RoutingTable();

    RoutingTable(const RoutingTable&) = delete;
    RoutingTable& operator=(const RoutingTable&) = delete;

    bool add_route(uint32_t id, uint16_t dest_addr,
                   uint8_t priority, uint8_t ttl,
                   const uint32_t* hops, size_t hop_count);

    bool remove_route(uint32_t id);

    RouteLookupResult lookup(uint16_t dest_addr) const;

    bool update_metric(uint32_t id, uint16_t delta);
    void age_routes();

    size_t route_count() const { return routes_.size(); }

private:
    RouteEntry* find_route_by_id(uint32_t id);

    std::unordered_map<uint32_t, RouteEntry*> routes_;
};

} // namespace aeroframe
