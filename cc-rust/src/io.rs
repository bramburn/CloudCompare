// src/io.rs — Phase 4: File I/O (placeholder)
//
// This module is a placeholder for Phase 4 (file parsers).
// For now, LAS loading is handled via the CXX bridge in cpp/las_loader.cc.
//
// Phase 4 targets:
//   • LAS / LAZ: replace qLASIO (uses C++ LASzip)
//   • PLY:       replace FileIOFilter's built-in PLY parser
//   • CSV:       new pure-Rust CSV reader (no C++ needed)
//
// The strategy is:
//   • Pure-Rust crates (csv, las-rs) do not need CXX
//   • Load via Rust, pass Vec<f32> back through the existing CXX bridge
//   • This eliminates LASzip and shapelib C++ dependencies entirely
//
// Stub only — filled in Phase 4.

use thiserror::Error;

#[derive(Debug, Error)]
pub enum IoError {
    #[error("file not found: {0}")]
    NotFound(String),

    #[error("parse error at byte {byte}: {message}")]
    ParseError { byte: usize, message: String },

    #[error("unsupported format: {0}")]
    UnsupportedFormat(String),
}

pub struct CloudData {
    pub points: Vec<f32>,       // interleaved xyz
    pub colors: Option<Vec<u8>>, // RGB
    pub intensities: Option<Vec<f32>>,
    pub normals: Option<Vec<f32>>,
}

/// Load a CSV file with xyz columns.
/// Pure Rust — no C++ needed.
pub fn load_csv(path: &str) -> Result<CloudData, IoError> {
    let mut rdr = csv::ReaderBuilder::new()
        .has_headers(true)
        .flexible(true)
        .from_path(path)
        .map_err(|_| IoError::NotFound(path.to_string()))?;

    let mut points = Vec::new();

    for result in rdr.records() {
        let record = result.map_err(|e| IoError::ParseError {
            byte: 0,
            message: e.to_string(),
        })?;

        // Expect at least x, y, z columns
        if record.len() >= 3 {
            for field in record.iter().take(3) {
                let v: f32 = field.parse().unwrap_or(0.0);
                points.push(v);
            }
        }
    }

    Ok(CloudData {
        points,
        colors: None,
        intensities: None,
        normals: None,
    })
}

// ===========================================================================
// LAS / LAZ reader (Phase 4 strategy — D6 in
// `experimental/docs/decisions.md`). Pure-Rust via the `las` crate;
// drops the C++ `LASzip` dep that `qLASIO` (the existing IO plugin)
// uses. Reads both `.las` (LAS 1.0-1.4 PDRF, uncompressed) and
// `.laz` (chunked-compressed LASzip).
//
// API design:
//   - `read_las_points(path) -> Vec<[f32; 3]>` is the primary entry.
//     Returns a flat list of (x, y, z) tuples in f32, matching the
//     rest of cc-rust (ICP, D9, etc.) which all use f32.
//   - `count_las_points(path) -> u64` reads just the header for
//     pre-allocation without touching the point stream.
//   - The CSV-based `load_csv` above remains the lightweight path;
//     this new `read_las_points` is the binary-format path.
//
// Performance: 7.2M pts/s on real survey data (D6 benchmark). For
// the canonical 7.5M-point brook-avenue scan, that's ~1 second of
// file-load time before ICP starts.
// ===========================================================================

use std::fs::File;
use std::io::BufReader;
use std::path::Path;

/// Read all points from a LAS / LAZ file as a flat list of
/// (x, y, z) tuples in f32. The coordinates are in the LAS
/// file's native reference frame (no transform applied).
///
/// Equivalent to the C++ `qLASIO` plugin's "load all" path, but
/// with the C++ `LASzip` dependency replaced by a pure-Rust
/// implementation. Supports both `.las` (LAS 1.0-1.4 PDRF) and
/// `.laz` (chunked-compressed LASzip).
///
/// The returned `Vec` is `O(N)` in the number of points — no
/// per-point allocation (the inner array is fixed-size
/// `[f32; 3]`, stored in a contiguous `Vec`).
pub fn read_las_points<P: AsRef<Path>>(path: P) -> Result<Vec<[f32; 3]>, IoError> {
    let path_ref = path.as_ref();
    let file = File::open(path_ref)
        .map_err(|_| IoError::NotFound(path_ref.display().to_string()))?;

    // `las::Reader::new` takes anything that implements
    // `std::io::Read`. We wrap in `BufReader` for performance
    // (the `las` crate's docs recommend this — file reads
    // are uncached otherwise).
    let mut reader = las::Reader::new(BufReader::new(file))
        .map_err(|e| IoError::ParseError { byte: 0, message: format!("open reader: {e}") })?;

    // Pre-allocate to the declared point count. The header's
    // number_of_points() is the upper bound; the actual point
    // count may be less if the file is truncated. We use the
    // declared count to avoid reallocations.
    let n = reader.header().number_of_points() as usize;
    let mut out: Vec<[f32; 3]> = Vec::with_capacity(n);

    for point in reader.points() {
        let p = point.map_err(|e| IoError::ParseError { byte: 0, message: format!("read point: {e}") })?;
        // LAS stores coordinates as f64; the rest of cc-rust
        // (ICP, D9, etc.) uses f32. Cast loses precision but
        // matches the canonical C++ side (which also reads
        // LAS f64 and casts to f32 in the .las loader).
        out.push([p.x as f32, p.y as f32, p.z as f32]);
    }

    Ok(out)
}

/// Number of points declared in a LAS file's header. Useful
/// for pre-allocating ICP result containers without reading
/// the full point stream.
pub fn count_las_points<P: AsRef<Path>>(path: P) -> Result<u64, IoError> {
    let path_ref = path.as_ref();
    let file = File::open(path_ref)
        .map_err(|_| IoError::NotFound(path_ref.display().to_string()))?;
    let reader = las::Reader::new(BufReader::new(file))
        .map_err(|e| IoError::ParseError { byte: 0, message: format!("open reader: {e}") })?;
    Ok(reader.header().number_of_points())
}

#[cfg(test)]
mod las_tests {
    use super::*;
    use std::io::Cursor;
    use tempfile::tempdir;

    /// Build a small in-memory LAS file (uncompressed) with
    /// `n` points at coarse integer coordinates in [0, 100) per
    /// axis. The coarse step avoids LAS-format quantization
    /// noise (LAS stores points as scaled i32s — values like
    /// 0.0025 would round to 0.003 with a 0.01 scale). Returns
    /// the raw bytes ready to write to disk.
    fn make_test_las(n: u32) -> Vec<u8> {
        let header = las::Header::from((1, 4));
        let builder = las::Builder::from(header);
        let mut writer =
            las::Writer::new(Cursor::new(Vec::<u8>::new()), builder.into_header().unwrap())
                .unwrap();

        for i in 0..n {
            // Coarse integer coordinates — exactly representable
            // in f32 and immune to LAS i32 quantisation.
            let mut p = las::Point::default();
            p.x = i as f64;
            p.y = (i as f64) * 0.5;
            p.z = (i as f64) * 0.25;
            // `write_point` is the non-deprecated form; the
            // writer auto-tracks point count + bounds.
            writer.write_point(p).unwrap();
        }
        writer.into_inner().unwrap().into_inner()
    }

    /// The expected points for `make_test_las(n)`.
    fn expected_points(n: u32) -> Vec<[f32; 3]> {
        (0..n)
            .map(|i| [i as f32, (i as f32) * 0.5, (i as f32) * 0.25])
            .collect()
    }

    #[test]
    fn read_las_points_recovers_uncompressed_file() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test.las");
        std::fs::write(&path, make_test_las(100)).unwrap();

        let points = read_las_points(&path).expect("read should succeed");
        let expected = expected_points(100);
        assert_eq!(points.len(), expected.len());
        for (i, (got, want)) in points.iter().zip(expected.iter()).enumerate() {
            // Coarse integer coordinates — well within f32
            // precision and immune to LAS i32 quantisation.
            // 1e-5 absolute tolerance is conservative.
            for axis in 0..3 {
                let diff = (got[axis] - want[axis]).abs();
                assert!(
                    diff < 1e-5,
                    "point {i} axis {axis}: got {}, want {} (diff {})",
                    got[axis],
                    want[axis],
                    diff
                );
            }
        }
    }

    #[test]
    fn read_las_points_count_matches_header() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("count_test.las");
        std::fs::write(&path, make_test_las(42)).unwrap();

        let declared = count_las_points(&path).expect("count should succeed");
        let actual = read_las_points(&path).expect("read should succeed");
        assert_eq!(declared, 42);
        assert_eq!(actual.len(), 42);
    }

    #[test]
    fn read_las_points_handles_missing_file() {
        let dir = tempdir().unwrap();
        let bogus = dir.path().join("does_not_exist.las");
        let result = read_las_points(&bogus);
        assert!(matches!(result, Err(IoError::NotFound(_))));
    }

    #[test]
    fn read_las_points_handles_corrupt_file() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("corrupt.las");
        // 1KB of zeros — valid LAS magic is 'LASF', so the
        // las crate should fail to parse.
        std::fs::write(&path, vec![0u8; 1024]).unwrap();
        let result = read_las_points(&path);
        assert!(matches!(
            result,
            Err(IoError::ParseError { .. })
        ));
    }

    #[test]
    fn read_las_points_handles_empty_file() {
        // Header says 0 points — should return empty Vec, not
        // error. Tests the `number_of_points` pre-allocation path.
        let dir = tempdir().unwrap();
        let path = dir.path().join("empty.las");
        std::fs::write(&path, make_test_las(0)).unwrap();
        let points = read_las_points(&path).expect("read should succeed");
        assert!(points.is_empty());
    }
}
