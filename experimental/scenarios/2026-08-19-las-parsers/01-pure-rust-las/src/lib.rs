//! Pure-Rust LAS reader using the `las` crate.
//!
//! Strategy 1 of the Phase 4 scenario. Replaces CCCoreLib's reliance
//! on `LASzip` (C++).
//!
//! Limitations:
//! - `.las` only. `.laz` (compressed) needs the `laz` crate as well.
//! - Loads all points into memory. For a 250MB `.las`, expect 1-2 GB RAM.
//! - API: returns a `Reader` that yields `las::Point` per call.

use std::path::Path;
use std::fs::File;
use std::io::BufReader;
use las::Reader;

/// Open a `.las` file and report the header + point count.
pub fn inspect(path: &Path) -> Result<LasInfo, String> {
    let file = File::open(path).map_err(|e| format!("open: {}", e))?;
    let mut reader = Reader::new(BufReader::new(file))
        .map_err(|e| format!("parse header: {}", e))?;
    let header = reader.header();
    Ok(LasInfo {
        point_count: header.number_of_points(),
        version: format!(
            "{}.{}",
            header.version().major, header.version().minor
        ),
        point_format_id: header.point_format().to_u8()
            .map_err(|e| format!("point format: {}", e))?,
        min: [
            header.bounds().min.x as f32,
            header.bounds().min.y as f32,
            header.bounds().min.z as f32,
        ],
        max: [
            header.bounds().max.x as f32,
            header.bounds().max.y as f32,
            header.bounds().max.z as f32,
        ],
    })
}

/// Read all points from a `.las` file, returning interleaved xyz + intensity.
/// The `las` crate returns f64 for coordinates; we keep f64 internally
/// for accuracy and only narrow on the CXX boundary (ccPointCloud stores f32).
pub fn read_all_xyz(path: &Path) -> Result<Vec<f64>, String> {
    let file = File::open(path).map_err(|e| format!("open: {}", e))?;
    let mut reader = Reader::new(BufReader::new(file))
        .map_err(|e| format!("parse header: {}", e))?;

    let n = reader.header().number_of_points() as usize;
    let mut out = Vec::with_capacity(n * 4);  // x, y, z, intensity

    for p in reader.points() {
        let p = p.map_err(|e| format!("point read: {}", e))?;
        out.push(p.x);
        out.push(p.y);
        out.push(p.z);
        // Intensity is a u16 in the LAS spec; normalise to f64 in [0, 1]
        // for downstream scalar-field work. Caller can un-normalise.
        out.push(p.intensity as f64 / 65535.0);
    }
    Ok(out)
}

#[derive(Debug, Clone)]
pub struct LasInfo {
    pub point_count: u64,
    pub version: String,
    pub point_format_id: u8,
    pub min: [f32; 3],
    pub max: [f32; 3],
}

impl std::fmt::Display for LasInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "LAS version:       {}", self.version)?;
        writeln!(f, "Point format:      {}", self.point_format_id)?;
        writeln!(f, "Point count:       {}", self.point_count)?;
        writeln!(f, "Bounds min:        ({}, {}, {})", self.min[0], self.min[1], self.min[2])?;
        writeln!(f, "Bounds max:        ({}, {}, {})", self.max[0], self.max[1], self.max[2])
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn smoke_compile() {
        // Just verify the type signatures are correct.
        let _: fn(&std::path::Path) -> Result<LasInfo, String> = inspect;
    }
}
