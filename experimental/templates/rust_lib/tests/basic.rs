//! Integration tests for the rust_lib template.
//!
//! These run with `cargo test --test basic` and exercise the public API
//! from a consumer's perspective. Add your own integration tests here.

use rust_lib_template::mean;

#[test]
fn mean_single_value() {
    assert_eq!(mean(&[42.0]), Some(42.0));
}

#[test]
fn mean_two_values() {
    assert_eq!(mean(&[1.0, 3.0]), Some(2.0));
}
