#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
namespace aeroframe {
struct BufferedRecord {
    uint32_t sequence;
    uint32_t timestamp_ms;
    std::vector<uint8_t> payload;
};
class RecordBuffer {
public:
    explicit RecordBuffer(size_t capacity) noexcept;
    bool push(const uint8_t* data, size_t len, uint32_t ts) noexcept;
    bool pop(BufferedRecord& out) noexcept;
    bool peek(size_t index, BufferedRecord& out) const noexcept;
    void compact() noexcept;
    size_t size() const noexcept { return records_.size(); }
    size_t capacity() const noexcept { return capacity_; }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    size_t capacity_;
    size_t head_{0};
    std::vector<BufferedRecord> records_;
    uint32_t next_seq_{0};
};
} // namespace aeroframe
