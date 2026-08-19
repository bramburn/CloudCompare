# AGENTS_REGISTRATION.md — Goal: Manual Point-Cloud Registration

**Feature goal:** port Faro Scene Classic-style **manual point-pair registration** into CloudCompare. **Current scope: single-viewport first (Phase 1); dual-synchronized viewports are Phase 2** (see §1c below and decision R15 from the decision matrix).

> ⚠️ **Decision R15 locked (2026-08-19):** Dual-viewport registration is deferred to Phase 2. Phase 1 implements single-viewport point-pair picking → transform → apply (the existing CC `ccPointPairRegistrationDlg` workflow, but as a plugin). This is because qScanBrowser (bubble view) is the higher-priority Phase 1 item and the bidirectional picking in qScanBrowser is the real dual-viewport bridge.

**Relationship with qScanBrowser:** qScanBrowser (PRD: `PRD/scan-view/PRD.md`) and qManualRegistration share the **bidirectional 3D ↔ bubble picking** infrastructure (F16 in qScanBrowser Phase 2). Once bubble view is working, those picked points feed into the registration workflow. Do not conflate the two — they are separate plugins with a shared dependency on the picking system.

> **Status:** goal & design only. No plugin has been created yet. This document + [`docs/context/registration/`](docs/context/registration/) exist so that any AI agent (or human) picking up the task has the full picture without re-deriving it.

---

## 0. How to use this file

This is the **goal-level doc**. It states what we're building, why, and the constraints. Deeper material lives in the layered context docs:

| File | What it covers |
|---|---|
| [`docs/context/registration/README.md`](docs/context/registration/README.md) | Layered context overview + file map (Phase A/B/C of the analysis) |
| [`docs/context/registration/data-flow.md`](docs/context/registration/data-flow.md) | Full data flow from selection to committed transform |
| [`docs/context/registration/picking-system.md`](docs/context/registration/picking-system.md) | Cross-window picking architecture (`ccPickingHub`, `ccPickingListener`, `POINT_PICKING`) |
| [`docs/context/registration/dual-viewport.md`](docs/context/registration/dual-viewport.md) | Dual `ccGLWindow` lifecycle via `ccMainAppInterface::createGLWindow()` |
| [`docs/context/registration/transform-math.md`](docs/context/registration/transform-math.md) | Horn / Umeyama math + the preview-vs-commit split |

The plugin recipe that this feature should ultimately follow is in [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) — **make it a plugin, not a `qCC/` core change**.

---

## 1. Why this is hard

A LLM agent given "build a Faro Scene-style manual registration" as a one-shot prompt will produce broken C++. The four reasons:

1. **Five subsystems at once** — UI (split view), OpenGL (two windows), picking (cross-window events), registration math (Horn/SVD), data model (transform application + undo). CloudCompare's existing registration work spans **12+ files**; the plugin surface for it is ~6 files.
2. **The math is solved** — but only as a *side effect* of the existing `ccPointPairRegistrationDlg`. An agent that doesn't read that file first will reinvent the wheel badly.
3. **Picking conflicts with selection** — `ccPickingHub` is a singleton-ish. Two listeners on two windows means you have to route events by source, not by global state.
4. **Preview must not mutate the scene graph** — `setGLTransformation` is the right hook; `applyRigidTransformation` is the wrong hook (until commit). An agent that conflates them will corrupt the db-tree.

This document + the five context files pin down all four so the agent doesn't have to discover them by code-archaeology.

---

## 2. Scope

### Phase 1 — Single viewport (current)

| In scope | Out of scope |
|---|---|
| A **new Standard plugin** (`qManualRegistration`) that opens a dialog and lets the user pick ≥3 point pairs from one cloud against a reference. | Replacing `ccPointPairRegistrationDlg` (single-window flow stays untouched). |
| Single viewport: pick points on the source cloud. | Dual synchronized viewports (Phase 2). |
| Live preview of the rigid transform applied to the source cloud (via `setGLTransformation`). | ICP refinement (use existing qPCL/ICP path for that). |
| Computing the rigid transform from pairs (Horn / Eigen Umeyama). | Scale-aware / non-rigid registration. |
| Final commit via `applyRigidTransformation` + `m_app->addToDB`. | Undo stack (deferred — see §6). |
| RMS error per-pair and overall. | Global-shift handling for very large coordinates. |
| CLI mode (optional — `ccCommandLineInterface::Command`). | Texture / colour-aware picking. |

### Phase 2 — Dual synchronized viewports (deferred)

| In scope | Depends on |
|---|---|
| Two `ccGLWindow`s side-by-side (source left, reference right). | qScanBrowser Phase 2 (bidirectional picking). |
| Cross-window picking: pick in source → pair with reference point. | `ccPickingHub` multi-listener pattern (§4). |
| Synchronized camera: rotate one → other follows (optional polish). | Phase 1 stable. |

---

## 3. Architecture constraints (don't deviate)

1. **Must be a Standard plugin** at `plugins/core/Standard/qManualRegistration/` (or similar name). Do NOT modify `qCC/` or `libs/` for the v1.
2. **Must use `ccMainAppInterface::createGLWindow(...)`** to own its two `ccGLWindow`s (it returns `ccGLWindowInterface*` + `QWidget*`). Do NOT touch `qCC/mainwindow.cpp`'s `m_mdiArea`. The plugin is a *consumer* of the app, not a modifier.
3. **Must register two `ccPickingListener`s** with `ccPickingHub` — one per GL window — and disambiguate picks by listener pointer. Do NOT install global state on the hub.
4. **Must use `ccDrawableObject::setGLTransformation(...)` for the live preview**, then `resetGLTransformation()` on cancel. Do NOT mutate the scene graph until commit.
5. **Must commit via `ccPointCloud::applyRigidTransformation(trans)` + `m_app->addToDB(...)`** on Apply. Do NOT skip the db-tree update.
6. **Must compute the rigid transform via `CCCoreLib::HornRegistrationTools::FindAbsoluteOrientation(...)`** or the Eigen Umeyama path already in `qCC/ccPointPairRegistrationDlg.cpp::UmeyamaRegistration`. Do NOT roll your own SVD.
7. **Must use `ccOverlayDialog` as the dialog base** (so it integrates with the host's MDI overlay system if we later decide to migrate to the overlay model).
8. **Must NOT block the UI thread** during transform computation or preview refresh — CloudCompare has `ccProgressDialog` for this; the existing `ccPointPairRegistrationDlg` uses it.
9. **License header** must be the GPL template on every new `.h/.cpp` (the plugin links into `qCC`).
10. **IID must be unique**: `ccorp.cloudcompare.plugin.qManualRegistration`.

---

## 4. The five related areas & their canonical references

| Area | Canonical reference (read these end-to-end first) | What we copy |
|---|---|---|
| **Math (rigid transform from N pairs)** | `qCC/ccPointPairRegistrationDlg.cpp::UmeyamaRegistration` (Eigen Umeyama path) + `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h` (`HornRegistrationTools::FindAbsoluteOrientation`) | The algorithm + RMS computation |
| **Picking (single window today)** | `qCC/ccPointPairRegistrationDlg.cpp` (whole file) — inherits `ccPickingListener`; registers with `ccPickingHub` via `m_pickingHub->addListener(this, true, true, ccGLWindowInterface::POINT_PICKING)` | The listener pattern |
| **Dialog UI** | `qCC/ui_templates/pointPairRegistrationDlg.ui` (single window) | The pair table + buttons; we add a *second* viewport widget |
| **GL window creation (plugin side)** | `libs/CCPluginAPI/include/ccMainAppInterface.h` (lines 80-99: `createGLWindow` / `destroyGLWindow`) | The API |
| **Transform application (commit)** | `qCC/mainwindow.cpp::applyTransformation` (line 1144) and `libs/qCC_db/include/ccPointCloud.h` line 623 (`applyRigidTransformation`) | The production path |
| **Preview transform (display-only)** | `libs/qCC_db/include/ccDrawableObject.h` lines 274-300 (`setGLTransformation`, `enableGLTransformation`, `resetGLTransformation`) | The non-mutating path |
| **Cross-window coordination** | `libs/CCPluginAPI/include/ccPickingHub.h` (multiple `addListener`s + `togglePickingMode`) | The pattern; we add two listeners |
| **Dual-viewport UI pattern** | `qCC/pluginManager/ccPluginUIManager.cpp` (creates menus + toolbars for plugins — orthogonal model) | **Closest precedent** — we adapt the "create UI elements owned by a plugin" pattern |

---

## 5. Testable milestones

Each milestone is a verification target. An agent should not start the next milestone until the previous is green.

| # | Milestone | Verification |
|---|---|---|
| **M1** | Dialog opens with two cloud selectors (source / target), no viewports yet. | Build, run, open Tools → Manual Registration; the dialog opens; selecting two clouds enables an "Open Viewports" button. |
| **M2** | Two `ccGLWindow`s appear in a QSplitter inside the dialog, one per cloud. | Both clouds render; the splitter resizes; closing the dialog destroys both windows (no leaks in `valgrind` / VLD). |
| **M3** | Clicking in the source viewport captures a `CCVector3d` and adds it to the pair list as "aligned (A)"; clicking in the target viewport adds it as "reference (B)"; pairs auto-pair by insertion order. | 3+ pairs produce a populated table; the table is editable; "Remove" deletes a pair. |
| **M4** | A "Preview" button (≥3 pairs required) computes the rigid transform and applies it as a `setGLTransformation` to the source cloud; the source viewport redraws the moved cloud live; RMS per-pair + overall shown. | Source cloud visibly moves in its viewport; RMS decreases as pairs improve; toggle off returns the cloud to its original pose (no scene-graph mutation). |
| **M5** | "Apply" commits the transform via `applyRigidTransformation` + `m_app->addToDB(...)`. The source cloud is permanently moved; the dialog closes; the db-tree shows the moved entity. | Restart CloudCompare, load the saved `.bin` file — the source cloud is at its new position (transform was committed, not just previewed). |
| **M6** | CLI mode: a new `-MANUAL_REGISTER` command works. | `CloudCompare -MANUAL_REGISTER -A cloudA.bin -B cloudB.bin -PA 100,200,300 …` performs the registration and saves a transformed `.bin`. |
| **M7** | `qManualRegistration` appears in **Help → About → Plugins** with the right metadata. | Yes. |
| **M8** | CI parity: add `-DPLUGIN_STANDARD_QMANUAL_REGISTRATION=ON` to `.github/workflows/build.yml` (all 4 jobs: Windows MSVC, macOS Clang, Ubuntu GCC, Ubuntu Clang). | CI green. |

A "green" milestone is one where the build is clean (`cmake --build build --target check-format` returns 0), the plugin loads, the milestone's verification passes, and the previous milestones still pass.

---

## 6. Out of v1 — deferred / aspirational

- **Undo support.** CloudCompare doesn't currently have a per-action undo stack for transforms (the `ccPointPairRegistrationDlg` doesn't either). Adding one is a *separate* feature spanning `qCC/mainwindow.cpp` and `ccHObject`.
- **Scale-aware registration.** The Umeyama path returns scale; we throw it away in v1.
- **Texture / colour-aware picking.** Useful for scan-vs-scan registration but adds complexity to the picking listener (need to query RGB at picked point).
- **Multi-resolution picking.** Pick from an octree at lower resolution for speed. Orthogonal to dual-viewport.
- **Synchronized camera views** (pan/rotate one view → the other follows). Useful for context but not strictly needed for picking.
- **Hotkey-driven pair picking** (e.g. "A" to pick in source, "B" to pick in target).

---

## 7. Risk register

| Risk | Probability | Impact | Mitigation |
|---|---|---|---|
| `ccPickingHub` only supports one listener per window reliably | Medium | High | Use a single `ccPickingListener` that owns both windows and disambiguates via `m_associatedWin`; fall back to manual `setPickingMode` if the hub can't handle two modes at once. |
| Plugin can't create `ccGLWindow` (the default `createGLWindow` in `CCPluginAPI` returns null) | Low | High | The implementation in `qCC/mainwindow.cpp::createGLWindow` is the actual provider — it works in GUI mode. Document the CLI-mode limitation (v1 is GUI-only). |
| Live preview thrashes the renderer on big clouds | Medium | Medium | Apply the preview transform via `setGLTransformation` (cheap), and only redraw on user action, not per frame. |
| Eigen Umeyama is brittle to bad pairs (one outlier pair wrecks the transform) | Medium | Medium | Show per-pair RMS in red; allow re-ordering or removing pairs. Document the outlier issue in the dialog help. |
| `addToDB` clobbers existing selection / camera | Low | Medium | Capture and restore `ccGLWindow`'s viewport parameters around the apply. |

---

## 8. Decision: plugin vs core

| Option | Pros | Cons |
|---|---|---|
| **Standard plugin** ✅ chosen | Zero changes to `qCC/` / `libs/`; reviewable as one PR; can be upstreamed as a plugin (mirrors `qCompass`, `qM3C2`); uses `createGLWindow` to own its own viewports. | Limited to what `ccMainAppInterface` exposes (today: enough). |
| Hybrid (plugin + minimal core hook in `MainWindow`) | Could expose a richer "dedicated-viewport" API to plugins. | Modifies `qCC/mainwindow.h` — every plugin that builds against the API would need to be aware. Not worth it for v1. |
| Pure core change in `qCC/` | Most control over the MDI area. | Touches the file the user said not to modify; review burden is huge; can't be upstreamed as a plugin. |

**Recommendation:** start as a Standard plugin. If `ccMainAppInterface` proves insufficient for some reason (e.g. we need cross-plugin event broadcasting), promote the missing surface area to a new method on the interface (bump `Q_DECLARE_INTERFACE` version) — but don't touch `qCC/` for v1.

---

## 9. Open questions (to resolve before M3)

These don't block the feature but should be settled during implementation:

1. **Plugin name:** `qManualRegistration`, `qDualRegistration`, `qPairAlign`? — pick one and stick to it; affects `IID` (`ccorp.cloudcompare.plugin.<Name>`), folder name, CMake `option()` flag.
2. **Where do the picked `CCVector3` coordinates live** — stored on the dialog as `std::vector<CCVector3d>` for aligned, another for reference, or stored as `cc2DLabel`s inside each cloud (the existing pattern)? Both work; labels are visible to the user.
3. **Should the source viewport be allowed to show the registered point pairs** (i.e. draw lines / spheres at each pair's coordinates)? — improves UX, costs GL state.
4. **Should the plugin also export its current pair set to a text file** so the user can re-load the same pairs across sessions? — nice-to-have.
5. **Do we want a "save pair set to file" / "load pair set from file"** as a v1.5 feature? — see §6.

---

## 10. Pointers

- **Project root** — [`AGENTS.md`](AGENTS.md) (canonical entry point for any agent)
- **Architecture** — [`AGENTS-architecture.md`](AGENTS-architecture.md) §3 (plugin model), §4 (`ccHObject`)
- **Plugin recipe** — [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) §1.1 (Standard plugin template)
- **Library ownership** — [`AGENTS-libs.md`](AGENTS-libs.md) (where `ccGLMatrix`, `ccPickingHub`, `ccMainAppInterface`, `ccOverlayDialog`, `FileIOFilter` live)
- **UI patterns** — [`AGENTS-ui.md`](AGENTS-ui.md) §4 (`ccOverlayDialog`), §5 (db-tree)
- **Coding standards** — [`AGENTS-coding-standards.md`](AGENTS-coding-standards.md) (file headers, naming, Qt conventions)
- **Existing single-window reference** — `qCC/ccPointPairRegistrationDlg.{h,cpp}` + `qCC/ui_templates/pointPairRegistrationDlg.ui` (read end-to-end before writing the plugin)
- **Existing dialog UI for RMS/error table** — `qCC/ui_templates/comparisonDlg.ui` + `qCC/ccComparisonDlg.{h,cpp}`
- **Layered context** — [`docs/context/registration/`](docs/context/registration/)

---

## 11. Phase-by-phase agent prompts (copy-pasteable)

The five prompts below are the **copy-pasteable instructions** to drive an LLM agent through reconnaissance → PRD → AGENTS → README → implementation. Use them in order; **do not skip Phase 1** — its codemap output is the bridge between generic CloudCompare knowledge and this specific fork.

> **Path convention:** the prompts use Unix-style absolute paths (`/docs/...`, `/AGENTS.md`, `/plugins/...`). In this checkout those resolve to the repo root — i.e. `c:/dev/CloudCompare/...` in our environment, or whatever working directory the agent is rooted at. If the agent runs in a different working directory, prepend the actual repo root.

### Phase 1 — Codebase reconnaissance

````markdown
TASK: Analyze the CloudCompare codebase to map all files related to point cloud registration, point picking, multi-window rendering, and plugin architecture.

DO:
1. Search for all files containing "Registration", "Align", "ICP", "pointPicking", "Pick", "OverlayDialog", "ccGLWindow" in filenames and content
2. Identify the exact file paths for:
   - The main registration dialog/UI (likely ccAlignDialog, ccRegistrationDlg, or similar)
   - The registration algorithm implementation (ccRegistrationTools or similar)
   - The point picking system (PickingHub, ccGLWindow picking methods)
   - The plugin interface base classes (ccPluginInterface, ccStdPluginInterface)
   - The overlay dialog base class (ccOverlayDialog)
   - The main window class that manages multiple GL windows (ccMainWindow or MainWindow)
   - The point cloud data class (ccPointCloud, ccHObject)
3. For each key file, extract:
   - Class name and inheritance hierarchy
   - Key public methods (especially virtual methods)
   - Qt signals/slots used for cross-component communication
   - Any TODO comments or known limitations
4. Output a structured markdown table: File Path | Class | Purpose | Key Methods | Dependencies

DO NOT:
- Do not modify any files during this phase
- Do not assume file paths; verify with grep/find
- Do not skip the plugin architecture analysis

OUTPUT FORMAT: Save as `/docs/recon/REGISTRATION_CODEMAP.md`
````

### Phase 2 — Write the PRD

````markdown
TASK: Write a Product Requirements Document for implementing "Manual Dual-Screen Point Cloud Registration" in CloudCompare, modeled after Faro Scene Classic.

CONTEXT: Use the REGISTRATION_CODEMAP.md from Phase 1. The feature must be implementable as a CloudCompare plugin (preferably) with minimal core modifications.

WRITE `/docs/PRD_MANUAL_REGISTRATION.md` with these exact sections:

### 1. Overview (2-3 sentences)
What this feature does and why it exists.

### 2. User Stories
- As a surveyor, I want to pick corresponding points on two point clouds in side-by-side views...
- As a user, I want to see a real-time preview of the alignment before committing...
- As a user, I want minimum 3 point pairs for rigid registration, with RMS error displayed...

### 3. Functional Requirements (numbered FR-01 to FR-XX)
FR-01: User can select Source and Target point clouds from the DB tree
FR-02: Dialog opens with two synchronized 3D viewports (source left, target right)
FR-03: User can pick 3D points in either viewport; picked points are highlighted and stored
FR-04: System enforces minimum 3 point pairs before enabling "Compute/Preview"
FR-05: System computes rigid transformation (SVD-based) from correspondences
FR-06: Preview mode applies temporary transform to source cloud in real-time
FR-07: RMS error and individual point-pair distances are displayed in a table
FR-08: User can delete individual point pairs from the correspondence list
FR-09: "Apply" commits the transform; "Cancel" discards; "Reset" clears preview
FR-10: Undo/Redo is supported for the final committed transform

### 4. Non-Functional Requirements
- Performance: Preview update < 100ms for clouds up to 10M points
- UI: Must use ccOverlayDialog or equivalent modal pattern
- Extensibility: Must be implementable as a standard plugin

### 5. Technical Constraints
- Must reuse existing ccGLWindow instances (no custom GL contexts)
- Must use existing picking infrastructure (ccPickingHub or ccGLWindow::getClick3DPos)
- Must use ccGLMatrix for all transformations
- Must not block the main thread during preview

### 6. UI/UX Mock Description (text-based)
Describe the dialog layout:
- Top: Source/Target cloud selection dropdowns
- Middle: Two ccGLWindow viewports side-by-side (50/50 split)
- Bottom-left: Correspondence table (Point #, Source coords, Target coords, Distance)
- Bottom-right: RMS error label, Compute/Preview/Apply/Cancel/Reset buttons
- Status bar: Instruction text ("Pick a point in the SOURCE cloud...")

### 7. Data Model
Define the minimal data structures needed:
- struct PointPair { CCVector3 source; CCVector3 target; unsigned index; }
- class ManualRegistrationDialog : public ccOverlayDialog
- How the transform is computed and stored

### 8. Success Criteria
- Feature works with two ASCII point clouds
- 3-point registration produces correct rigid transform
- Preview shows immediate visual feedback
- Plugin loads without modifying core build files

### 9. Open Questions
Leave 3-5 open questions for the team (e.g., "Should we support scale registration or only rigid?")

TONE: Technical, precise, implementation-oriented. No marketing language.
````

### Phase 3 — Write the AGENTS.md (or augment the existing one)

> **Note for this fork:** the canonical `AGENTS.md` already exists (and the sibling `AGENTS-*.md` topical files + `docs/context/registration/` layered context). Phase 3 in this codebase should *augment* — not replace — those. If the agent's recon (Phase 1) found anything that contradicts the existing `AGENTS.md` network, surface the contradiction as an Open Question in the PRD instead of overwriting.

````markdown
TASK: Write `/AGENTS.md` (or update existing) to provide persistent context for any LLM agent working on this CloudCompare feature. The AGENTS.md must be structured as a "living document" with these sections:

### Project Identity
- Project: CloudCompare Manual Registration Plugin
- Language: C++ (C++17 or project standard)
- Framework: Qt5/6, OpenGL (via ccGLWindow)
- Build System: CMake
- Plugin Type: Standard qCC plugin (qManualRegistration or similar)

### Architecture Rules (Agent MUST follow)
1. ALWAYS check CCCoreLib/ for math utilities before writing custom algorithms
2. ALWAYS inherit from ccOverlayDialog for modal dialogs, never QWidget directly
3. ALWAYS use ccGLMatrix for transformations; never raw float[16] arrays
4. ALWAYS use the picking hub (ccPickingHub) or ccGLWindow picking APIs; never implement custom raycasting
5. NEVER modify ccGLWindow, ccMainWindow, or core rendering without explicit approval
6. NEVER block the main thread; use Qt's signal/slot for async operations if needed
7. ALWAYS handle null pointers and empty selections defensively (CloudCompare crashes are catastrophic)
8. ALWAYS follow existing naming conventions: m_ prefix for members, camelCase for methods

### Key File References (Auto-populated from reconnaissance)
List the top 10-15 files with one-line descriptions:
- `ccPointCloud.h` — Point cloud data container
- `ccGLMatrix.h` — 4x4 transformation matrix
- `ccOverlayDialog.h` — Base class for modal dialogs
- `ccPickingHub.h` — Central point picking coordinator
- `ccGLWindow.h` — 3D rendering viewport
- `ccRegistrationTools.h` — ICP/registration algorithms (reference only)
- [etc...]

### Plugin Development Patterns
Document the standard plugin structure:
1. Plugin class inherits ccStdPluginInterface
2. UI class inherits ccOverlayDialog
3. Algorithm logic is separate from UI (in a Worker or Tool class)
4. Use qRegisterMetaType for custom types crossing thread boundaries
5. Use ccLog::Print for debug output, not std::cout

### Common Pitfalls
- "CloudCompare uses a custom DB tree (ccHObject). Always check object types with isKindOf() before casting."
- "ccGLWindow refresh is not automatic. Call redraw() or update() explicitly after transform changes."
- "Picking mode must be explicitly exited; otherwise it interferes with other tools."

### Build & Test Commands
Provide the exact commands an agent needs to verify compilation:
```bash
mkdir build && cd build
cmake .. -DPLUGIN_STANDARD_QMANUAL_REGISTRATION=ON
make -j$(nproc)
# Or equivalent for your platform
```

### Extension Points
If the agent needs to modify core code, it must document:
- What core file is touched
- Why a plugin-only approach is impossible
- The minimal surface area of the change

OUTPUT: A single AGENTS.md file that is self-contained. A new agent should be able to implement the first dialog skeleton using ONLY this file + the PRD.
````

### Phase 4 — Write the plugin README

> **Note for this fork:** the plugin directory `plugins/core/Standard/qManualRegistration/` does not exist yet. This README will be created *inside* the plugin tree as part of Milestone 1 (scaffolding). For now, store the draft in `docs/context/registration/DRAFT_PLUGIN_README.md` and move it into the plugin tree when the directory is created.

````markdown
TASK: Write `/plugins/core/Standard/qManualRegistration/README.md` (or equivalent plugin directory). This README is for developers who will work on this plugin, NOT end users.

STRUCTURE:

### qManualRegistration Plugin

#### Purpose
One paragraph describing what this plugin does.

#### Dependencies
- CloudCompare core libraries (CCCoreLib, CCPluginAPI)
- Qt5/6 (Widgets, OpenGL)
- Eigen (if used for SVD, or note if CCCoreLib provides it)

#### Architecture
```
qManualRegistrationPlugin  (entry point, implements ccStdPluginInterface)
   └── qManualRegistrationDialog  (UI, inherits ccOverlayDialog)
           ├── Source/Viewport management
           ├── Point picking coordination
           ├── Correspondence table model
           └── Transform computation (delegated to math utils)
```

#### How to Build
```bash
# From CloudCompare build directory
cmake .. -DPLUGIN_STANDARD_QMANUAL_REGISTRATION=ON
make
```

#### Key Classes
| Class | File | Responsibility |
|-------|------|----------------|
| qManualRegistrationPlugin | qManualRegistrationPlugin.cpp | Plugin entry, menu registration |
| qManualRegistrationDialog | qManualRegistrationDialog.cpp | UI, viewport management, picking |
| ManualRegistrationTool | ManualRegistrationTool.cpp | Math: SVD, RMS, transform application |

#### Integration Points
- Registers menu item under "Plugins > Registration > Manual Registration..."
- Uses ccPickingHub for cross-viewport point selection
- Uses ccGLWindow::setGLTransformation for preview
- Emits signal to apply final transform to ccPointCloud

#### Testing Checklist
- [ ] Plugin loads without errors
- [ ] Dialog opens with two cloud selection dropdowns
- [ ] Two viewports render selected clouds side-by-side
- [ ] Point picking captures 3D coordinates in both viewports
- [ ] Correspondence table updates with each picked pair
- [ ] Compute button enabled only after 3+ pairs
- [ ] Preview applies temporary transform
- [ ] RMS error calculates correctly
- [ ] Apply commits transform to DB tree
- [ ] Cancel closes dialog without changes

#### Known Limitations / TODO
Leave a section for the agent to fill in as development progresses.
````

### Phase 5 — Implementation milestones (for agent execution)

> **Note for this fork:** the milestones below match [`AGENTS_REGISTRATION.md` §5](AGENTS_REGISTRATION.md) §5 above; refer there for the verification criteria for each milestone (M1-M8). The prompts below are the terse "give to the agent" version; combine with §5 for full verification detail.

````markdown
TASK: Implement the Manual Registration plugin in 5 milestones.
STOP after each milestone and wait for human review.

MILESTONE 1: Plugin Skeleton
- Create plugin directory structure
- Implement qManualRegistrationPlugin (entry point, menu registration)
- Empty qManualRegistrationDialog inheriting ccOverlayDialog
- CMakeLists.txt for the plugin
- VERIFY: Plugin compiles and appears in CloudCompare menu

MILESTONE 2: Dialog UI & Cloud Selection
- Create .ui file with: two cloud selectors, placeholder for viewports, correspondence table
- Implement cloud selection from DB tree (validate that selections are ccPointCloud)
- VERIFY: Dialog opens, shows cloud names, validates selection

MILESTONE 3: Dual Viewport & Picking
- Embed two ccGLWindow instances or split existing windows
- Implement point picking in both viewports
- Store picked points in PointPair structures
- VERIFY: Clicking in viewport captures correct 3D coordinates

MILESTONE 4: Transform & Preview
- Implement SVD-based rigid transform from 3+ point pairs
- Implement temporary preview (setGLTransformation)
- Implement RMS error calculation
- VERIFY: Preview moves source cloud; RMS displays correctly

MILESTONE 5: Commit, Undo, Polish
- Implement Apply (commit transform to DB tree)
- Implement Cancel/Reset
- Add undo support
- Add error handling and edge cases
- VERIFY: Full workflow end-to-end

RULES FOR EACH MILESTONE:
- Write unit tests where possible (even if minimal)
- Update README.md Testing Checklist as items are completed
- Update AGENTS.md if new constraints are discovered
- Do NOT proceed to next milestone until current one compiles and runs
````

### Quick reference — master prompt

> **Pro tip:** if your agent has a small context window, run Phase 1 first, then feed `docs/recon/REGISTRATION_CODEMAP.md` back into the agent as context for Phases 2–4. The reconnaissance output is the critical bridge between "generic CloudCompare knowledge" and "this specific fork/version."

````markdown
You are an expert C++/Qt developer working on the CloudCompare open-source project. Your goal is to prepare comprehensive documentation and then implement a new plugin for manual dual-screen point cloud registration.

WORKING DIRECTORY: [specify your repo root]

CONSTRAINTS:
- You may only read files; do not write code yet unless explicitly instructed
- Always verify file paths with find/grep before referencing them
- Prefer plugin architecture over core modifications

EXECUTE THESE PHASES IN ORDER:
1. Run Phase 1 (Codebase Reconnaissance) and save output to /docs/recon/REGISTRATION_CODEMAP.md
2. Run Phase 2 (Write PRD) and save to /docs/PRD_MANUAL_REGISTRATION.md
3. Run Phase 3 (Write AGENTS.md) and save to /AGENTS.md
4. Run Phase 4 (Write README) and save to /plugins/core/Standard/qManualRegistration/README.md
5. Report back with a summary of all generated files and any blockers discovered

After each phase, pause and confirm completion before proceeding.
````

