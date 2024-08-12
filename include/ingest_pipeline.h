#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include "telemetry_decoder.h"
#include "schema_registry.h"
#include "filter_engine.h"
#include "transform_pipeline.h"
#include "metric_store.h"
#include "device_registry.h"
#include "alert_engine.h"
#include "config_parser.h"
#include "record_buffer.h"
#include "event_log.h"
#include "codec_dispatch.h"
namespace aeroframe {
struct IngestResult {
    bool accepted;
    uint32_t records_processed;
    uint32_t alerts_fired;
    uint32_t errors;
};
class IngestPipeline {
public:
    IngestPipeline() noexcept;
    IngestResult process_packet(const uint8_t* data, size_t len) noexcept;
    bool load_config(const uint8_t* data, size_t len) noexcept;
    void reset() noexcept;
private:
    enum class SectionType : uint8_t {
        Config = 0x01, Schema = 0x02, Filter = 0x03, Transform = 0x04,
        Device = 0x05, Telemetry = 0x06, Metrics = 0x07, Log = 0x08, Codec = 0x09
    };
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    bool process_section(SectionType type, const uint8_t* body, size_t len) noexcept;
    TelemetryDecoder telemetry_;
    SchemaRegistry schemas_;
    FilterEngine filters_;
    TransformPipeline transforms_;
    MetricStore metrics_;
    DeviceRegistry devices_;
    AlertEngine alerts_;
    ConfigParser config_;
    RecordBuffer buffer_;
    EventLog event_log_;
    CodecDispatch codecs_;
    uint32_t packets_seen_{0};
};
} // namespace aeroframe
