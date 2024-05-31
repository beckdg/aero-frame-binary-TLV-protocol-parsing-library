#include "telemetry_decoder.h"
#include <cstring>

namespace aeroframe {

TelemetryDecoder::TelemetryDecoder() noexcept : batch_count_(0) {
    decode_stats_.fill(0);
}

uint16_t TelemetryDecoder::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t TelemetryDecoder::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

int16_t TelemetryDecoder::read_i16(const uint8_t* p) noexcept {
    return static_cast<int16_t>(read_u16(p));
}

int32_t TelemetryDecoder::read_i32(const uint8_t* p) noexcept {
    return static_cast<int32_t>(read_u32(p));
}

std::string_view TelemetryDecoder::record_type_name(TelemetryRecordType t) noexcept {
    switch (t) {
        case TelemetryRecordType::GpsFix: return "GpsFix";
        case TelemetryRecordType::ImuSample: return "ImuSample";
        case TelemetryRecordType::Attitude: return "Attitude";
        case TelemetryRecordType::Barometric: return "Barometric";
        case TelemetryRecordType::Temperature: return "Temperature";
        case TelemetryRecordType::PowerRail: return "PowerRail";
        case TelemetryRecordType::CustomPayload: return "CustomPayload";
        default: return "Unknown";
    }
}

size_t TelemetryDecoder::minimum_record_size(TelemetryRecordType t) noexcept {
    switch (t) {
        case TelemetryRecordType::GpsFix: return 22;
        case TelemetryRecordType::ImuSample: return 24;
        case TelemetryRecordType::Attitude: return 20;
        case TelemetryRecordType::Barometric: return 14;
        case TelemetryRecordType::Temperature: return 8;
        case TelemetryRecordType::PowerRail: return 12;
        case TelemetryRecordType::CustomPayload: return 10;
        default: return 2;
    }
}

DecodedTelemetry TelemetryDecoder::decode_gps(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::GpsFix;
    if (len < 22) return out;
    out.gps.timestamp_ms = read_u32(data + 1);
    out.gps.latitude_e7 = read_i32(data + 5);
    out.gps.longitude_e7 = read_i32(data + 9);
    out.gps.altitude_mm = read_i32(data + 13);
    out.gps.hdop_centi = read_u16(data + 17);
    out.gps.fix_quality = data[19];
    out.gps.satellites = data[20];
    if (out.gps.fix_quality > 5) return out;
    if (out.gps.satellites > 32) return out;
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::GpsFix)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_imu(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::ImuSample;
    if (len < 24) return out;
    out.imu.timestamp_ms = read_u32(data + 1);
    out.imu.accel_x_mg = read_i16(data + 5);
    out.imu.accel_y_mg = read_i16(data + 7);
    out.imu.accel_z_mg = read_i16(data + 9);
    out.imu.gyro_x_mdps = read_i16(data + 11);
    out.imu.gyro_y_mdps = read_i16(data + 13);
    out.imu.gyro_z_mdps = read_i16(data + 15);
    out.imu.mag_x_ut = read_i16(data + 17);
    out.imu.mag_y_ut = read_i16(data + 19);
    out.imu.mag_z_ut = read_i16(data + 21);
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::ImuSample)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_attitude(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::Attitude;
    if (len < 20) return out;
    out.attitude.timestamp_ms = read_u32(data + 1);
    out.attitude.roll_cdeg = read_i16(data + 5);
    out.attitude.pitch_cdeg = read_i16(data + 7);
    out.attitude.yaw_cdeg = read_i16(data + 9);
    out.attitude.quaternion_w = read_u16(data + 11);
    out.attitude.quaternion_x = read_u16(data + 13);
    out.attitude.quaternion_y = read_u16(data + 15);
    out.attitude.quaternion_z = read_u16(data + 17);
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::Attitude)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_baro(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::Barometric;
    if (len < 14) return out;
    out.baro.timestamp_ms = read_u32(data + 1);
    out.baro.pressure_pa = read_u32(data + 5);
    out.baro.temperature_centi = read_i16(data + 9);
    out.baro.humidity_centi = read_u16(data + 11);
    if (out.baro.pressure_pa < 10000 || out.baro.pressure_pa > 120000) return out;
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::Barometric)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_temp(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::Temperature;
    if (len < 8) return out;
    out.temp.timestamp_ms = read_u32(data + 1);
    out.temp.sensor_id = data[5];
    out.temp.value_centi = read_i16(data + 6);
    out.temp.flags = (len >= 9) ? data[8] : 0;
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::Temperature)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_power(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::PowerRail;
    if (len < 12) return out;
    out.power.timestamp_ms = read_u32(data + 1);
    out.power.rail_id = data[5];
    out.power.voltage_mv = read_u16(data + 6);
    out.power.current_ma = read_i16(data + 8);
    out.power.status = data[10];
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::PowerRail)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_custom(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    out.type = TelemetryRecordType::CustomPayload;
    if (len < 10) return out;
    out.custom.timestamp_ms = read_u32(data + 1);
    out.custom.vendor_id = read_u16(data + 5);
    out.custom.payload_type = read_u16(data + 7);
    uint16_t plen = read_u16(data + 9);
    if (len < static_cast<size_t>(11) + plen) return out;
    out.custom.payload.assign(data + 11, data + 11 + plen);
    out.valid = true;
    decode_stats_[static_cast<uint8_t>(TelemetryRecordType::CustomPayload)]++;
    return out;
}

DecodedTelemetry TelemetryDecoder::decode_record(const uint8_t* data, size_t len) noexcept {
    DecodedTelemetry out{};
    if (!data || len < 1) return out;
    auto rtype = static_cast<TelemetryRecordType>(data[0]);
    switch (rtype) {
        case TelemetryRecordType::GpsFix: return decode_gps(data, len);
        case TelemetryRecordType::ImuSample: return decode_imu(data, len);
        case TelemetryRecordType::Attitude: return decode_attitude(data, len);
        case TelemetryRecordType::Barometric: return decode_baro(data, len);
        case TelemetryRecordType::Temperature: return decode_temp(data, len);
        case TelemetryRecordType::PowerRail: return decode_power(data, len);
        case TelemetryRecordType::CustomPayload: return decode_custom(data, len);
        default: return out;
    }
}

bool TelemetryDecoder::decode_batch(const uint8_t* data, size_t len,
                                    std::vector<DecodedTelemetry>& out) noexcept {
    if (!data || len < 2) return false;
    size_t offset = 0;
    out.clear();
    batch_count_++;
    while (offset + 2 <= len) {
        uint16_t rec_len = read_u16(data + offset);
        offset += 2;
        if (rec_len == 0 || offset + rec_len > len) break;
        auto rec = decode_record(data + offset, rec_len);
        if (rec.valid) out.push_back(std::move(rec));
        offset += rec_len;
    }
    return !out.empty();
}

static bool telemetry_validate_range_0(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_0(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_0(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_1(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_1(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_1(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_2(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_2(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_2(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_3(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_3(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_3(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_4(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_4(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_4(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_5(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_5(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_5(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_6(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_6(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_6(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_7(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_7(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_7(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_8(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_8(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_8(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_9(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_9(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_9(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_10(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_10(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_10(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_11(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_11(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_11(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_12(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_12(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_12(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_13(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_13(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_13(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_14(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_14(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_14(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_15(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_15(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_15(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_16(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_16(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_16(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_17(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_17(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_17(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_18(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_18(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_18(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_19(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_19(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_19(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_20(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_20(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_20(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_21(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_21(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_21(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_22(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_22(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_22(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_23(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_23(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_23(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_24(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_24(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_24(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_25(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_25(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_25(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_26(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_26(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_26(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_27(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_27(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_27(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_28(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_28(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_28(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_29(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_29(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_29(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_30(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_30(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_30(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_31(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_31(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_31(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_32(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_32(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_32(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_33(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_33(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_33(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_34(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_34(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_34(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_35(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_35(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_35(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_36(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_36(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_36(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_37(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_37(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_37(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_38(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_38(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_38(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_39(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_39(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_39(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_40(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_40(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_40(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_41(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_41(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_41(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_42(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_42(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_42(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_43(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_43(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_43(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_44(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_44(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_44(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_45(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_45(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_45(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_46(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_46(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_46(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_47(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_47(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_47(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_48(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_48(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_48(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_49(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_49(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_49(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_50(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_50(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_50(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_51(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_51(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_51(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_52(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_52(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_52(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_53(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_53(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_53(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_54(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_54(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_54(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_55(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_55(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_55(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_56(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_56(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_56(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_57(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_57(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_57(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_58(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_58(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_58(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_59(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_59(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_59(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_60(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_60(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_60(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_61(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_61(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_61(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_62(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_62(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_62(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_63(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_63(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_63(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_64(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_64(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_64(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_65(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_65(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_65(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_66(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_66(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_66(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_67(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_67(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_67(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_68(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_68(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_68(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_69(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_69(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_69(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_70(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_70(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_70(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_71(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_71(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_71(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_72(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_72(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_72(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_73(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_73(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_73(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_74(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_74(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_74(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_75(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_75(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_75(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_76(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_76(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_76(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_77(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_77(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_77(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_78(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_78(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_78(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

static bool telemetry_validate_range_79(int32_t value, int32_t lo, int32_t hi) noexcept {
    if (value < lo || value > hi) return false;
    return true;
}

static uint32_t telemetry_hash_field_79(uint32_t seed, uint32_t field) noexcept {
    seed ^= field + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    return seed;
}

static double telemetry_interpolate_79(double a, double b, double t) noexcept {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    return a + (b - a) * t;
}

} // namespace aeroframe
