cmake_minimum_required(VERSION 3.14)
project(AeroFrame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AEROFRAME_SOURCES
    src/aeroframe.cpp
    src/session.cpp
    src/cipher.cpp
    src/compress.cpp
    src/compression.cpp
    src/fragment.cpp
    src/protocol.cpp
    src/queue.cpp
    src/routing.cpp
    src/telemetry_decoder.cpp
    src/schema_registry.cpp
    src/filter_engine.cpp
    src/transform_pipeline.cpp
    src/metric_store.cpp
    src/device_registry.cpp
    src/alert_engine.cpp
    src/config_parser.cpp
    src/record_buffer.cpp
    src/event_log.cpp
    src/codec_dispatch.cpp
    src/ingest_pipeline.cpp
    src/time_sync.cpp
    src/checksum_chain.cpp
    src/dispatch_router.cpp
    src/binary_archive.cpp
    src/rate_limiter.cpp
    src/field_mapper.cpp
    src/state_store.cpp
    src/pipeline_scheduler.cpp
    src/session_store.cpp
    src/binary_validator.cpp
    src/packet_builder.cpp
)

# ---------------------------------------------------------------------------
# Core library — all modules except the CLI main()
# ---------------------------------------------------------------------------
add_library(aeroframe ${AEROFRAME_SOURCES})
target_include_directories(aeroframe PUBLIC include)

# ---------------------------------------------------------------------------
# CLI tool — packet-parsing demo
# ---------------------------------------------------------------------------
add_executable(aeroframe_parser src/main.cpp)
target_link_libraries(aeroframe_parser PRIVATE aeroframe)

# ---------------------------------------------------------------------------
# Unit tests (GoogleTest, fetched automatically)
# ---------------------------------------------------------------------------
option(AEROFRAME_BUILD_TESTS "Build unit tests" ON)
if(AEROFRAME_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# ---------------------------------------------------------------------------
# Fuzz targets (libFuzzer) — manual build via clusterfuzzlite/build.sh
# This CMake file does not build fuzz targets by default; they are
# compiled inside the ClusterFuzzLite / OSS-Fuzz container.
# ---------------------------------------------------------------------------
