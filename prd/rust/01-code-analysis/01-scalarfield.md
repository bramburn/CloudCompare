# Code Analysis 01: ScalarField
**Subsystem:** `libs/qCC_db/extern/CCCoreLib/src/ScalarField.cpp`
**Phase 1 Migration Target**
**Date:** 2026-08-19

---

## 1. Class Overview

```cpp
class ScalarField : public std::vector<ScalarType>  // ScalarType = float
{
    std::string m_name;
    double m_offset;
    bool m_offsetHasBeenSet;
    double m_localMinVal;
    double m_localMaxVal;
};
```

**Key observations:**
- Inherits from `std::vector<float>` — slicing risk, but `std::vector` is well-behaved
- No thread safety — concurrent reads/writes constitute a data race
- `m_offset` is a floating-point shift applied to all values (for numerical stability in large datasets)
- `m_localMinVal` / `m_localMaxVal` are cached per-operation, not updated on every write

---

## 2. Memory Safety Issues

### Issue 1: Inheritance from std::vector

**Risk:** MEDIUM

```cpp
// Client code that may slice the ScalarField:
void ProcessScalarField(std::vector<float>* sf) {
    // If passed a ScalarField*, this works fine.
    // If passed a raw pointer to the underlying vector, also works.
    // But:
    ScalarField* sf_ptr = new ScalarField("Intensity");
    std::vector<float>* base_ptr = sf_ptr;  // OK
    // Copying through base_ptr slices off name/offset:
    std::vector<float> copy = *base_ptr;  // copies only the float data
}
```

**Rust fix:** No inheritance. `ScalarField` is a struct with explicit fields:

```rust
pub struct ScalarField {
    values: Vec<f32>,
    name: String,
    offset: f64,
    offset_has_been_set: bool,
    local_min: f64,
    local_max: f64,
}
```

### Issue 2: Offset Arithmetic Without Overflow Guard

**Risk:** MEDIUM

```cpp
// ScalarField.cpp — offset arithmetic:
void ScalarField::setOffset(double offset) {
    if (!m_offsetHasBeenSet) {
        for (auto& v : *this)
            v -= static_cast<ScalarType>(offset);  // v = v - offset
        m_offset = offset;
        m_offsetHasBeenSet = true;
    }
}

// Problem: subtraction on float can lose precision for very large offsets
// No guard for NaN/Inf offset values
```

**Rust fix:**
```rust
impl ScalarField {
    pub fn set_offset(&mut self, offset: f64) {
        if !self.offset_has_been_set {
            for v in &mut self.values {
                *v = (*v - offset as f32);  // same logic, explicit
            }
            self.offset = offset;
            self.offset_has_been_set = true;
        }
    }
}
```

### Issue 3: Iterator-Based Access Without Bounds Checking

**Risk:** LOW (std::vector already bounds-checks in debug mode)

```cpp
// ScalarFieldTools.cpp:
for (size_t i = 0; i < sf->size(); ++i) {
    ScalarType val = sf->at(i);  // .at() checks bounds in debug
    if (isfinite(val)) { ... }
}
```

**Rust fix:** `values[i]` in release is unchecked. In debug, `.get(i)` returns `Option<&T>`. Use `.iter()` for safe iteration:

```rust
for val in &self.values {
    if val.is_finite() { ... }
}
```

---

## 3. ScalarFieldTools Methods to Migrate

| Method | Description | Rust Equivalent | Complexity |
|--------|-----------|---------------|-----------|
| `ComputeMean` | Iterate + sum / count | `iter().sum::<f32>() / n as f64` | LOW |
| `ComputeStdDev` | Mean + variance + sqrt | `iter().map(|x| (x-mu).powi(2)).sum::<f32>() / n as f64` | LOW |
| `ComputeMinMax` | Linear scan for min/max | `iter().fold((f32::MAX, f32::MIN), ...)` | LOW |
| `Compute Kurtosis` | 4th moment / variance² | manual loop | LOW |
| `CropToRange` | Set values outside range to NaN | `values.iter_mut().map(|v| if !in_range(v) { NaN } else { *v })` | LOW |
| `setColorScheme` | GUI colour scale | **Keep in C++** | N/A |
| `renameField` | String rename | trivial | LOW |
| `abs` | In-place absolute value | `for v in &mut values { *v = v.abs() }` | LOW |

---

## 4. FFI Contract

```cpp
// C++ calling Rust (ScalarFieldTools.cpp after migration):
double ScalarField::computeMean() const {
    return rust_scalar_mean(data(), size());  // data() = float* pointer, size() = count
}
```

```rust
// Rust receiving CXX-converted slice:
#[no_mangle]
pub extern "C" fn rust_scalar_mean(values: &[f32]) -> f64 {
    let valid: f64 = values.iter()
        .filter(|v| v.is_finite())
        .map(|v| *v as f64)
        .sum();
    let count = values.iter().filter(|v| v.is_finite()).count() as f64;
    if count == 0.0 { std::f64::NAN } else { valid / count }
}
```

**CXX automatically converts `std::vector<float>&` → `&[f32]`. Zero-copy.**

---

## 5. NaN Validity Pattern

CloudCompare uses `NAN` as the sentinel for "no data" in scalar fields. This is a common pattern in scientific computing.

```cpp
// C++ validity check:
#define VALID_VALUE(v) std::isfinite(v)
if (!VALID_VALUE(sf->at(i))) { continue; }
```

```rust
// Rust validity check:
fn is_valid(v: f32) -> bool {
    v.is_finite()  // NaN != NaN, so this works correctly
}

// Better: use Option<f32> with None representing invalid
// But this changes the data layout — characterise before changing
```

---

## 6. Test Vector Corpus

Before migration, generate characterisation test vectors:

```python
# Generate characterisation corpus for ScalarField parity tests
import numpy as np
import struct, json

test_cases = []

# Case 1: Sequential floats
vals = np.linspace(-1000, 1000, 10000).astype(np.float32)
test_cases.append({"name": "sequential", "values": vals.tolist(), "expected_mean": float(vals.mean())})

# Case 2: Random with NaN
vals = np.random.randn(10000).astype(np.float32)
vals[::100] = np.nan  # 1% invalid
test_cases.append({"name": "random_with_nan", "values": vals.tolist()})

# Case 3: All valid (worst case)
vals = np.full(1000000, 1.5, dtype=np.float32)
test_cases.append({"name": "uniform", "values": vals.tolist()})

# Case 4: Edge values
vals = np.array([0.0, -0.0, 1e-38, 1e38, np.nan, np.inf, -np.inf], dtype=np.float32)
test_cases.append({"name": "edge_values", "values": vals.tolist()})

with open("test_corpus/scalar_field.json", "w") as f:
    json.dump(test_cases, f, indent=2)
```

Run both C++ and Rust implementations against this corpus and compare outputs.
