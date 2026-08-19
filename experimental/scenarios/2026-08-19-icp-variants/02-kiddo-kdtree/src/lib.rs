//! ICP variant 2: `kiddo` KD-tree nearest-neighbour.
//!
//! Status: STUB — the kiddo 6.0 API is significantly different from earlier
//! versions (`kiddo::float::kdtree::KdTree` was removed; you now have to
//! choose a stem strategy like `Eytzinger` and a leaf strategy like
//! `VecOfArenas`). Filling in the full implementation is left as a follow-up.
//!
//! For now, this file has a tiny test that just verifies the crate compiles
//! and links. The real ICP-with-KD-tree implementation is TODO.

#[cfg(test)]
mod tests {
    #[test]
    fn kiddo_compiles() {
        // Sanity check: kiddo is in the dependency tree and compiles.
        // kiddo 6.0 needs explicit type parameters; we just verify the
        // type is reachable. The real ICP impl is TODO — see AGENTS.md.
        use kiddo::kd_tree::KdTree;
        // Use the documented type parameters. Don't try to .new() — that
        // requires more args. The mere fact that the type is nameable
        // means the crate compiled.
        let _: Option<KdTree<f64, u64, kiddo::stem_strategies::Eytzinger, kiddo::leaf_strategies::VecOfArenas<f64, u64, 3, 256>, 3, 256>> = None;
    }
}
