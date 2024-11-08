#!/bin/bash -eu
#
# ClusterFuzzLite / OSS-Fuzz build script for AeroFrame.
#
# Compiles core library objects, builds all fuzzing harnesses,
# generates seed corpora, and packages everything into $OUT.
#
# All paths are relative to $SRC (the repository root).
# The build container has no network access.

cd "$SRC"

# ------------------------------------------------------------------
# 1.  Seed corpus generation
# ------------------------------------------------------------------
mkdir -p fuzz/corpus/{packet,compression,session,fragment,protocol,routing,queue,compress,cipher,telemetry,ingest}_fuzzer

$CXX $CXXFLAGS -Iinclude \
    fuzz/generate_seeds.cpp \
    -o /tmp/generate_seeds

/tmp/generate_seeds

# ------------------------------------------------------------------
# 2.  Core library objects (all src/*.cpp except main.cpp)
# ------------------------------------------------------------------
LIB_OBJS=()
for src_file in src/*.cpp; do
    base=$(basename "$src_file" .cpp)
    if [ "$base" = "main" ]; then
        continue
    fi
    obj="/tmp/${base}.o"
    $CXX $CXXFLAGS -Iinclude -c "$src_file" -o "$obj"
    LIB_OBJS+=("$obj")
done

# ------------------------------------------------------------------
# 3.  Fuzz targets
# ------------------------------------------------------------------

# packet_fuzzer — end-to-end parser (needs all layers)
$CXX $CXXFLAGS -Iinclude \
    fuzz/packet_fuzzer.cpp \
    "${LIB_OBJS[@]}" \
    $LIB_FUZZING_ENGINE \
    -o "$OUT/packet_fuzzer"

# Single-module fuzz targets
$CXX $CXXFLAGS -Iinclude fuzz/compression_fuzzer.cpp /tmp/compression.o \
    $LIB_FUZZING_ENGINE -o "$OUT/compression_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/session_fuzzer.cpp /tmp/session.o \
    $LIB_FUZZING_ENGINE -o "$OUT/session_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/fragment_fuzzer.cpp /tmp/fragment.o \
    $LIB_FUZZING_ENGINE -o "$OUT/fragment_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/protocol_fuzzer.cpp /tmp/protocol.o \
    $LIB_FUZZING_ENGINE -o "$OUT/protocol_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/routing_fuzzer.cpp /tmp/routing.o \
    $LIB_FUZZING_ENGINE -o "$OUT/routing_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/queue_fuzzer.cpp /tmp/queue.o \
    $LIB_FUZZING_ENGINE -o "$OUT/queue_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/compress_fuzzer.cpp /tmp/compress.o \
    $LIB_FUZZING_ENGINE -o "$OUT/compress_fuzzer"
$CXX $CXXFLAGS -Iinclude fuzz/cipher_fuzzer.cpp /tmp/cipher.o \
    $LIB_FUZZING_ENGINE -o "$OUT/cipher_fuzzer"

# Subsystem fuzz targets
$CXX $CXXFLAGS -Iinclude fuzz/telemetry_fuzzer.cpp /tmp/telemetry_decoder.o \
    $LIB_FUZZING_ENGINE -o "$OUT/telemetry_fuzzer"

$CXX $CXXFLAGS -Iinclude fuzz/ingest_fuzzer.cpp \
    /tmp/ingest_pipeline.o /tmp/telemetry_decoder.o /tmp/schema_registry.o \
    /tmp/filter_engine.o /tmp/transform_pipeline.o /tmp/metric_store.o \
    /tmp/device_registry.o /tmp/alert_engine.o /tmp/config_parser.o \
    /tmp/record_buffer.o /tmp/event_log.o /tmp/codec_dispatch.o /tmp/compress.o \
    $LIB_FUZZING_ENGINE -o "$OUT/ingest_fuzzer"

# ------------------------------------------------------------------
# 4.  Seed corpus archives
# ------------------------------------------------------------------
for target in packet compression session fragment protocol routing queue compress cipher telemetry ingest; do
    zip -j "$OUT/${target}_fuzzer_seed_corpus.zip" "fuzz/corpus/${target}_fuzzer"/* || true
done
