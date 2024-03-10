# AeroFrame

A modular binary packet parsing framework for embedded telemetry ingest. Handles session management, encryption, compression, fragment reassembly, protocol negotiation, routing, and priority queuing — designed for resource-constrained IoT and telemetry systems.

## Quick Start

### Build with CMake (recommended)

```bash
cmake -B build
cmake --build build
./build/aeroframe_parser   # CLI tool
```

### Run tests

```bash
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Or run the test binary directly:

```bash
./build/test/aeroframe_tests
```

### Build with Make (CLI tool only)

```bash
make
./aeroframe_parser
```

### Fuzzing (ClusterFuzzLite / OSS-Fuzz)

See `.clusterfuzzlite/build.sh` for the full build recipe. Individual fuzz targets can be compiled locally with libFuzzer:

```bash
clang++ -fsanitize=fuzzer,address -Iinclude \
    fuzz/packet_fuzzer.cpp src/*.cpp -o packet_fuzzer
```

## Architecture

```
  Binary Packet
       │
       ▼
  ┌─────────────┐
  │  AeroParser  │  ← Top-level parser (aeroframe.h/cpp)
  │  (orchestrator)
  └──────┬──────┘
         │ dispatch by TLV type
         ▼
  ┌──────┴──────┐
  │  TLV Chunks  │  Type-Length-Value tuples parsed from payload
  └──────┬──────┘
         │
  ┌──────┼──────┬──────┬──────┬──────┬──────┬──────┐
  ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼
 Session Cipher  Comp  Frag  Proto  Route  Queue  Multi
  Mgr    Xor   RLE/  Reasm  Engine Table   PriQ  Comp
                LZSS
```

Each module lives in `src/<name>.cpp` + `include/<name>.h` and is independently usable. `AeroParser` owns one instance of every module and dispatches incoming packets to the appropriate handler based on the TLV type code.

## Packet Format

```
┌───────────────────────────────────────────────────┐
│ Magic (4B)  │  Seq ID (2B) │ Total Len (2B)       │
│ 0xAF01AF01  │  big-endian  │  big-endian           │
├───────────────────────────────────────────────────┤
│ TLV #0: Type (2B) │ Length (2B) │ Value (...)      │
│ TLV #1: Type (2B) │ Length (2B) │ Value (...)      │
│ ...                                                │
├───────────────────────────────────────────────────┤
│ CRC-16 (2B) — CCITT, init 0xFFFF                  │
└───────────────────────────────────────────────────┘
```

### TLV Type Codes

| Code | Constant             | Handler                  |
|------|----------------------|--------------------------|
| 0x02 | `TYPE_SESSION_CMD`   | Open/close sessions      |
| 0x03 | `TYPE_COMPRESSED_DATA` | RLE-decompressed telemetry |
| 0x04 | `TYPE_FRAGMENT`      | Fragment insertion       |
| 0x05 | `TYPE_RETRANSMIT_REQ` | Lost fragment request    |
| 0x06 | `TYPE_PROTOCOL_MSG`  | Stateful protocol engine |
| 0x07 | `TYPE_CIPHERED_DATA` | XOR-decrypted payload    |
| 0x08 | `TYPE_ROUTING_UPDATE` | Route table management   |
| 0x09 | `TYPE_QUEUE_STATUS`  | Priority queue ops       |
| 0x0A | `TYPE_MULTI_COMPRESS` | Multi-algorithm decompress |

## Modules

### Session (`session.h`)
- `SessionManager` — tracks open sessions with telemetry buffers
- Manual memory management (`Session*` allocated on open, freed on close)
- Move semantics only (non-copyable)

### Cipher (`cipher.h`)
- `XorCipher` — XOR-based symmetric encryption
- Key derivation with `derive_round_key()`
- Precomputed word-aligned key for fast-path encryption
- Full copy and move semantics

### Compression (`compress.h`)
- RLE: run-length encoding with tagged literals/repeats
- LZSS: LZ77-style ring-buffer decompression (12-bit offset, 4-bit length)
- Dispatch function selecting algorithm by `CompressMethod` enum

### Fragment Reassembly (`fragment.h`)
- `FragmentReassembler` — collect fragments by message ID
- Retransmit queue with timeout expiry
- Complete message assembly

### Protocol Engine (`protocol.h`)
- `ProtocolEngine` — stateful handshake (Version → Capabilities → Negotiate → Established)
- Capability table with declare/ack/negotiate
- Message types: Data, Ack, Nak, Heartbeat, Control

### Routing (`routing.h`)
- `RoutingTable` — hop-based route storage
- Metric updates, TTL-based aging
- 256-entry maximum capacity

### Priority Queue (`queue.h`)
- `PriorityQueue` — 4-level priority queue (Low, Normal, High, Critical)
- Promote/demote operations, time-based expiry
- Reprioritization heuristic

## Fuzzing

Nine libFuzzer targets are available in `fuzz/`:

| Target              | Module             | Entry point                       |
|---------------------|--------------------|-----------------------------------|
| `packet_fuzzer`     | All (end-to-end)   | `AeroParser::parse()`             |
| `cipher_fuzzer`     | XorCipher          | `encrypt()` / `decrypt()`         |
| `compress_fuzzer`   | Compress dispatch  | `decompress()`, RLE, LZSS         |
| `compression_fuzzer`| Legacy RLE         | `decompress_payload()`            |
| `session_fuzzer`    | SessionManager     | Open/append/close sequence        |
| `fragment_fuzzer`   | FragmentReassembler| Push/assemble/retransmit          |
| `protocol_fuzzer`   | ProtocolEngine     | `handle_message()` state machine  |
| `routing_fuzzer`    | RoutingTable       | Add/lookup/remove/age operations  |
| `queue_fuzzer`      | PriorityQueue      | Enqueue/dequeue/promote/demote    |

Seed corpora are generated by `fuzz/generate_seeds.cpp` and automatically bundled by the ClusterFuzzLite build.

## API Reference

### `AeroParser`

```cpp
class AeroParser {
public:
    AeroParser();
    ParsedPacket parse(const std::vector<uint8_t>& data) noexcept;
};
```

### `ParsedPacket`

```cpp
struct ParsedPacket {
    PacketHeader          header;
    std::vector<TLVChunk> tlvs;
    uint16_t              crc;
    ErrorCode             error;
};
```

### `ErrorCode`

```cpp
enum class ErrorCode : uint8_t {
    None, InvalidMagic, PacketTooShort, LengthMismatch,
    InvalidCRC, TruncatedTLV, DecompressFail, CipherError,
    ProtocolError, FragmentError,
};
```

Full per-module API is documented in each header under `include/`.

## License

MIT
