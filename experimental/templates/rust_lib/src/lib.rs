//! rust_lib template — pure-Rust library.
//!
//! Replace this with your experiment. The skeleton is here so the template
//! builds and tests pass before you touch it.

use log::{debug, info, warn};

/// A trivial statistics function. Replace with your actual experiment logic.
pub fn mean(values: &[f64]) -> Option<f64> {
    if values.is_empty() {
        warn!("mean() called on empty slice");
        return None;
    }
    debug!("mean() on {} values", values.len());
    let s: f64 = values.iter().sum();
    let m = s / values.len() as f64;
    info!("mean = {}", m);
    Some(m)
}

/// A trivial variance function. Used by `tests/basic.rs`.
pub fn variance(values: &[f64]) -> Option<f64> {
    let m = mean(values)?;
    let s: f64 = values.iter().map(|v| (v - m).powi(2)).sum();
    Some(s / values.len() as f64)
}

/// Optional: initialise Sentry for crash reporting.
///
/// Call from `main()` (or from each test that you want tracked).
/// No-op if the `sentry` feature is disabled.
#[cfg(feature = "sentry")]
pub fn init_sentry(dsn: &str, release: &str) -> sentry::ClientInitGuard {
    let _guard = sentry::init((
        dsn.to_string(),
        sentry::ClientOptions {
            release: Some(release.to_string().into()),
            ..Default::default()
        },
    ));
    _guard
}

#[cfg(not(feature = "sentry"))]
pub fn init_sentry(_dsn: &str, _release: &str) {
    log::warn!("sentry feature is disabled; Sentry not initialised");
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn mean_empty() {
        assert!(mean(&[]).is_none());
    }

    #[test]
    fn mean_basic() {
        assert_eq!(mean(&[1.0, 2.0, 3.0, 4.0, 5.0]), Some(3.0));
    }

    #[test]
    fn variance_basic() {
        // variance of [2,4,4,4,5,5,7,9] is 4.0 (population, ÷n)
        assert!((variance(&[2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0]).unwrap() - 4.0).abs() < 1e-9);
    }
}
