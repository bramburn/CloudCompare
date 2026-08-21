//! Subsample a LAS file to a smaller LAS file (every Nth point).
//! Used to create test fixtures from the 7.5M brook-avenue scan.
//!
//! Usage:
//!   cargo run --release --example subsample_las -- <input.las> <output.las> <stride>

use std::io::BufReader;
use std::path::PathBuf;

use las::Writer;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 4 {
        eprintln!("Usage: subsample_las <input.las> <output.las> <stride>");
        std::process::exit(1);
    }
    let input = PathBuf::from(&args[1]);
    let output = PathBuf::from(&args[2]);
    let stride: usize = args[3].parse()?;

    let file = std::fs::File::open(&input)?;
    let mut reader = las::Reader::new(BufReader::new(file))?;
    let header = reader.header().clone();
    eprintln!("Reading {} ({} points declared)",
        input.display(), header.number_of_points());

    let points: Vec<[f64; 3]> = reader
        .points()
        .step_by(stride)
        .filter_map(|p| p.ok())
        .map(|p| [p.x, p.y, p.z])
        .collect();
    eprintln!("Wrote {} points (stride {})", points.len(), stride);

    let cursor = std::io::Cursor::new(Vec::<u8>::new());
    let mut writer = Writer::new(cursor, header)?;
    // `las::Point::default()` has all optional fields as
    // `None`. If the template's point format requires GPS
    // time and color (as brook-avenue does — PDRF format
    // 6 with both), writing a default point fails. The
    // `WritePoint` impl enforces "the point's fields must
    // match the format exactly". We read the original
    // points and write them back (subsampled) so all
    // optional fields are preserved exactly.
    let file = std::fs::File::open(&input)?;
    let mut reader = las::Reader::new(BufReader::new(file))?;
    let header = reader.header().clone();
    let mut point_iter = reader.points();
    let mut written = 0usize;
    for i in 0.. {
        let p = match point_iter.next() {
            Some(p) => p?,
            None => break,
        };
        if i % stride == 0 {
            writer.write_point(p)?;
            written += 1;
        }
    }
    eprintln!("Wrote {} points (stride {})", written, stride);
    let cursor = writer.into_inner()?;
    std::fs::write(&output, cursor.into_inner())?;
    eprintln!("Wrote {}", output.display());
    Ok(())
}
