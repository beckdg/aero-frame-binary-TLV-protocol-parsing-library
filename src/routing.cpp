#include "routing.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

RoutingTable::RoutingTable() {}

RoutingTable::~RoutingTable() {
    for (auto& kv : routes_) {
        delete[] kv.second->next_hops;
        delete kv.second;
    }
    routes_.clear();
}

RouteEntry* RoutingTable::find_route_by_id(uint32_t id) {
    auto it = routes_.find(id);
    return (it != routes_.end()) ? it->second : nullptr;
}

bool RoutingTable::add_route(uint32_t id, uint16_t dest_addr,
                              uint8_t priority, uint8_t ttl,
                              const uint32_t* hops, size_t hop_count)
{
    if (routes_.size() >= ROUTING_TABLE_MAX) return false;
    if (routes_.count(id) != 0) return false;
    if (priority > ROUTE_PRIORITY_CRITICAL) return false;

    uint32_t* hop_copy = new uint32_t[hop_count];
    std::memcpy(hop_copy, hops, hop_count * sizeof(uint32_t));

    RouteEntry* entry = new RouteEntry();
    entry->id = id;
    entry->dest_addr = dest_addr;
    entry->priority = priority;
    entry->ttl = ttl;
    entry->metric = 0;
    entry->next_hops = hop_copy;
    entry->hop_count = hop_count;
    entry->active = true;
    entry->local = false;

    routes_[id] = entry;
    return true;
}

bool RoutingTable::remove_route(uint32_t id) {
    auto it = routes_.find(id);
    if (it == routes_.end()) return false;

    RouteEntry* entry = it->second;
    delete[] entry->next_hops;
    delete entry;
    routes_.erase(it);
    return true;
}

RouteLookupResult RoutingTable::lookup(uint16_t dest_addr) const {
    RouteLookupResult result;
    result.found = false;
    result.entry = nullptr;
    result.next_hop = 0;

    const RouteEntry* best = nullptr;
    uint16_t best_metric = UINT16_MAX;

    for (const auto& kv : routes_) {
        const RouteEntry* e = kv.second;
        if (!e->active) continue;
        if (e->dest_addr == dest_addr) {
            if (e->metric < best_metric) {
                best_metric = e->metric;
                best = e;
            }
        }
    }

    if (best) {
        result.found = true;
        result.entry = best;
        if (best->hop_count > 0) {
            result.next_hop = static_cast<uint16_t>(best->next_hops[0] & 0xFFFF);
        }
    }
    return result;
}

bool RoutingTable::update_metric(uint32_t id, uint16_t delta) {
    RouteEntry* entry = find_route_by_id(id);
    if (!entry) return false;
    entry->metric += delta;
    return true;
}

void RoutingTable::age_routes() {
    for (auto& kv : routes_) {
        RouteEntry* e = kv.second;
        if (e->ttl > 0) {
            e->ttl--;
            if (e->ttl == 0) {
                e->active = false;
            }
        }
    }
}

} // namespace aeroframe
