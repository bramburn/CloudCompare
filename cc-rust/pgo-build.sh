#!/usr/bin/env bash
#
# PGO build script for cc-rust.
#
# PGO (Profile-Guided Optimization) is a two-pass compile:
#   1. Build with `-Cprofile-generate=<dir>` to add instrumentation.
#   2. Run a representative workload to generate profile data.
#   3. Rebuild with `-Cprofile-use=<dir>` to use the profile for
#      branch prediction, inlining, and code layout.
#
# This script does all three steps. The workload is the
# `cargo test --release` suite, which exercises the ICP hot path
# (D8 trait dispatch, NN search, SVD, ICP loop) at multiple sizes.
# The full benchmark suite is more representative but takes
# longer to run.
#
# Usage:
#   bash pgo-build.sh           # build with PGO using test workload
#   bash pgo-build.sh bench      # use the bench workload instead
#   bash pgo-build.sh clean     # remove the PGO output dirs
#
# Output:
#   target/release-pgo/        — the PGO-optimised binary
#   target/pgo-data/           — the profile data dir (intermediate)
#
# Expected speedup at the time of writing (2026-08-21): 5-15% on
# the ICP hot path. PGO is most effective when the profile data
# matches the production workload — for cc-rust's ICP, the test
# suite covers most code paths (D8 trait dispatch, NN search,
# ICP loop) but not real-data sizes. For the maximum win, profile
# against the `experimental/sessions/2026-08-20-d8-realdata-all-nns`
# binary running on the 7.5M brook-avenue scan.
#
# Platform note: PGO requires `profiler_builtins` in the rustc
# distribution. The MSVC rustc shipped with this repo (and with
# stable rustup on Windows) does NOT include it — you'll get
# `error[E0463]: can't find crate for 'profiler_builtins'` from
# the build script. Workarounds:
#   - Build a custom rustc with `profiler_builtins` (large effort).
#   - Run on Linux/macOS where the system rustc includes the
#     profiler runtime.
#   - On Windows, use the next-best opt in `[profile.release]`
#     (opt-level=3 + lto="fat" + codegen-units=1) which gives
#     ~80% of the PGO speedup without needing the profiler
#     runtime. This is what `cc-rust/Cargo.toml` is configured
#     for.
# This script is kept for future use on a host where PGO works.

set -euo pipefail

# Detect cargo. On Windows (this repo's primary dev environment)
# cargo lives at C:\Users\bramburn\.cargo\bin\cargo.exe.
if command -v cargo >/dev/null 2>&1; then
    CARGO=cargo
else
    CARGO="C:/Users/bramburn/.cargo/bin/cargo.exe"
fi

# Repo root (parent of cc-rust/).
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

PROFILE_DIR="$REPO_ROOT/target/pgo-data"
OUT_DIR="$REPO_ROOT/target/release-pgo"
mkdir -p "$PROFILE_DIR" "$OUT_DIR"

mode="${1:-test}"

if [[ "$mode" == "clean" ]]; then
    echo "Cleaning PGO artefacts..."
    rm -rf "$PROFILE_DIR" "$OUT_DIR"
    echo "Done."
    exit 0
fi

# Step 1 + 2: build with instrumentation, run a workload.
echo "=== PGO step 1/3: build with -Cprofile-generate ==="
echo "    Profile data: $PROFILE_DIR"
echo "    Output binary: $OUT_DIR"
RUSTFLAGS="-Cprofile-generate=$PROFILE_DIR" \
    "$CARGO" build --release --target-dir "$OUT_DIR/target-build"

echo
echo "=== PGO step 2/3: run workload to generate profile data ==="
if [[ "$mode" == "bench" ]]; then
    echo "    Running cargo bench --release (this takes a few minutes)..."
    RUSTFLAGS="-Cprofile-generate=$PROFILE_DIR" \
        "$CARGO" bench --release --target-dir "$OUT_DIR/target-build"
else
    echo "    Running cargo test --release (covers D8 trait dispatch,"
    echo "    NN search, ICP loop, ICP parity tests)..."
    RUSTFLAGS="-Cprofile-generate=$PROFILE_DIR" \
        "$CARGO" test --release --target-dir "$OUT_DIR/target-build"
fi

# Step 3: build with profile-use.
echo
echo "=== PGO step 3/3: rebuild with -Cprofile-use ==="
RUSTFLAGS="-Cprofile-use=$PROFILE_DIR" \
    "$CARGO" build --release --target-dir "$OUT_DIR/target-build"

echo
echo "=== PGO build complete ==="
echo "    Binary:  $OUT_DIR/target-build/release/cc_rust_cli.exe"
echo "    Profile: $PROFILE_DIR"
echo
echo "Compare the PGO build to the non-PGO build (target/release/cc_rust_cli.exe)"
echo "to measure the speedup. The ICP hot path should improve by"
echo "5-15% on real-data sizes."
