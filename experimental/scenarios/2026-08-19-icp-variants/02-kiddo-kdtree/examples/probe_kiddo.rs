// Quick test of the kiddo 6.0 API.
// Usage: cargo run --release --example probe_kiddo
use kiddo::KdTree;
use kiddo::SquaredEuclidean;

fn main() {
    let points: Vec<[f64; 3]> = vec![
        [0.0, 0.0, 0.0],
        [1.0, 1.0, 1.0],
        [0.5, 0.5, 0.5],
        [2.0, 3.0, 4.0],
    ];
    // kiddo 6.0 type parameters: <A, T, SS, LS, const K, const B>.
    // Use `new_from_slice` which auto-assigns item = insertion index.
    let kdtree: KdTree<
        f64,
        u32,
        kiddo::stem_strategies::EytzingerFlexPf,
        kiddo::leaf_strategies::VecOfArenas<f64, u32, 3, 32>,
        3,
        32,
    > = KdTree::new_from_slice(&points).expect("kd-tree construction");
    println!("size = {}", kdtree.size());

    // Query: nearest_one via the builder pattern
    let result = kdtree
        .query(&[0.1, 0.0, 0.0])
        .nearest_one::<SquaredEuclidean<f64>>()
        .execute();
    println!("nearest to (0.1,0,0): dist={}, item={}", result.distance, result.item);
    assert_eq!(result.item, 0);

    // Another query
    let r2 = kdtree
        .query(&[2.0, 3.0, 4.05])
        .nearest_one::<SquaredEuclidean<f64>>()
        .execute();
    println!("nearest to (2,3,4.05): dist={}, item={}", r2.distance, r2.item);
    assert_eq!(r2.item, 3);
}

