# Code Analysis 05: File I/O & Parsers
**Subsystem:** `libs/qCC_io/src/`, `plugins/core/IO/`
**Phase 4 Migration Target**
**Date:** 2026-08-19

---

## 1. File I/O Architecture

```
FileIOFilter (dispatcher)
  ├── qCoreIO → OBJ, PLY, BIN, ASC, CSV
  ├── qLASIO → LAS, LAZ (via LASzip C++)
  ├── qReCapIO → RCS, RCP (Autodesk SDK — not installed)
  └── [Other plugins]
```

**Key pattern:** `FileIOFilter::Save` / `FileIOFilter::Load` dispatches to the appropriate plugin filter based on file extension. The core `FileIOFilter` is a Qt plugin interface with `canLoad` / `canSave` / `load` / `save` virtual methods.

---

## 2. FileIOFilter.cpp — Core Dispatcher

### Location: `libs/qCC_io/src/FileIOFilter.cpp`

**Safety issues in binary chunk reading:**

```cpp
// FileIOFilter.cpp — Chunk loading (guessed pattern)
void BinaryFilter::LoadChunk(std::istream& in, Chunk& chunk) {
    // Read header — no validation
    in.read(reinterpret_cast<char*>(&chunk.type), sizeof(chunk.type));
    in.read(reinterpret_cast<char*>(&chunk.size), sizeof(chunk.size));

    // Vulnerable: size could be attacker-controlled from file
    if (chunk.size > MAX_REASONABLE_SIZE) {
        throw std::runtime_error("Chunk too large");
    }

    chunk.data.resize(chunk.size);
    in.read(chunk.data.data(), chunk.size);  // ← Could read past EOF if stream is malformed
    // No check: did we actually read chunk.size bytes?
}
```

### Rust Fix

```rust
pub fn read_chunk<R: Read>(reader: &mut R) -> io::Result<Chunk> {
    let mut header = [0u8; 12];
    reader.read_exact(&mut header)?;  // Returns error if EOF

    let chunk_type = u32::from_le_bytes(header[0..4].try_into()?);
    let size = u64::from_le_bytes(header[4..12].try_into()?);

    if size > MAX_REASONABLE_SIZE as u64 {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            format!("Chunk {} too large: {} bytes", chunk_type, size)
        ));
    }

    let mut data = vec![0u8; size as usize];
    reader.read_exact(&mut data)?;  // Fails if EOF before size bytes

    Ok(Chunk { chunk_type, data })
}
```

**Key difference:** `read_exact` returns an error if fewer bytes than requested are available. C++ `istream::read` silently fills the buffer with whatever is available, potentially reading garbage from the stream buffer.

---

## 3. Integer Overflow in Buffer Allocation

**C++ (vulnerable):**
```cpp
unsigned bufferSize = header.pointCount * sizeof(PointRecord);
void* buffer = malloc(bufferSize);  // overflow → small allocation → heap overflow
```

**Rust (safe):**
```rust
let buffer_size = header.point_count
    .checked_mul(std::mem::size_of::<PointRecord>() as u64)
    .ok_or_else(|| io::Error::new(
        io::ErrorKind::InvalidData,
        "Integer overflow in buffer size calculation"
    ))?;

if buffer_size > MAX_BUFFER_SIZE {
    return Err(io::Error::new(
        io::ErrorKind::InvalidData,
        "Buffer size exceeds maximum allowed"
    ));
}
```

---

## 4. LAS/LAZ Parser — qLASIO

### Current Architecture

```
qLASIO (C++)
  └── LASzip (C++) → LASpoint decompression
       ├── Point format 0–10 (LAS 1.0–1.4)
       ├── Variable-length records (VLR)
       ├── GPS time stamps
       └── Waveform packets
```

### Rust Replacement: las-rs + laz-perf

```rust
use las::{Reader, Point};
use laz:: LazDecompressor;

pub struct LasParseResult {
    pub points: Vec<Point3d>,
    pub colors: Option<Vec<RGB>>,
    pub intensities: Vec<u16>,
    pub gps_time: Option<Vec<f64>>,
    pub classification: Vec<u8>,
    pub header: LasHeader,
}

pub fn parse_las(path: &Path) -> Result<LasParseResult, LasError> {
    let file = std::fs::File::open(path)?;
    let mut reader = Reader::new(file)?;

    let header = reader.header().clone();
    let mut points = Vec::with_capacity(header.point_count as usize);
    let mut intensities = Vec::with_capacity(header.point_count as usize);

    // Streaming — doesn't load entire file into memory
    for result in reader.points() {
        let point = result?;
        points.push(Point3d::new(
            point.x, point.y, point.z
        ));
        intensities.push(point.intensity);
    }

    Ok(LasParseResult { points, intensities, header })
}
```

**Benefits of Rust LAS parser:**
- Streaming (doesn't require full file in memory — critical for 10GB+ LiDAR files)
- ASPRS-certified accuracy via `las-rs`
- No LASzip C++ dependency
- Memory-safe: no buffer overflow possible

---

## 5. PLY Parser — qCoreIO

### Location: `libs/qCC_io/src/PlyFilter.cpp`

PLY (Polygon File Format) is a simple mesh/cloud format with ASCII and binary variants.

**C++ concerns:**
- ASCII parsing: `sscanf` / `std::stringstream` — format string bugs possible
- Binary parsing: same chunk-size vulnerabilities as binary file filters
- Memory: entire file loaded before parsing begins

**Rust: `ply-rs` crate**

```rust
use ply_rs::{Ply, PlyElement, PlyProperty};
use std::collections::HashMap;

pub fn parse_ply(path: &Path) -> Result<PlyMesh, PlyError> {
    let file = std::fs::File::open(path)?;
    let mut decoder = ply_rs::Decoder::new(file)?;
    let ply = decoder.read_ply()?;

    // ply.elements — HashMap of element name → properties
    // e.g., "vertex" → [("x", f32), ("y", f32), ("z", f32)]
    // e.g., "face" → [("vertex_indices", Vec<u32>)]

    Ok(PlyMesh {
        vertices: parse_vertices(&ply)?,
        faces: parse_faces(&ply)?,
    })
}
```

---

## 6. Security Considerations

| File Format | CVE Risk | Rust Mitigation |
|------------|----------|----------------|
| LAS/LAZ | Buffer overflow in VLR | Streaming parse, bounds-checked |
| PLY | Integer overflow in face count | checked_mul, streaming |
| OBJ | Format string in material path | No format strings — String handling |
| E57 | Complex binary structure | `las-rs`-equivalent doesn't exist — keep C++ |
| CSV | CSV injection (formula) | Input validation, no evaluation |
| BIN (CloudCompare) | Integer overflow in chunk size | checked_mul + MAX_SIZE guard |

---

## 7. Path Traversal Prevention

```cpp
// C++ — potentially vulnerable:
QString outputPath = baseDir + "/" + filename;
// If filename = "../../../etc/passwd" → path traversal
```

```rust
// Rust — validated:
let canonical_base = std::fs::canonicalize(&base_dir)?;
let output_path = base_dir.join(&filename);
let canonical_output = std::fs::canonicalize(&output_path)?;

if !canonical_output.starts_with(&canonical_base) {
    return Err(io::Error::new(
        io::ErrorKind::PermissionDenied,
        "Path traversal attempt detected"
    ));
}
```

**For CloudCompare, path traversal is lower risk** (user-controlled local files, not network), but still worth fixing for robustness.
