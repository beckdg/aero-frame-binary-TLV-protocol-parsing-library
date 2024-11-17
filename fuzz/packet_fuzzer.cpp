#include <cstdint>
#include <cstddef>
#include <vector>

#include "aeroframe.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Wrap the raw fuzz bytes into a vector and feed it to the full parser.
    // Every code path — magic validation, length checks, boundary enforcement,
    // TLV extraction, session/compression dispatch, and CRC verification — is
    // exercised through this single entry point.
    const std::vector<uint8_t> buf(data, data + size);
    aeroframe::AeroParser parser;
    const auto result = parser.parse(buf);

    // Consume the result so the optimizer cannot eliminate the call.
    if (result.error != aeroframe::ErrorCode::None) {
        // Error paths are interesting; keep the value live.
        (void)result.error;
    }
    for (const auto& tlv : result.tlvs) {
        (void)tlv.type;
        (void)tlv.length;
        (void)tlv.value.size();
    }
    (void)result.crc;

    return 0;
}
