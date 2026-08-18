# docs/context/registration/transform-math.md

The math of turning N point pairs into a rigid 4×4 transform, plus how to apply it as a **preview** vs. how to **commit** it. Two distinct APIs in CloudCompare, deliberately kept separate.

---

## 1. The math: Horn registration / Eigen Umeyama

The user is picking N ≥ 3 correspondences between two point clouds: `P[i] ↔ Q[i]` (paired by insertion order).

Goal: find `R` (3×3 rotation), `T` (3-vector translation) such that for all `i`:

```
Q[i] ≈ R · P[i] + T
```

Plus optionally a uniform scale `s`. v1: **fixed scale = 1.0** (rigid only).

### 1.1 Path A — Eigen Umeyama (already used by `ccPointPairRegistrationDlg`)

In `qCC/ccPointPairRegistrationDlg.cpp` line 1240:

```cpp
static bool UmeyamaRegistration(CCCoreLib::GenericCloud* toBeAlignedPoints,
                                CCCoreLib::GenericCloud* referencePoints,
                                CCCoreLib::PointProjectionTools::Transformation& trans,
                                bool fixedScale)
{
    // Build Eigen matrices from the point pairs
    Eigen::MatrixXd srcMatrix(3, pointCount), destMatrix(3, pointCount);
    // ... fill srcMatrix from alignedPoints, destMatrix from refPoints ...

    Eigen::MatrixXd resultUmeyama = Eigen::umeyama(srcMatrix, destMatrix, /*with_scaling=*/!fixedScale);

    // trans.s, trans.R, trans.T are populated
    // ...
}
```

Eigen Umeyama solves the **orthogonal Procrustes problem** via SVD — closed-form, O(N) in the number of pairs. ~500 LoC to write from scratch, ~10 LoC if you call `Eigen::umeyama` directly.

### 1.2 Path B — `CCCoreLib::HornRegistrationTools::FindAbsoluteOrientation`

From `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h` line 105:

```cpp
class CC_CORE_LIB_API HornRegistrationTools : public RegistrationTools {
public:
    static bool FindAbsoluteOrientation(GenericCloud*       toBeAlignedPoints,
                                        GenericCloud*       referencePoints,
                                        ScaledTransformation& trans,
                                        bool                fixedScale = false);
};
```

Same algorithm under the hood (SVD-based closed form), but the CCCoreLib wrapper returns a `CCCoreLib::PointProjectionTools::Transformation` (which has `R`, `T`, `s`). This is the **right** path for a plugin — already linked, no Eigen dependency to manage.

> **Constraint:** the `CC_USES_EIGEN` compile flag in `qCC/ccPointPairRegistrationDlg.cpp:53` is conditional. If we link Eigen directly from the plugin, we need `-DCC_USES_EIGEN` and an Eigen include path. Using `CCCoreLib::HornRegistrationTools` avoids this — that's why it's there. **Use Path B for the plugin.**

### 1.3 Path C — DIY SVD

**Don't.** `Eigen::JacobiSVD` or `Eigen::BDCSVD` are ~30 lines, but we already have a tested implementation. The cost of a wrong SVD path is a silent bug that produces a 180°-rotated transform. Use the library.

---

## 2. RMS — overall and per-pair

### 2.1 Overall RMS

`CCCoreLib::RegistrationTools::ComputeRMS` (line 60 of `RegistrationTools.h`):

```cpp
static double ComputeRMS(GenericCloud* lCloud,         // source
                         GenericCloud* rCloud,         // reference (after transform)
                         const ScaledTransformation& trans);
```

Returns the per-point RMS error. Use it for the dialog's "Overall RMS" label.

### 2.2 Per-pair RMS

For each pair `i`, compute `||R·P[i] + T - Q[i]||` and store it in the table. Useful for the user to identify bad pairs.

```cpp
struct PairRMS { int index; double rms; };
QList<PairRMS> perPair;
for (int i = 0; i < n; ++i) {
    CCVector3d pred = trans.R * m_alignedPoints[i] + trans.T;
    double rms = (pred - m_referencePoints[i]).norm();
    perPair.push_back({i, rms});
}
```

Show in the dialog table as a column; highlight rows with RMS > 2× median as "bad".

---

## 3. The two-step apply: preview, then commit

This is the most important part of the doc. CloudCompare has **two** distinct APIs for transforming a drawable. They are not interchangeable:

### 3.1 Preview — `setGLTransformation` (display-only)

From `libs/qCC_db/include/ccDrawableObject.h` line 274:

```cpp
//! Associates entity with a GL transformation (rotation + translation)
/** \warning FOR DISPLAY PURPOSE ONLY (i.e. should only be temporary)
    If the associated GL transformation is enabled (see
    ccDrawableObject::enableGLTransformation), it will
    be applied before displaying this entity.
    However it will not be taken into account by any CCCoreLib algorithm
    (distance computation, etc.) for instance.
    Note: GL transformation is automatically enabled.
**/
virtual void setGLTransformation(const ccGLMatrix& trans);

virtual void enableGLTransformation(bool state);
virtual const ccGLMatrix& getGLTransformation() const;
virtual void resetGLTransformation();
```

This is what we use for the live preview:

```cpp
// In PreviewClicked()
m_aligned->setGLTransformation(glTrans);    // display-only
m_aligned->enableGLTransformation(true);    // turns it on (setGLTransformation already enables)
m_glA->redraw();                            // user sees the moved cloud
```

⚠ **Do NOT call this from anywhere except the preview flow.** If `enableGLTransformation(true)` is left on after a commit, future renders still apply the preview transform on top of the committed one — a subtle double-transform bug.

### 3.2 Commit — `applyRigidTransformation` (mutates the geometry)

From `libs/qCC_db/include/ccPointCloud.h` line 623:

```cpp
void applyRigidTransformation(const ccGLMatrix& trans) override;
```

This **physically moves the points**. The cloud's `ccBBox` is recomputed, scalar fields may be invalidated, and `notifyGeometryUpdate()` is called.

```cpp
// In ApplyClicked()
m_aligned->applyRigidTransformation(glTrans);   // <-- mutates the vertices
m_aligned->resetGLTransformation();             // <-- clear the preview
m_aligned->enableGLTransformation(false);       // <-- belt-and-braces
```

After commit, the source cloud's coordinates have changed permanently. The db-tree needs to know:

```cpp
m_app->addToDB(m_aligned, /*updateZoom*/true, /*autoExpandDBTree*/false,
               /*checkDimensions*/false);
m_app->refreshAll();    // redraw every active GL window
```

### 3.3 Why this split

| | Preview (`setGLTransformation`) | Commit (`applyRigidTransformation`) |
|---|---|---|
| Affects rendering? | Yes | Yes |
| Affects saved `.bin`? | **No** — never written | **Yes** |
| Affects distance / RMS / ICP math? | **No** — these algorithms don't see GL transforms | **Yes** |
| Reversible? | `resetGLTransformation()` | Need to keep the inverse transform yourself |
| Fast? | Trivially fast (one matrix multiply at render time) | O(N) in point count |

The split is so important that CloudCompare's own `ccPointPairRegistrationDlg` follows it:
1. Compute `trans` once.
2. Apply as `setGLTransformation` for live preview.
3. On accept, call `applyRigidTransformation(trans)`, then `resetGLTransformation()`.
4. On cancel, just `resetGLTransformation()`.

**Always** copy this pattern.

---

## 4. Conversion between `ccGLMatrix` and `CCCoreLib::Transformation`

The math API returns `CCCoreLib::PointProjectionTools::Transformation` (fields: `R` (3×3), `T` (3-vec), `s` (scalar)). The display/commit APIs take `ccGLMatrix`. Converting:

```cpp
// from CCCoreLib::PointProjectionTools::Transformation → ccGLMatrix
ccGLMatrixd glMat = ccGLMatrixd::FromDoublePrecision(trans.R, trans.T);
ccGLMatrix  glMatF(static_cast<const ccGLMatrixd&>(glMat));
// (or use ccGLMatrixd directly — both APIs accept either form)
```

Easiest: just keep the math in `ccGLMatrixd` end-to-end. Build `Eigen` matrices from your `std::vector<CCVector3d>` directly, run Umeyama, take the result and feed it to `Eigen::umeyama`-style conversion. Or call `CCCoreLib::HornRegistrationTools::FindAbsoluteOrientation`, then convert.

> **Tip:** look at `qCC/mainwindow.cpp::applyTransformation` (line 1144) — it takes a `ccGLMatrixd` and walks the db-tree applying it. That's the production-shape a commit should look like (minus the tree walk, which `applyRigidTransformation` already does for a single cloud).

---

## 5. Edge cases in the math

### 5.1 Degenerate pairs

3 collinear points → SVD finds R = identity (or its 180° mirror) but T is undefined. `FindAbsoluteOrientation` returns false in this case. Show an error and leave the dialog open.

### 5.2 Same point picked twice

Both points are at `(0,0,0)`. Umeyama with a zero-variance source matrix returns NaN. The CCCoreLib wrapper handles this (returns false). Show "Pairs must be distinct points."

### 5.3 3D points vs. global-shifted points

`ccGlobalShiftManager` shifts large coordinates (e.g. GPS UTM in mm) to local space for float precision. `CCVector3d` (double) holds the unshifted global coords; `CCVector3` (float) holds the shifted local coords. **Always convert to `CCVector3d` before computing the transform** — `FindAbsoluteOrientation` takes `GenericCloud*` whose `getNextPoint()` returns `CCVector3*` (float). If the clouds are global-shifted, the math is in the local frame, which is fine because both clouds use the same global shift logic.

### 5.4 Single-cloud vs. multi-cloud

`FindAbsoluteOrientation` expects **two clouds of the same size**, with point `i` of cloud A paired with point `i` of cloud B. We pass **synthetic** `GenericCloud`s built from the `std::vector<CCVector3d>` of picks — not the original clouds. So this concern doesn't apply.

---

## 6. Testing the math without UI

Two unit-test-friendly surfaces:

1. **Synthetic pairs.** Build a known rotation + translation, apply to a synthetic cloud, then check that `FindAbsoluteOrientation` recovers the rotation. **TODO before v1.**
2. **`ccRegistrationTools::ComputeRMS`.** Round-trip a transform; check RMS is ~0 on the synthetic input. **TODO before v1.**

The `BUILD_TESTING=ON` test target is `cc-test-lib`. Add new tests under `libs/qCC_io/test/` (or a new `libs/qCC_db/test/` directory if we can — currently there's no `qCC_db/test/`). See [`../../AGENTS-coding-standards.md`](../../../AGENTS-coding-standards.md) §10.

---

## 7. Pointers

- [`picking-system.md`](picking-system.md)
- [`dual-viewport.md`](dual-viewport.md)
- [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md)
- Source:
  - `qCC/ccPointPairRegistrationDlg.cpp` line 1240 (`UmeyamaRegistration`)
  - `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h` line 60 (`ComputeRMS`), line 105 (`HornRegistrationTools::FindAbsoluteOrientation`)
  - `libs/qCC_db/extern/CCCoreLib/include/PointProjectionTools.h` (`Transformation` struct)
  - `libs/qCC_db/include/ccDrawableObject.h` line 274 (`setGLTransformation` + family)
  - `libs/qCC_db/include/ccPointCloud.h` line 623 (`applyRigidTransformation`)
  - `libs/qCC_db/include/ccGLMatrix.h` (`ccGLMatrix` and `ccGLMatrixd`)
  - `qCC/mainwindow.cpp` line 1144 (`applyTransformation` — the production-shape commit path)
