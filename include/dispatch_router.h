#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
namespace aeroframe {
struct RouteEntry {
    uint16_t dest_id;
    uint8_t handler_id;
    uint8_t priority;
    uint16_t max_hops;
};
class DispatchRouter {
public:
    DispatchRouter() noexcept;
    bool add_route(const RouteEntry& entry) noexcept;
    bool load_routes_binary(const uint8_t* data, size_t len) noexcept;
    bool dispatch(uint16_t dest_id, const uint8_t* payload, size_t len,
                  uint8_t& handler_id) noexcept;
    void clear() noexcept;
    size_t route_count() const noexcept { return routes_.size(); }
private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    std::unordered_map<uint16_t, RouteEntry> routes_;
    uint32_t dispatch_count_{0};
};
} // namespace aeroframe
