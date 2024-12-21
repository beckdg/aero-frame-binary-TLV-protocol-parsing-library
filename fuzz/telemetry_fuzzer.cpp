#include <cstdint>
#include <cstddef>
#include <vector>

#include "telemetry_decoder.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    aeroframe::TelemetryDecoder decoder;
    std::vector<aeroframe::DecodedTelemetry> batch;
    decoder.decode_batch(data, size, batch);
    auto single = decoder.decode_record(data, size);
    if (single.valid) {
        (void)aeroframe::TelemetryDecoder::record_type_name(single.type);
    }
    for (const auto& rec : batch) {
        (void)rec.type;
        (void)aeroframe::TelemetryDecoder::minimum_record_size(rec.type);
    }
    return 0;
}
