//! Translate a LAS file's points by a fixed (x, y, z) offset.
//! Used to create a "data" cloud for ICP testing —
//! the ICP CLI's known-translation test case.
//!
//! Usage:
//!   cargo run --release --example translate_las -- <input.las> <output.las> <tx> <ty> <tz>

use std::io::BufReader;
use std::path::PathBuf;

use las::Writer;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 6 {
        eprintln!("Usage: translate_las <input.las> <output.las> <tx> <ty> <tz>");
        std::process::exit(1);
    }
    let input = PathBuf::from(&args[1]);
    let output = PathBuf::from(&args[2]);
    let tx: f64 = args[3].parse()?;
    let ty: f64 = args[4].parse()?;
    let tz: f64 = args[5].parse()?;

    let file = std::fs::File::open(&input)?;
    let mut reader = las::Reader::new(BufReader::new(file))?;
    let header = reader.header().clone();
    eprintln!("Translating {} by ({}, {}, {})", input.display(), tx, ty, tz);

    let cursor = std::io::Cursor::new(Vec::<u8>::new());
    let mut writer = Writer::new(cursor, header)?;
    for p in reader.points() {
        let mut p = p?;
        p.x += tx;
        p.y += ty;
        p.z += tz;
        writer.write_point(p)?;
    }
    let cursor = writer.into_inner()?;
    std::fs::write(&output, cursor.into_inner())?;
    eprintln!("Wrote {}", output.display());
    Ok(())
}
