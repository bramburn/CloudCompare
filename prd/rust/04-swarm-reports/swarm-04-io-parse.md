# Swarm Report 04: File I/O & Parser Analysis
**Agent:** explore — I/O and parser specialist
**Scope:** `libs/qCC_io/`, `qCC/` file handling, plugin I/O
**Date:** 2026-08-19

---

## 1. I/O Architecture Overview

CloudCompare's I/O system uses a plugin-based `FileIOFilter` architecture:

```
FileIOFilter (base class)
├── Core I/O (qCoreIO) — OBJ, PLY, BIN
├── LAS/LAZ (qLASIO) — via LASzip
├── E57 (qE57IO) — via Xerces-C++
├── ReCap RCS/RCP (qReCapIO) — via Autodesk ReCap SDK
└── [Other plugins...]
```

**Key insight:** I/O is inherently about **untrusted external input validation** — the #1 use case for Rust's safety guarantees.

---

## 2. I/O Modules with Rust Migration Potential

### 2.1 `FileIOFilter.cpp` — Core I/O Dispatcher

**File:** `libs/qCC_io/src/FileIOFilter.cpp` (~1,200 lines)

**Key patterns:**
```cpp
// Pattern: File extension → filter dispatch
FileIOFilter::Save(const QString& filename, ccHObject* obj, ...) {
    // Dispatches to appropriate plugin filter based on extension
    // Lots of shared state: m_associatedEntity, m_saveParameters
    for (auto* filter : m_filters) {
        if (filter->canSave(filename)) {
            filter->save(filename, obj, ...);
        }
    }
}

// Pattern: Binary format reading with manual buffer management
void ReadBinaryChunk(std::istream& in, ccChunkType& chunk) {
    in.read(reinterpret_cast<char*>(&chunk.header), sizeof(chunk.header));
    if (chunk.size > MAX_CHUNK_SIZE) {
        throw std::runtime_error("Chunk too large");  // not caught everywhere
    }
    chunk.data.resize(chunk.size);
    in.read(chunk.data.data(), chunk.size);  // potential overread if stream corrupted
}
```

**Rust win:**
```rust
// Zero-copy buffer reads with checked sizes
fn read_chunk<R: Read>(reader: &mut R) -> io::Result<Chunk> {
    let mut header = [0u8; CHUNK_HEADER_SIZE];
    reader.read_exact(&mut header)?;
    let size = u64::from_le_bytes(header[4..12].try_into()?);

    if size > MAX_CHUNK_SIZE {
        return Err(io::Error::new(io::ErrorKind::InvalidData, "chunk too large"));
    }

    let mut data = vec![0u8; size as usize];
    reader.read_exact(&mut data)?;
    Ok(Chunk { header, data })
}
```

### 2.2 `ccCommandLineCommands.cpp` — Batch Processing I/O

**File:** `qCC/ccCommandLineCommands.cpp`

**Key patterns:**
```cpp
// Pattern: Filename from command line — potential injection
void ProcessFile(const QString& filename) {
    auto* filter = FileIOFilter::GetFileIOFilter(filename);
    // ...
}

// Pattern: Large file memory mapping (mmap on Linux, CreateFileMapping on Windows)
// No bounds checking on mapped region access
```

**Rust win:** `memmap2` crate for safe memory-mapped file access with explicit lifetime:
```rust
let mmap = unsafe { Mmap::map(&file)? };
// Access is bounds-checked at the Mmap level
```

### 2.3 LAS/LAZ Reader (qLASIO)

**File:** `plugins/core/IO/qLASIO/src/`

LASzip is a C++ library for LiDAR compression. Key concerns:
- `LASReader` reads variable-length records
- Point format varies by LAS version (1.0–1.4)
- GPS time stamps, waveform packets — complex binary structures
- `return_number`, `number_of_returns` — uint8 fields with semantic constraints

**Rust candidate:** `las-rs` or `laz-perf` (pure Rust LAS/LAZ codec) — could replace the LASzip C++ dependency entirely.

```rust
// las-rs example
use las::{Reader, Point};
let file = std::fs::File::open("input.las")?;
let mut reader = Reader::new(file)?;
let header = reader.header();
// Points are Iterator<Item = Result<Point>> — streaming, no full-load required
for point in reader.points() {
    process_point(point?);
}
```

**Note:** LASzip (used by qLASIO) is C++. A Rust LAS codec would eliminate one C++ dependency.

---

## 3. Parser Security Concerns (Rust Opportunity)

### 3.1 Buffer Overread / Overflow

```cpp
// DgmOctree.cpp — loading binary octree from file
// If the file is truncated, read() may read past EOF (undefined in C++)
// C++ streams may partially fill the buffer or return fewer bytes than requested
void LoadFromBinary(std::istream& in) {
    char buffer[1024];
    in.read(buffer, 1024);  // No check: did we actually get 1024 bytes?
    // ...
}
```

**Rust:**
```rust
fn read_exact_all<R: Read>(reader: &mut R, buf: &mut [u8]) -> io::Result<()> {
    reader.read_exact(buf)  // Returns Err if EOF before buf is filled
}
// read_exact is the only safe way — panics if short read
```

### 3.2 Integer Overflow in Size Calculations

```cpp
// FileIOFilter — allocating buffer based on header values from file
unsigned bufferSize = header.pointCount * sizeof(PointRecord);
// If header.pointCount is attacker-controlled and large:
// bufferSize overflows → small allocation → heap overflow
```

**Rust:** `usize` is guaranteed to be the pointer size (64-bit on x64). Multiplication overflow in Rust `usize` **panics in debug** and **wraps in release** — this is a known issue. Use `.checked_mul()` for untrusted input:
```rust
let buffer_size = header.point_count
    .checked_mul(std::mem::size_of::<PointRecord>() as u64)
    .ok_or_else(|| io::Error::new(io::ErrorKind::InvalidData, "size overflow"))?;
```

### 3.3 Path Traversal in File Operations

```cpp
// Any file save/load that uses the filename directly:
QString outputPath = baseDir + "/" + filename;  // Potential path traversal
// If filename = "../../../etc/passwd", overwrites system files
```

**Rust:** `std::path::Path` with `canonicalize()` for validation:
```rust
let canonical_base = std::fs::canonicalize(&base_dir)?;
let output_path = base_dir.join(&filename);
let canonical_output = std::fs::canonicalize(&output_path)?;
if !canonical_output.starts_with(&canonical_base) {
    return Err(io::Error::new(io::ErrorKind::PermissionDenied, "path traversal"));
}
```

---

## 4. FFI Complexity for I/O Subsystems

**FFI complexity: HIGH** for file I/O because:

1. **Qt File I/O** — `QFile`, `QDataStream` are Qt classes; cannot be used directly from Rust
2. **Plugin system** — `FileIOFilter` is a Qt plugin interface; Rust FFI would require C-only interface
3. **GUI integration** — progress dialogs via `GenericProgressCallback*` (Qt callback)
4. **Error handling** — C++ exceptions (`throw std::runtime_error`) must not cross FFI boundary

**Recommended approach for I/O:**

Do **not** migrate the full I/O pipeline to Rust. Instead:
1. Keep `FileIOFilter` and plugin architecture in C++
2. Write **pure parsing/deserialisation** logic in Rust as a standalone library
3. Expose via CXX as `Box<dyn ReadBinary>` or `read_point_cloud(bytes: &[u8]) -> Result<PointCloudView>`
4. The C++ side calls the Rust parser, then wraps the result in `ccPointCloud`

```
C++ FileIOFilter
    ↓ (calls)
Rust las_parser.rs  ← pure parsing, no Qt dependency
    ↓ (returns)
ccPointCloud (C++) ← wraps result
```

---

## 5. File Format Rust Libraries

| Format | Rust Crate | Status | Notes |
|--------|-----------|--------|-------|
| LAS/LAZ LiDAR | `las-rs`, `laz-perf` | Active | laz-perf is ASPRS-certified |
| PLY | `ply-rs` | Active | Polygon file format |
| OBJ | `tobj` | Active | Wavefront OBJ (basic) |
| E57 | None | — | No pure Rust E57 (needs ASMGEEK E57lib C bindings) |
| CSV | `csv` | Very active | For scalar field export |
| PCD (PCL) | `pcd.rs` (if exists) | Unknown | Check crates.io |
| HDF5 | `hdf5` | Active | Scientific data |
| TIFF/GeoTIFF | `tiff`, `geoimage` | Active | For raster/georeferenced |

**For CloudCompare I/O Phase:**
- **LAS/LAZ**: Replace LASzip with `las-rs` or `laz-perf` (pure Rust, no C++)
- **PLY**: Replace with `ply-rs`
- **CSV**: Use `csv` crate for scalar field import/export
- **OBJ**: `tobj` for import (limited material support)
