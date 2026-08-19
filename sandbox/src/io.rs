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
