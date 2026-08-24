// Debug helper for D9 correctness on 5k Gaussian.
// Run with: cargo run --example debug_d9_query6
use cc_rust::dgm_octree::{DgmOctree, compute_cell_code, get_cell_pos};

fn gaussian_cloud(n: usize, sigma: f32, seed: u64) -> Vec<[f32; 3]> {
    let mut state = seed;
    let mut next_u = || -> f32 {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        (state as f64 / u64::MAX as f64) as f32
    };
    let mut next_normal = || -> f32 {
        let u1 = next_u().max(1e-6);
        let u2 = next_u();
        let r = (-2.0 * u1.ln()).sqrt();
        let theta = 2.0 * std::f32::consts::PI * u2;
        r * theta.cos() * sigma
    };
    let mut out = Vec::with_capacity(n);
    for _ in 0..n {
        out.push([next_normal(), next_normal(), next_normal()]);
    }
    out
}

fn main() {
    let points = gaussian_cloud(5000, 0.5, 99);
    let tree = DgmOctree::build(&points, 6);
    println!("bb_min = {:?}  bb_max = {:?}", tree.bb_min, tree.bb_max);
    println!("cell_count = {}  sorted_indices.len = {}", tree.cell_count, tree.sorted_indices.len());

    // Build query 6 the same way the failing test does.
    let mut state: u64 = 7777;
    let mut queries: Vec<[f32; 3]> = Vec::with_capacity(200);
    for _ in 0..200 {
        state ^= state << 13; state ^= state >> 7; state ^= state << 17;
        let x = (state as f64 / u64::MAX as f64) as f32;
        state ^= state << 13; state ^= state >> 7; state ^= state << 17;
        let y = (state as f64 / u64::MAX as f64) as f32;
        state ^= state << 13; state ^= state >> 7; state ^= state << 17;
        let z = (state as f64 / u64::MAX as f64) as f32;
        queries.push([x * 2.0 - 1.0, y * 2.0 - 1.0, z * 2.0 - 1.0]);
    }

    let q6 = queries[6];
    println!("\nquery 6 = {:?}", q6);

    // Brute force.
    let mut bf_idx: usize = 0;
    let mut bf_d2 = f32::INFINITY;
    for (j, p) in points.iter().enumerate() {
        let dx = p[0] - q6[0];
        let dy = p[1] - q6[1];
        let dz = p[2] - q6[2];
        let d2 = dx * dx + dy * dy + dz * dz;
        if d2 < bf_d2 {
            bf_d2 = d2;
            bf_idx = j;
        }
    }
    println!("brute force: idx={} d2={} point={:?}", bf_idx, bf_d2, points[bf_idx]);

    // D9.
    let (d9_idx, d9_d2) = tree.nearest_neighbor(q6);
    println!("D9:          idx={} d2={} point={:?}", d9_idx, d9_d2, points[d9_idx]);

    // Cell code of the query.
    let q_code = compute_cell_code(q6, tree.bb_min, tree.bb_max, 6);
    let q_pos = get_cell_pos(q_code, 6, true);
    println!("\nquery code = {}  pos = {:?}", q_code, q_pos);
    println!("code of brute-force NN: {}  pos = {:?}",
             tree.codes[bf_idx], get_cell_pos(tree.codes[bf_idx], 6, true));
    println!("code of D9 NN:          {}  pos = {:?}",
             tree.codes[d9_idx], get_cell_pos(tree.codes[d9_idx], 6, true));

    // Show how many points share the brute-force NN's cell.
    let bf_code = tree.codes[bf_idx];
    let (s, e) = tree.cell_ranges.get(&bf_code).copied().unwrap_or((0, 0));
    println!("\nbrute-force cell code={} has {} points", bf_code, e - s);
    for idx in s..e {
        let p = points[tree.sorted_indices[idx]];
        let dx = p[0] - q6[0];
        let dy = p[1] - q6[1];
        let dz = p[2] - q6[2];
        let d2 = dx * dx + dy * dy + dz * dz;
        println!("  point[{}] = {:?}  d2={}", tree.sorted_indices[idx], p, d2);
    }
}
