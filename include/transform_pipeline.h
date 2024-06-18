#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace aeroframe {

enum class TransformType : uint8_t {
    Scale, Offset, Clamp, MapRange, Interpolate, BitShift, SwapEndian, Custom
};

struct TransformStage {
    TransformType type;
    double param_a;
    double param_b;
    double param_c;
    uint16_t input_field;
    uint16_t output_field;
};

class TransformPipeline {
public:
    TransformPipeline() noexcept;

    void add_stage(const TransformStage& stage) noexcept;
    bool load_pipeline_binary(const uint8_t* data, size_t len) noexcept;
    bool apply(const uint8_t* input, size_t input_len,
               std::vector<uint8_t>& output) const noexcept;
    void clear() noexcept;
    size_t stage_count() const noexcept { return stages_.size(); }

private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static void write_u32(uint8_t* p, uint32_t v) noexcept;
    double read_field_as_double(const uint8_t* data, size_t len,
                                uint16_t field_id) const noexcept;
    void write_field_as_double(std::vector<uint8_t>& buf,
                               uint16_t field_id, double value) const noexcept;

    std::vector<TransformStage> stages_;
    mutable uint32_t apply_count_{0};
};

} // namespace aeroframe
