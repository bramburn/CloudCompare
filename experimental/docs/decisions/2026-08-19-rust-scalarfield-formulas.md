# D1 Detail — CCCoreLib ScalarField formulas (Rust port)

**Status:** Verified — `cargo test` in
`sessions/2026-08-19-rust-migration-icp-scalarfield/` shows 28/28
tests passing including characterisation tests.

**Source code:** `sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`

## The formulas (with line numbers in the C++ source)

### 1. `ScalarField::computeMeanAndVariance`

**File:** `libs/qCC_db/extern/CCCoreLib/src/ScalarField.cpp:65-95`

```cpp
void ScalarField::computeMeanAndVariance(ScalarType& mean, ScalarType* variance) const {
    double _mean = 0.0;
    double _std2 = 0.0;
    std::size_t count = 0;

    for (std::size_t i = 0; i < size(); ++i) {
        float val = at(i);
        if (std::isfinite(val)) {                   // ← ValidValue
            _mean += val;
            _std2 += static_cast<double>(val) * val; // ← Σx²
            ++count;
        }
    }

    if (count) {
        _mean /= count;                              // ← mean = Σx/n
        mean = _mean;

        if (variance) {
            _std2 = std::abs(_std2 / count - _mean*_mean);  // ← var = Σx²/n − mean²
            *variance = static_cast<ScalarType>(_std2);
        }

        mean += m_offset;
    }
}
```

**Rust port:**

```rust
pub fn mean(values: &[f32]) -> f64 {
    let mut sum = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            sum += v as f64;
            count += 1;
        }
    }
    if count == 0 { std::f64::NAN } else { sum / count as f64 }
}

pub fn std(values: &[f32], mean: f64) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            let d = v as f64 - mean;
            sum_sq += d * d;
            count += 1;
        }
    }
    // Population std — NO Bessel correction (no (n-1) divisor)
    if count == 0 { 0.0 } else { (sum_sq / count as f64).sqrt() }
}
```

### 2. `ValidValue`

**File:** `libs/qCC_db/extern/CCCoreLib/include/ScalarField.h:100`

```cpp
static inline bool ValidValue(ScalarType value) { return std::isfinite(value); }
```

**Rust port:** `value.is_finite()`.

**Important:** `NAN_VALUE = std::numeric_limits<ScalarType>::quiet_NaN()`.
It is **real IEEE NaN**, not a sentinel like `-1.0e-30`. See
`libs/qCC_db/extern/CCCoreLib/include/CCConst.h:54`.

### 3. `computeMeanSquareScalarValue`

**File:** `libs/qCC_db/extern/CCCoreLib/src/ScalarFieldTools.cpp:753-777`

```cpp
ScalarType ScalarFieldTools::computeMeanSquareScalarValue(GenericCloud* theCloud) {
    double meanValue = 0.0;
    unsigned count = 0;
    for (unsigned i=0; i<theCloud->size(); ++i) {
        ScalarType V = theCloud->getPointScalarValue(i);
        if (ScalarField::ValidValue(V)) {
            double Vd = static_cast<double>(V);
            meanValue += Vd*Vd;     // ← Σx²
            ++count;
        }
    }
    return (count ? static_cast<ScalarType>(meanValue/count) : 0);
    //                            ↑ mean of squares, NOT RMS
}
```

**Rust port:**

```rust
pub fn rms(values: &[f32]) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            sum_sq += (v as f64) * (v as f64);
            count += 1;
        }
    }
    if count == 0 { std::f64::NAN } else { (sum_sq / count as f64).sqrt() }
    //                                              ↑ RMS = sqrt(mean of squares)
}
```

## Verification table

| Test | Input | Expected | Got | Pass? |
|---|---|---|---|---|
| `test_std_basic` | `[2,4,4,4,5,5,7,9]` | std = 2.0 (population) | 2.0 | ✅ |
| `test_rms` | `[3, 4]` | sqrt(12.5) ≈ 3.5355 | 3.5355 | ✅ |
| `parity_mean_with_nan_cloudcompare` | `[1, NaN, 3]` | mean = 2.0 | 2.0 | ✅ |
| `test_mean_with_nan` | `[1, NaN, 3, inf, 5]` | mean = 3.0 | 3.0 | ✅ |
| `parity_mean_known` | `[1, 2, 3, 4, 5]` | mean = 3.0 | 3.0 | ✅ |

## Why Bessel correction was wrong

Rust's default convention is sample std (`÷n-1`). Statistics textbooks
teach this as "Bessel's correction". But CCCoreLib uses population std
(`÷n`). Two reasons it's the right call here:

1. **CCCoreLib is the reference.** Porting a library means matching
   its behaviour, not "improving" it. If the maintainers wanted
   Bessel correction, they'd have it.
2. **It matters for small samples.** The `[2,4,4,4,5,5,7,9]` test
   has only 8 values. Population gives 2.0 exactly; sample gives
   2.138. Downstream code (e.g. M3C2 distances) depends on the
   exact value.

## Related

- Top-level decision: [`../decisions.md#d1`](../decisions.md#d1)
- Originating session: [`../../sessions/2026-08-19-rust-migration-icp-scalarfield/`](../../sessions/2026-08-19-rust-migration-icp-scalarfield/)
- Pattern: [`../patterns.md#p1`](../patterns.md#p1)
