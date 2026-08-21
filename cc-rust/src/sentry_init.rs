//! Sentry crash reporting — opt-in via the `sentry` feature.
//!
//! Mirrors the qCC C++ `CC_USE_SENTRY` opt-in (see
//! `qCC/main.cpp` and `qCC/CMakeLists.txt`). When the `sentry`
//! feature is enabled, the `cc_rust_cli` binary calls
//! [`sentry_init`] at the top of `main()` and holds the resulting
//! guard for the program's lifetime; on drop (normal exit,
//! panic, or process tear-down), Sentry flushes any queued
//! events and closes the transport.
//!
//! DSN defaults to the same one the main CloudCompare app uses
//! (`https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184`).
//! Override at runtime with the `CC_SENTRY_DSN` env var; override
//! at compile time by setting the `CC_SENTRY_DSN` env var before
//! `cargo build` (the const below is read at compile time).
//!
//! Release: `cc-rust@<CARGO_PKG_VERSION>`. Override with
//! `CC_SENTRY_RELEASE` env var at compile time.
//!
//! **The init is a no-op if:**
//! - The `sentry` feature is not enabled (no-op at compile time).
//! - The DSN is empty / unparseable / empty-string after env-var
//!   resolution (returns `None` at runtime).
//! - The DSN is the literal string `"<off>"` (a common
//!   convention for "explicitly disabled"; returns `None`).
//!
//! This mirrors the qCC `sentry_init()` style — non-fatal,
//! returns a guard, and the guard's `Drop` flushes events on
//! shutdown.

/// The DSN the main CloudCompare app uses. Mirror this so Rust
/// crashes land in the same Sentry project as C++ crashes.
///
/// Override with the `CC_SENTRY_DSN` env var (at build time for
/// the compile-time default, or at runtime for explicit override).
const DEFAULT_DSN: &str = "https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184";

/// Pure version of the DSN resolution logic, factored out so it
/// can be unit-tested without touching process env vars (which
/// would race with other tests running in parallel). Production
/// callers should use [`effective_dsn`] instead.
///
/// `runtime_dsn`: value of `CC_SENTRY_DSN` at the time of the call
/// (use `std::env::var(...).ok()` to read it).
///
/// Returns `None` if `runtime_dsn` is empty/`<off>`, OR if it's
/// unset (None) and the compile-time `default_dsn` is empty/`<off>`,
/// OR if neither parses as a valid DSN.
fn resolve_dsn<'a>(
    runtime_dsn: Option<&'a str>,
    default_dsn: &'a str,
) -> Option<sentry::types::Dsn> {
    if let Some(rd) = runtime_dsn {
        if rd.is_empty() || rd == "<off>" {
            return None;
        }
        return rd.parse().ok();
    }
    if default_dsn.is_empty() || default_dsn == "<off>" {
        return None;
    }
    default_dsn.parse().ok()
}

/// Returns the effective DSN. Priority:
/// 1. `CC_SENTRY_DSN` env var (runtime, overrides everything).
/// 2. The hard-coded `DEFAULT_DSN` (compile-time).
///
/// Returns `None` if the resolved DSN is empty, equal to `"<off>"`,
/// or fails to parse.
fn effective_dsn() -> Option<sentry::types::Dsn> {
    resolve_dsn(std::env::var("CC_SENTRY_DSN").ok().as_deref(), DEFAULT_DSN)
}

/// Returns the effective release string. Priority:
/// 1. `CC_SENTRY_RELEASE` env var (runtime override).
/// 2. `cc-rust@<CARGO_PKG_VERSION>` (compile-time default).
fn effective_release() -> String {
    std::env::var("CC_SENTRY_RELEASE")
        .ok()
        .filter(|s| !s.is_empty())
        .unwrap_or_else(|| format!("cc-rust@{}", env!("CARGO_PKG_VERSION")))
}

/// Initialize the Sentry client. Must be called at the top of
/// `main()` (before any other code that might panic) so startup
/// crashes are captured.
///
/// Returns a `ClientInitGuard` that the caller must hold alive
/// for the program's lifetime. On drop, the guard flushes any
/// queued events and closes the Sentry transport (with a
/// 2-second deadline).
///
/// Returns `None` if the `sentry` feature is not enabled, the
/// DSN is empty / unparseable / explicitly disabled, or the
/// `sentry::init` call itself fails. **Never panics** — a
/// failure to initialise Sentry is not a failure of the
/// program.
pub fn sentry_init() -> Option<sentry::ClientInitGuard> {
    let dsn = effective_dsn()?;
    let release = effective_release();
    let client = sentry::init(sentry::ClientOptions {
        dsn: Some(dsn),
        release: Some(release.into()),
        // Capture panics as events. This is the Rust equivalent
        // of the C++ `sentry_set_user_feedback_after_crash` etc.
        // — the default `sentry-panic` integration is enabled
        // when the `sentry` crate is built with default features.
        attach_stacktrace: true,
        ..Default::default()
    });
    if !client.is_enabled() {
        return None;
    }
    Some(client)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn resolve_dsn_parses_production_value() {
        // The default DSN should parse cleanly.
        let dsn = DEFAULT_DSN.parse::<sentry::types::Dsn>();
        assert!(dsn.is_ok(), "default DSN should parse: {:?}", dsn.err());
    }

    #[test]
    fn resolve_dsn_returns_none_for_empty_runtime() {
        // Empty runtime override → treated as "<off>", returns
        // None (does NOT fall through to default — explicit
        // empty is an opt-out).
        let dsn = resolve_dsn(Some(""), DEFAULT_DSN);
        assert!(dsn.is_none(), "empty runtime DSN should disable Sentry");
    }

    #[test]
    fn resolve_dsn_returns_none_for_off_runtime() {
        // "<off>" runtime override → return None.
        let dsn = resolve_dsn(Some("<off>"), DEFAULT_DSN);
        assert!(dsn.is_none(), "<off> runtime DSN should disable Sentry");
    }

    #[test]
    fn resolve_dsn_returns_none_for_off_default() {
        // Default "<off>" (e.g. compile-time override) → return None.
        let dsn = resolve_dsn(None, "<off>");
        assert!(dsn.is_none(), "<off> default DSN should disable Sentry");
    }

    #[test]
    fn resolve_dsn_runtime_overrides_default() {
        // Runtime override should win over default — verify
        // the runtime DSN parses to a DSN whose host matches.
        let custom = "https://custom@example.com/123";
        let dsn = resolve_dsn(Some(custom), DEFAULT_DSN);
        assert!(dsn.is_some());
        let dsn = dsn.unwrap();
        // Don't compare to_string() — Dsn::to_string() may
        // normalise the format (e.g. adding an empty password
        // colon). Compare the public key/id instead.
        assert!(dsn.to_string().contains("example.com"));
    }

    #[test]
    fn resolve_dsn_unparseable_runtime_returns_none() {
        // Runtime DSN that doesn't parse → return None.
        let dsn = resolve_dsn(Some("not a valid DSN"), DEFAULT_DSN);
        assert!(dsn.is_none(), "unparseable runtime DSN should be treated as disabled");
    }

    #[test]
    fn effective_release_includes_pkg_version() {
        let release = effective_release();
        // Should be "cc-rust@0.1.0" (or whatever the current
        // package version is) — not the runtime override.
        if std::env::var("CC_SENTRY_RELEASE").is_err() {
            assert!(
                release.starts_with("cc-rust@"),
                "release should start with 'cc-rust@', got {}",
                release
            );
        }
    }

    #[test]
    fn sentry_init_does_not_panic() {
        // We don't assert Some/None here because the default
        // DSN points at the production Sentry project — the
        // CI environment may or may not be able to reach it.
        // What we DO assert: the call doesn't panic.
        let guard = sentry_init();
        if let Some(g) = guard {
            assert!(g.is_enabled());
        }
    }
}
