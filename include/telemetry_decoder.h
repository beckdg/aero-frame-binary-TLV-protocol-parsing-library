#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string_view>
#include <array>

namespace aeroframe {

enum class TelemetryRecordType : uint8_t {
    Unknown = 0,
    GpsFix = 1,
    ImuSample = 2,
    Attitude = 3,
    Barometric = 4,
    Temperature = 5,
    PowerRail = 6,
    CustomPayload = 7,
};

struct GpsFixRecord {
    uint32_t timestamp_ms;
    int32_t latitude_e7;
    int32_t longitude_e7;
    int32_t altitude_mm;
    uint16_t hdop_centi;
    uint8_t fix_quality;
    uint8_t satellites;
};

struct ImuSampleRecord {
    uint32_t timestamp_ms;
    int16_t accel_x_mg;
    int16_t accel_y_mg;
    int16_t accel_z_mg;
    int16_t gyro_x_mdps;
    int16_t gyro_y_mdps;
    int16_t gyro_z_mdps;
    int16_t mag_x_ut;
    int16_t mag_y_ut;
    int16_t mag_z_ut;
};

struct AttitudeRecord {
    uint32_t timestamp_ms;
    int16_t roll_cdeg;
    int16_t pitch_cdeg;
    int16_t yaw_cdeg;
    uint16_t quaternion_w;
    uint16_t quaternion_x;
    uint16_t quaternion_y;
    uint16_t quaternion_z;
};

struct BarometricRecord {
    uint32_t timestamp_ms;
    uint32_t pressure_pa;
    int16_t temperature_centi;
    uint16_t humidity_centi;
};

struct TemperatureRecord {
    uint32_t timestamp_ms;
    uint8_t sensor_id;
    int16_t value_centi;
    uint8_t flags;
};

struct PowerRailRecord {
    uint32_t timestamp_ms;
    uint8_t rail_id;
    uint16_t voltage_mv;
    int16_t current_ma;
    uint8_t status;
};

struct CustomPayloadRecord {
    uint32_t timestamp_ms;
    uint16_t vendor_id;
    uint16_t payload_type;
    std::vector<uint8_t> payload;
};

struct DecodedTelemetry {
    TelemetryRecordType type{TelemetryRecordType::Unknown};
    GpsFixRecord gps{};
    ImuSampleRecord imu{};
    AttitudeRecord attitude{};
    BarometricRecord baro{};
    TemperatureRecord temp{};
    PowerRailRecord power{};
    CustomPayloadRecord custom{};
    bool valid{false};
};

class TelemetryDecoder {
public:
    TelemetryDecoder() noexcept;

    DecodedTelemetry decode_record(const uint8_t* data, size_t len) noexcept;
    bool decode_batch(const uint8_t* data, size_t len,
                      std::vector<DecodedTelemetry>& out) noexcept;

    static std::string_view record_type_name(TelemetryRecordType t) noexcept;
    static size_t minimum_record_size(TelemetryRecordType t) noexcept;

private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static int16_t read_i16(const uint8_t* p) noexcept;
    static int32_t read_i32(const uint8_t* p) noexcept;

    DecodedTelemetry decode_gps(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_imu(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_attitude(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_baro(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_temp(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_power(const uint8_t* data, size_t len) noexcept;
    DecodedTelemetry decode_custom(const uint8_t* data, size_t len) noexcept;

    std::array<uint32_t, 256> decode_stats_{};
    uint32_t batch_count_{0};
};

} // namespace aeroframe
