#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
namespace aeroframe {
struct ArchiveEntry {
    uint32_t entry_id;
    uint32_t offset;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint8_t codec_id;
    char name[64];
};
class BinaryArchive {
public:
    BinaryArchive() noexcept;
    bool parse_index(const uint8_t* data, size_t len) noexcept;
    bool extract_entry(uint32_t entry_id, const uint8_t* archive, size_t archive_len,
                       std::vector<uint8_t>& out) const noexcept;
    const ArchiveEntry* lookup(uint32_t entry_id) const noexcept;
    size_t entry_count() const noexcept { return entries_.size(); }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::vector<ArchiveEntry> entries_;
};
} // namespace aeroframe
