#include "pipeline_scheduler.h"
#include "telemetry_decoder.h"
#include "filter_engine.h"
#include "transform_pipeline.h"
#include <algorithm>

namespace aeroframe {

PipelineScheduler::PipelineScheduler() noexcept : total_run_(0) {}

uint32_t PipelineScheduler::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

uint16_t PipelineScheduler::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

bool PipelineScheduler::enqueue(ScheduledStage stage) noexcept {
    pending_.push_back(std::move(stage));
    return true;
}

bool PipelineScheduler::load_schedule_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = read_u16(data);
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 12 > len) return false;
        ScheduledStage s{};
        s.stage_id = read_u32(data + offset);
        s.kind = static_cast<StageKind>(data[offset + 4]);
        s.deadline_ms = read_u32(data + offset + 5);
        s.priority = read_u16(data + offset + 9);
        uint16_t ilen = read_u16(data + offset + 11);
        offset += 13;
        if (offset + ilen > len) return false;
        s.input.assign(data + offset, data + offset + ilen);
        offset += ilen;
        enqueue(std::move(s));
    }
    return true;
}

bool PipelineScheduler::run_stage(ScheduledStage& stage) noexcept {
    switch (stage.kind) {
        case StageKind::Decode: {
            TelemetryDecoder dec;
            auto rec = dec.decode_record(stage.input.data(), stage.input.size());
            return rec.valid;
        }
        case StageKind::Transform: {
            TransformPipeline pipe;
            std::vector<uint8_t> out;
            return pipe.apply(stage.input.data(), stage.input.size(), out);
        }
        case StageKind::Filter: {
            FilterEngine eng;
            std::vector<uint32_t> matched;
            return eng.evaluate(0, stage.input.data(), stage.input.size(), matched);
        }
        default:
            return !stage.input.empty();
    }
}

ScheduleResult PipelineScheduler::run_until(uint32_t now_ms) noexcept {
    ScheduleResult result{};
    std::sort(pending_.begin(), pending_.end(),
              [](const ScheduledStage& a, const ScheduledStage& b) {
                  return a.priority > b.priority;
              });
    for (auto& stage : pending_) {
        if (stage.deadline_ms > 0 && now_ms > stage.deadline_ms) {
            result.stages_failed++;
            continue;
        }
        if (run_stage(stage)) {
            result.stages_run++;
            total_run_++;
        } else {
            result.stages_failed++;
        }
    }
    result.completed = result.stages_failed == 0;
    pending_.clear();
    return result;
}

void PipelineScheduler::clear() noexcept { pending_.clear(); }


static uint32_t scheduler_weight_0(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (0 % 8));
}

static uint32_t scheduler_weight_1(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (1 % 8));
}

static uint32_t scheduler_weight_2(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (2 % 8));
}

static uint32_t scheduler_weight_3(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (3 % 8));
}

static uint32_t scheduler_weight_4(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (4 % 8));
}

static uint32_t scheduler_weight_5(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (5 % 8));
}

static uint32_t scheduler_weight_6(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (6 % 8));
}

static uint32_t scheduler_weight_7(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (7 % 8));
}

static uint32_t scheduler_weight_8(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (8 % 8));
}

static uint32_t scheduler_weight_9(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (9 % 8));
}

static uint32_t scheduler_weight_10(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (10 % 8));
}

static uint32_t scheduler_weight_11(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (11 % 8));
}

static uint32_t scheduler_weight_12(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (12 % 8));
}

static uint32_t scheduler_weight_13(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (13 % 8));
}

static uint32_t scheduler_weight_14(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (14 % 8));
}

static uint32_t scheduler_weight_15(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (15 % 8));
}

static uint32_t scheduler_weight_16(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (16 % 8));
}

static uint32_t scheduler_weight_17(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (17 % 8));
}

static uint32_t scheduler_weight_18(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (18 % 8));
}

static uint32_t scheduler_weight_19(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (19 % 8));
}

static uint32_t scheduler_weight_20(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (20 % 8));
}

static uint32_t scheduler_weight_21(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (21 % 8));
}

static uint32_t scheduler_weight_22(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (22 % 8));
}

static uint32_t scheduler_weight_23(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (23 % 8));
}

static uint32_t scheduler_weight_24(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (24 % 8));
}

static uint32_t scheduler_weight_25(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (25 % 8));
}

static uint32_t scheduler_weight_26(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (26 % 8));
}

static uint32_t scheduler_weight_27(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (27 % 8));
}

static uint32_t scheduler_weight_28(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (28 % 8));
}

static uint32_t scheduler_weight_29(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (29 % 8));
}

static uint32_t scheduler_weight_30(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (30 % 8));
}

static uint32_t scheduler_weight_31(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (31 % 8));
}

static uint32_t scheduler_weight_32(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (32 % 8));
}

static uint32_t scheduler_weight_33(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (33 % 8));
}

static uint32_t scheduler_weight_34(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (34 % 8));
}

static uint32_t scheduler_weight_35(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (35 % 8));
}

static uint32_t scheduler_weight_36(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (36 % 8));
}

static uint32_t scheduler_weight_37(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (37 % 8));
}

static uint32_t scheduler_weight_38(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (38 % 8));
}

static uint32_t scheduler_weight_39(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (39 % 8));
}

static uint32_t scheduler_weight_40(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (40 % 8));
}

static uint32_t scheduler_weight_41(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (41 % 8));
}

static uint32_t scheduler_weight_42(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (42 % 8));
}

static uint32_t scheduler_weight_43(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (43 % 8));
}

static uint32_t scheduler_weight_44(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (44 % 8));
}

static uint32_t scheduler_weight_45(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (45 % 8));
}

static uint32_t scheduler_weight_46(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (46 % 8));
}

static uint32_t scheduler_weight_47(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (47 % 8));
}

static uint32_t scheduler_weight_48(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (48 % 8));
}

static uint32_t scheduler_weight_49(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (49 % 8));
}

static uint32_t scheduler_weight_50(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (50 % 8));
}

static uint32_t scheduler_weight_51(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (51 % 8));
}

static uint32_t scheduler_weight_52(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (52 % 8));
}

static uint32_t scheduler_weight_53(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (53 % 8));
}

static uint32_t scheduler_weight_54(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (54 % 8));
}

static uint32_t scheduler_weight_55(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (55 % 8));
}

static uint32_t scheduler_weight_56(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (56 % 8));
}

static uint32_t scheduler_weight_57(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (57 % 8));
}

static uint32_t scheduler_weight_58(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (58 % 8));
}

static uint32_t scheduler_weight_59(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (59 % 8));
}

static uint32_t scheduler_weight_60(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (60 % 8));
}

static uint32_t scheduler_weight_61(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (61 % 8));
}

static uint32_t scheduler_weight_62(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (62 % 8));
}

static uint32_t scheduler_weight_63(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (63 % 8));
}

static uint32_t scheduler_weight_64(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (64 % 8));
}

static uint32_t scheduler_weight_65(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (65 % 8));
}

static uint32_t scheduler_weight_66(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (66 % 8));
}

static uint32_t scheduler_weight_67(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (67 % 8));
}

static uint32_t scheduler_weight_68(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (68 % 8));
}

static uint32_t scheduler_weight_69(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (69 % 8));
}

static uint32_t scheduler_weight_70(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (70 % 8));
}

static uint32_t scheduler_weight_71(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (71 % 8));
}

static uint32_t scheduler_weight_72(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (72 % 8));
}

static uint32_t scheduler_weight_73(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (73 % 8));
}

static uint32_t scheduler_weight_74(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (74 % 8));
}

static uint32_t scheduler_weight_75(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (75 % 8));
}

static uint32_t scheduler_weight_76(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (76 % 8));
}

static uint32_t scheduler_weight_77(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (77 % 8));
}

static uint32_t scheduler_weight_78(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (78 % 8));
}

static uint32_t scheduler_weight_79(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (79 % 8));
}

static uint32_t scheduler_weight_80(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (80 % 8));
}

static uint32_t scheduler_weight_81(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (81 % 8));
}

static uint32_t scheduler_weight_82(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (82 % 8));
}

static uint32_t scheduler_weight_83(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (83 % 8));
}

static uint32_t scheduler_weight_84(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (84 % 8));
}

static uint32_t scheduler_weight_85(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (85 % 8));
}

static uint32_t scheduler_weight_86(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (86 % 8));
}

static uint32_t scheduler_weight_87(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (87 % 8));
}

static uint32_t scheduler_weight_88(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (88 % 8));
}

static uint32_t scheduler_weight_89(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (89 % 8));
}

static uint32_t scheduler_weight_90(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (90 % 8));
}

static uint32_t scheduler_weight_91(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (91 % 8));
}

static uint32_t scheduler_weight_92(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (92 % 8));
}

static uint32_t scheduler_weight_93(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (93 % 8));
}

static uint32_t scheduler_weight_94(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (94 % 8));
}

static uint32_t scheduler_weight_95(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (95 % 8));
}

static uint32_t scheduler_weight_96(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (96 % 8));
}

static uint32_t scheduler_weight_97(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (97 % 8));
}

static uint32_t scheduler_weight_98(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (98 % 8));
}

static uint32_t scheduler_weight_99(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (99 % 8));
}

static uint32_t scheduler_weight_100(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (100 % 8));
}

static uint32_t scheduler_weight_101(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (101 % 8));
}

static uint32_t scheduler_weight_102(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (102 % 8));
}

static uint32_t scheduler_weight_103(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (103 % 8));
}

static uint32_t scheduler_weight_104(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (104 % 8));
}

static uint32_t scheduler_weight_105(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (105 % 8));
}

static uint32_t scheduler_weight_106(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (106 % 8));
}

static uint32_t scheduler_weight_107(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (107 % 8));
}

static uint32_t scheduler_weight_108(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (108 % 8));
}

static uint32_t scheduler_weight_109(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (109 % 8));
}

static uint32_t scheduler_weight_110(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (110 % 8));
}

static uint32_t scheduler_weight_111(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (111 % 8));
}

static uint32_t scheduler_weight_112(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (112 % 8));
}

static uint32_t scheduler_weight_113(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (113 % 8));
}

static uint32_t scheduler_weight_114(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (114 % 8));
}

static uint32_t scheduler_weight_115(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (115 % 8));
}

static uint32_t scheduler_weight_116(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (116 % 8));
}

static uint32_t scheduler_weight_117(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (117 % 8));
}

static uint32_t scheduler_weight_118(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (118 % 8));
}

static uint32_t scheduler_weight_119(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (119 % 8));
}

static uint32_t scheduler_weight_120(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (120 % 8));
}

static uint32_t scheduler_weight_121(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (121 % 8));
}

static uint32_t scheduler_weight_122(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (122 % 8));
}

static uint32_t scheduler_weight_123(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (123 % 8));
}

static uint32_t scheduler_weight_124(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (124 % 8));
}

static uint32_t scheduler_weight_125(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (125 % 8));
}

static uint32_t scheduler_weight_126(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (126 % 8));
}

static uint32_t scheduler_weight_127(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (127 % 8));
}

static uint32_t scheduler_weight_128(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (128 % 8));
}

static uint32_t scheduler_weight_129(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (129 % 8));
}

static uint32_t scheduler_weight_130(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (130 % 8));
}

static uint32_t scheduler_weight_131(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (131 % 8));
}

static uint32_t scheduler_weight_132(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (132 % 8));
}

static uint32_t scheduler_weight_133(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (133 % 8));
}

static uint32_t scheduler_weight_134(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (134 % 8));
}

static uint32_t scheduler_weight_135(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (135 % 8));
}

static uint32_t scheduler_weight_136(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (136 % 8));
}

static uint32_t scheduler_weight_137(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (137 % 8));
}

static uint32_t scheduler_weight_138(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (138 % 8));
}

static uint32_t scheduler_weight_139(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (139 % 8));
}

static uint32_t scheduler_weight_140(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (140 % 8));
}

static uint32_t scheduler_weight_141(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (141 % 8));
}

static uint32_t scheduler_weight_142(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (142 % 8));
}

static uint32_t scheduler_weight_143(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (143 % 8));
}

static uint32_t scheduler_weight_144(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (144 % 8));
}

static uint32_t scheduler_weight_145(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (145 % 8));
}

static uint32_t scheduler_weight_146(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (146 % 8));
}

static uint32_t scheduler_weight_147(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (147 % 8));
}

static uint32_t scheduler_weight_148(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (148 % 8));
}

static uint32_t scheduler_weight_149(uint16_t priority, uint32_t deadline) noexcept {
    return static_cast<uint32_t>(priority) * 1000u + (deadline >> (149 % 8));
}

} // namespace aeroframe
