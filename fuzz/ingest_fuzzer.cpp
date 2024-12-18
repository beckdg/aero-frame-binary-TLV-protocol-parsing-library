#include <cstdint>
#include <cstddef>

#include "ingest_pipeline.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    aeroframe::IngestPipeline pipeline;
    auto result = pipeline.process_packet(data, size);
    (void)result.accepted;
    (void)result.records_processed;
    (void)result.errors;
    pipeline.load_config(data, size);
    return 0;
}
