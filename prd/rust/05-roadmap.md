# Rust Migration Roadmap — CloudCompare
**Decision: Proceed — Incremental C++ → Rust via CXX FFI**
**Date:** 2026-08-19
**Owner:** Icelabz (bramburn)
**Status:** Ready to begin Phase 0

---

## Decision Summary

**What to migrate:** CCCoreLib compute kernels only — `ScalarField`, `RegistrationTools` (ICP/Horn), `DgmOctree`, `KdTree`, then file parsers.
**What stays C++ permanently:** Qt/GUI layer (`qCC/`, `ccViewer/`), `ccPointCloud`, plugin system.
**How:** CXX FFI crate — zero-copy, compile-time safe, no big-bang rewrite.
**Why:** Primary motivation is **safety** (eliminate data races, memory leaks, dangling ptrs in hot paths). Performance is equivalent to C++ — Rust matches or slightly exceeds in real-world compute workloads.

**Go / No-Go: Go.** Authorization needed for Phase 0 only.

---

## Migration Priority

| # | Subsystem | Lines (est.) | Why Rust Here | Qt Deps | Risk |
|---|-----------|-------------|---------------|---------|------|
| **T1** | `ScalarField` + `ScalarFieldTools` | ~1,300 | Bounds safety, NaN handling, no threading | None | Low |
| **T2** | `RegistrationTools` (ICP/Horn) | ~1,500 | Data race on `m_MT_wrapper`, manual GC → RAII | None | Medium |
| **T3** | `DgmOctree` + `KdTree` | ~4,500 | Manual `new`/`delete`, `MultiThreadingWrapper` race | None | Med-High |
| **T4** | `GeometricalAnalysisTools` | ~1,000 | MT race elimination, buffer safety | None | Medium |
| **T5** | File parsers (LAS, PLY, CSV) | ~2,000 | Input validation, eliminate LASzip/shapelib deps | None | Medium |
| **DEF** | `ccPointCloud` | ~4,000 | Qt/MOC coupling too deep | Full Qt | Deferred |
| **NEVER** | `ccGLWindow` / UI / plugins | — | Qt plugin loader can't discover Rust types | Full Qt | Never |

---

## The 4-Phase Plan

```
Phase 0 ─── Setup + FFI infrastructure ───────────── Month 1–2
Phase 1 ─── ScalarField ──────────────────────────── Month 3–6
Phase 2 ─── RegistrationTools (ICP/Horn) ─────────── Month 7–12
Phase 3 ─── DgmOctree + KdTree ───────────────────── Month 13–24
Phase 4 ─── File parsers (LAS, PLY) ───────────────── Month 18–36
```

### Phase 0: Infrastructure (Month 1–2) — **Start here**

**Goal:** Build system + FFI + CI working before writing any Rust logic.

| # | Task | Owner |
|---|------|-------|
| 0.1 | Install Rust toolchain (`x86_64-pc-windows-msvc`) | User |
| 0.2 | Create `cc-rust/` Cargo workspace with `Cargo.toml` + `build.rs` | Agent |
| 0.3 | Add CXX bridge stub — empty `#[cxx::bridge]` that compiles | Agent |
| 0.4 | Integrate `cc_rust_lib` custom target into `tools/cc-configure.cmd` | Agent |
| 0.5 | Verify CloudCompare.exe launches without crash | User |
| 0.6 | Run ASAN across FFI boundary — no false positives | User |
| 0.7 | Add `cargo test` to CI alongside existing C++ tests | Agent |

**Deliverable:** Hybrid C++/Rust binary that builds and runs. No Rust logic yet.

**What this rules out early:** If the CMake + Cargo + CXX pipeline can't be made to work on Windows + MSVC + Ninja in month 1–2, the whole plan stops here with minimal sunk cost.

---

### Phase 1: ScalarField (Month 3–6) — **First Rust logic**

**Goal:** Replace `ScalarField.cpp` + `ScalarFieldTools.cpp` with Rust behind `ENABLE_RUST_SCALAR_FIELD`.

| # | Task | Owner |
|---|------|-------|
| 1.1 | Write characterisation tests: capture C++ mean/std/min/max results | Agent |
| 1.2 | Implement `rust_scalar_mean`, `rust_scalar_std`, `rust_scalar_min_max` in Rust | Agent |
| 1.3 | Wire via CXX bridge — one method at a time, CI green each step | Agent |
| 1.4 | Verify parity with C++ to `1e-6` on characterisation corpus | User |
| 1.5 | Add `ENABLE_RUST_SCALAR_FIELD` CMake feature flag | Agent |
| 1.6 | Run full `TestScalarField` suite with Rust enabled | User |

**CXX bridge (Phase 1):**
```rust
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn rust_scalar_mean(values: &[f32]) -> f64;
        fn rust_scalar_std(values: &[f32], mean: f64) -> f64;
        fn rust_scalar_min_max(values: &[f32]) -> (f32, f32);
        fn rust_scalar_valid_count(values: &[f32]) -> usize;
    }
}
```

**Rust exit criteria:**
- All 12 existing `TestScalarField` tests pass
- Rust/C++ parity to `1e-6`
- ASAN/TSAN clean across FFI boundary
- Performance within ±10% of C++

**Rollback:** Set `ENABLE_RUST_SCALAR_FIELD=OFF` — C++ version is unchanged.

---

### Phase 2: RegistrationTools — ICP/Horn (Month 7–12)

**Goal:** Replace `RegistrationTools.cpp` + `PointProjectionTools.cpp`. The highest-value Rust target — eliminates the `MultiThreadingWrapper` data race.

| # | Task | Owner |
|---|------|-------|
| 2.1 | Audit `ICP::DataCloud` / `ModelCloud` structs for raw pointers — document which are `Send`/`Sync` | Agent |
| 2.2 | Implement Horn registration (SVD-based) in Rust | Agent |
| 2.3 | Implement ICP iteration loop in Rust (Rayon for parallelism) | Agent |
| 2.4 | Replace `Garbage<>` collector with Rust's `Drop` + arena allocator | Agent |
| 2.5 | Characterisation: ICP result quality matches C++ on benchmark cloud pairs | User |
| 2.6 | Add `ENABLE_RUST_REGISTRATION` CMake feature flag | Agent |

**Rust architecture:**
```rust
pub struct IcprContext {
    model_kdtree: kdtree::KdTree<3, f32, ()>,
    data_points: Vec<[f32; 3]>,
    params: IcprParams,
    transform: nalgebra::Isometry3<f64>,
}
// ICP step: find neighbours → compute transform (SVD) → apply → compute RMS → check convergence
```

**Rollback:** `ENABLE_RUST_REGISTRATION=OFF` — original C++ registration.

---

### Phase 3: DgmOctree + KdTree (Month 13–24)

**Goal:** Replace manual `new`/`delete` KD-tree with `Box<KdTreeCell>`. Replace `MultiThreadingWrapper` races with Rust atomics.

| # | Task | Owner |
|---|------|-------|
| 3.1 | Implement KD-tree in Rust (`Box<KdTreeCell>` replaces `new`/`delete`) | Agent |
| 3.2 | Replace `NeighboursSet` dynamic allocation with `SmallVec<[usize; 27]>` | Agent |
| 3.3 | Implement octree build in Rust — verify Morton code parity with C++ | Agent |
| 3.4 | Replace TBB/QtConcurrent threading with Rayon | Agent |
| 3.5 | Validate Rayon results against TBB on 5+ real scan datasets | User |
| 3.6 | Replace `m_buildInProgress` atomic state machine with Rust `AtomicI32` + enum | Agent |
| 3.7 | Add `ENABLE_RUST_OCTREE` CMake feature flag | Agent |

**Safety fix — this bug goes away:**
```cpp
// C++: data race (confirmed)
struct MultiThreadingWrapper {
    bool cellFuncSuccess = true;  // ← written by threads, read by main = RACE
};
// Rust: compile-time enforced — shared state requires Arc<Mutex<T>>
```

**Rollback:** `ENABLE_RUST_OCTREE=OFF`.

---

### Phase 4: File I/O Parsers (Month 18–36)

**Goal:** Replace C++ parsers with Rust crates. Eliminates LASzip and shapelib C++ dependencies.

```
4a: CSV scalar field import/export        → pure Rust, 1 month
4b: PLY parser replacement (ply-rs)       → CXX bridge: Vec<u8> → ccPointCloud, 3 months
4c: LAS/LAZ parser (las-rs / laz-perf)    → eliminates LASzip dependency, 6 months
```

**Why last:** Parser FFI is complex (ccPointCloud is deeply coupled). File I/O is not a safety risk in the same way that a data race during ICP iteration is.

---

## Build System Layout

```
CloudCompare/
├── CCCoreLib/                      ← C++ lib (unchanged)
├── cc-rust/                        ← NEW Rust library
│   ├── Cargo.toml                  ← rayon, nalgebra, kdtree, cxx
│   ├── build.rs                    ← cxx_build::bridge("src/lib.rs").compile()
│   └── src/
│       ├── lib.rs                  ← FFI bridge
│       ├── scalar_field.rs         ← Phase 1
│       ├── registration.rs         ← Phase 2
│       ├── octree/                 ← Phase 3
│       │   ├── mod.rs
│       │   ├── dgm_octree.rs
│       │   └── kdtree.rs
│       └── io/                     ← Phase 4
│           └── las.rs
└── tools/
    └── cc-configure.cmd             ← add Rust toolchain + cc_rust_lib target
```

CMake integration: `cc_rust_lib` is a custom target that runs `cargo build --release`. It is a dependency of `CCCoreLib`. CloudCompare.exe links `libcc_rust.a`.

---

## Timeline Summary

| Phase | Duration | Total elapsed | Key deliverable |
|-------|----------|---------------|-----------------|
| Phase 0: Infrastructure | 1–2 months | Month 2 | Hybrid binary builds and runs |
| Phase 1: ScalarField | 3–4 months | Month 6 | Rust ScalarField behind flag |
| Phase 2: Registration | 4–6 months | Month 12 | Rust ICP behind flag |
| Phase 3: Octree/KDTree | 6–9 months | Month 24 | Rust spatial index behind flag |
| Phase 4: Parsers | 9–18 months | Month 36 | Rust file I/O behind flag |

**Total: 18–36 months.** Phases overlap — while Phase 1 is being validated in month 6, Phase 2 scaffolding can begin.

---

## Rollback Mechanism (Every Phase)

Every phase ships behind a CMake feature flag. If Rust is broken, flip the flag and the original C++ is unchanged:

```cmake
option(ENABLE_RUST_SCALAR_FIELD "Use Rust implementation" OFF)
option(ENABLE_RUST_REGISTRATION  "Use Rust implementation" OFF)
option(ENABLE_RUST_OCTREE        "Use Rust implementation" OFF)
```

The C++ implementation is **never deleted** — it remains the fallback as long as CloudCompare ships.

---

## What Can Go Wrong

| Risk | Likelihood | Mitigation |
|------|-----------|-----------|
| CXX + MSVC + Ninja integration fails | Low | Phase 0 validates this in month 1 |
| Rust ICP gives different results than C++ | Medium | Characterisation tests before migration |
| Performance regression in Rust | Low | Benchmarks in CI; ±10% gate |
| Rayon results diverge from TBB results | Medium | 5-dataset validation before Phase 3 merge |
| FFI boundary too wide (ownership errors) | Low-Medium | CXX enforces at compile time |
| C++ compiler / Rust compiler ABI mismatch | Low | CXX handles this; test in Phase 0 |

---

## First Action

**Authorize Phase 0.** The only upfront commitment is:
1. Install Rust (`rustup default stable-msvc` on Windows)
2. Let the agent scaffold `cc-rust/`

If the FFI pipeline doesn't work on this toolchain, the report stops here. If it works, you have a working hybrid binary in month 2 and a clear path to eliminating the confirmed data races in ICP and octree build.

---

## Source Reports

All analysis behind this roadmap is in `PRD/rust/`:

```
PRD/rust/
├── README.md                     ← index + executive summary
├── 01-code-analysis/            ← per-subsystem C++ analysis
├── 02-migration/
│   ├── 01-strategy.md           ← methodology + case studies
│   ├── 02-phases.md             ← detailed phase specs
│   └── 03-ffi-cxx.md            ← CXX FFI deep-dive
├── 03-research/
│   ├── 01-benchmarks.md         ← Rust vs C++ performance
│   └── 02-case-studies.md       ← Discord, Dropbox, Cloudflare, ClickHouse
└── 04-swarm-reports/            ← parallel agent analysis
    ├── swarm-01-memory.md
    ├── swarm-02-concurrency.md
    ├── swarm-03-fp-math.md
    ├── swarm-04-io-parse.md
    └── swarm-05-plugin-api.md
```
