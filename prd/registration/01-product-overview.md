# 01 — Product Overview

> **What problem are we solving, for whom, and how do we know we've solved it?**

This document is the "why". If [`00-README.md`](00-README.md) is the table of contents, this is the executive summary — the section a stakeholder reads to decide whether to fund the work.

---

## 1. The problem, in one sentence

**CloudCompare can register two point clouds well, but it cannot register a 20-scan project with mixed target-bearing and targetless scans the way a surveyor needs to run a real job.**

That gap is the entire reason for this PRD.

---

## 2. The pain in detail

### 2.1 What a surveying job actually looks like

A typical topo / monitoring / as-built job produces **5 to 200 scans**. The workflow in 2026, on a Windows laptop with a Terrestrial Laser Scanner (Faro Focus, Leica RTC360, Trimble X7, etc.), is:

1. **Field capture.** Tripod → scan → move → scan. Often 30–60 seconds per scan. Spheres / checkerboards placed around the scene for **target-based registration**; some jobs use **cloud-to-cloud only** when the geometry is dense enough.
2. **Transfer to office.** E57, LAS, FLS, or proprietary scan format onto the laptop.
3. **Import into registration software.** Faro SCENE, Leica Cyclone REGISTER 360, Trimble RealWorks, or (for the budget-conscious) **CloudCompare**.
4. **Organise into clusters.** Surveyors group scans by floor, by area, by sub-project. A 30-scan building might become 4 clusters of ~7-8 scans each.
5. **Register within each cluster.** Each cluster is internally registered as a unit. Lock it. The surveyor knows the result is good — they've verified it with clipping boxes.
6. **Register the clusters together.** Two locked sub-clusters → drag both into a parent cluster → register the parent. The "lock + reference at parent" pattern.
7. **Verify with global control.** If the job has survey control points (georeferenced targets at known coordinates), the entire registered project is tied to those control points.
8. **Export.** E57 / LAS / PTS / RCP for downstream modelling.

**Steps 4–6 are where CloudCompare falls down.** The single-window `ccPointPairRegistrationDlg` is fine for "I have two clouds and I want to roughly align them" but the surveyor needs:

- A **persistent UI** that shows their cluster structure (the db-tree alone is not enough — they need registration-specific status: lock, reference, registration state).
- A **multi-mode** registration pipeline (target-based, top-view, cloud-to-cloud, in any order) with the ability to lock a result and move on.
- A **dual-viewport correspondence view** (picking pairs in two side-by-side clouds) — the existing single-window dialog has this but it's modal, single-shot, doesn't fit a multi-stage workflow.
- **Save and reload** of registration state across sessions.
- A **registration report** they can hand to the client or check against.

### 2.2 What the competition does

| Software | Approx cost | Strengths | Weaknesses (for our market) |
|---|---|---|---|
| **Faro SCENE / SCENE 2go** | £2-4k/yr per seat | Best-in-class target detection; cluster + lock + reference; interactive registration 2023.1+; hybrid registration; VR view. | FARO-locked; expensive. |
| **Leica Cyclone REGISTER 360** | Enterprise | Tight Cyclone integration; survey-control heavy; batch. | Heavyweight; Leica-locked. |
| **Trimble RealWorks** | Enterprise | Strong survey workflow; surface generation; inspection. | Trimble-locked; UI dated. |
| **Autodesk ReCap Pro** | ~£300/yr | Cheap; Autodesk pipeline friendly. | Limited registration; no cluster model. |
| **CloudCompare (today)** | Free | Open; multi-format; best-in-class for analysis. | Registration is point-pair only; no cluster model. |

**The strategic gap.** CloudCompare is the only open, vendor-neutral choice. If we close the registration gap, we become the **default for surveyors who don't want to be locked to one scanner brand**. The user is already doing this — Icelabz uses Faro and Trimble scanners and uses CloudCompare for everything except the registration step, which they do in SCENE (because they have to). Bringing that step into CloudCompare removes a paid-seat dependency.

### 2.3 What a user does *today* to work around the gap

- **Buys a SCENE seat** for the registration step, then exports `.e57` into CloudCompare. Costs ~£3k/yr.
- **Does it all in CloudCompare manually**, opening `ccPointPairRegistrationDlg` 5-20 times, dragging C2C alignment, and using `qPCL` ICP for refinement. Tedious, error-prone, no report.
- **Uses qPCL's ICP for everything**, but ICP needs a good initial alignment and CloudCompare's single-window dialog is the only way to provide one. ICP is good for refinement, bad for cold-start.

**Our opportunity.** Replace the SCENE seat. Cost to the user: zero (CloudCompare is free). Cost to us: 3-6 months of dev + ongoing maintenance.

---

## 3. Users

### 3.1 Primary persona: the solo surveyor-company owner

> *Dave, 42, runs a 2-person surveying firm. Bought a Faro Focus S70 two years ago. Uses CloudCompare for everything except registration because it "just works" in SCENE for his 8-15 scan jobs. Paying ~£3k/yr for SCENE because he can't register 12 scans manually in CloudCompare without losing a day to it.*

- **Tools he already has:** Faro Focus scanner, SCENE seat, CloudCompare, AutoCAD.
- **Job sizes:** 5-30 scans.
- **Tolerance:** 5mm typical for indoor, 10-20mm for outdoor topo.
- **Pain:** SCENE seat is overhead he doesn't use 90% of the time. Manual CloudCompare registration is a time-sink.
- **What success looks like:** He opens CloudCompare, drags his 12 scans in, groups them into 2 clusters, runs target-based registration on each cluster, locks both, drags them under a parent, registers the parent, sees a 4mm overall RMS, exports `.e57`. 10 minutes total. Closes SCENE for good.

### 3.2 Secondary persona: the Icelabz-internal user (you)

> *You. The owner and sole developer of this codebase. Building this partly because (a) you need it for Icelabz jobs, and (b) you think it'll be useful upstream. You're the maintainer, the QA, and the first user.*

**What success looks like:** You ship it. Other surveyors find it. It works.

### 3.3 Tertiary persona: the scanner-vendor employee

> *Someone at Faro / Leica / Trimble evaluating CloudCompare as a "vendor-neutral" tool. If the registration is good, they may recommend CloudCompare to customers who want to escape vendor lock-in.*

**What success looks like:** They don't say "CloudCompare can't register scans properly" anymore.

### 3.4 Anti-persona: the BIM modeller

> *Someone who needs Revit / IFC outputs from point clouds, not just registered point clouds.*

**Why they're not a target:** CloudCompare doesn't have a Revit pipeline, and that's a much bigger feature than registration. They should use CloudCompare + Recap + Revit, or a BIM-specific tool. We're not competing here.

---

## 4. The five jobs-to-be-done (in order of priority)

| # | Job | Frequency | Today |
|---|---|---|---|
| **1** | "I have N scans, register them into one cloud at known tolerance" | Every job | Manual; takes 30min-2hr; SCENE for serious jobs |
| **2** | "I have a registered sub-project, add more scans to it without losing accuracy" | Every medium+ job | Manual; risky; requires re-verify |
| **3** | "I need to verify my registration is good enough" | Every job | Manual; eyeball + clip box |
| **4** | "I need a record of how the registration was done" | Audit / disputes | Manual; screenshot + email |
| **5** | "I have many similar jobs, automate the registration" | Repeat clients | Impossible; CLI is dream |

**v1 covers jobs 1, 2, 3, 4.** Job 5 (batch / CLI for similar jobs) is partially covered by the CLI mode but a proper "job template" feature is v2.

---

## 5. Success metrics (what we measure to know we won)

### 5.1 Quality

- **Per-pair RMS < 5mm for a typical indoor job** (spheres, 10m radius). This is the SCENE benchmark and what surveyors expect.
- **Cluster-to-cluster drift < 3mm** for sub-projects pre-registered with targets, then combined via lock + reference. This is the SCENE benchmark.
- **No silent misregistration.** Every commit must surface a report. No commits without a report.
- **Save/load round-trip is exact.** Reload gives the same transforms, lock states, reference states, and pair tables.

### 5.2 Speed (one-dev, one-laptop target)

- **Manual point-pair registration** for two 5M-point clouds: pick 4 pairs + commit < 30 seconds.
- **Target-based registration** of a 12-scan cluster with 30 spheres: < 60 seconds.
- **C2C refinement** of two pre-aligned 10M-point clouds: < 5 minutes.
- **CLI batch of 10 jobs**: < 30 minutes.

### 5.3 Adoption (we can't fully measure this in OSS, but…)

- Plugin appears in `Help → About → Plugins` on first launch.
- Plugin shows up in plugin docs on the fork's GitHub Pages site.
- At least one survey firm outside the Icelabz user base uses it for a real job in the first 3 months post-release.
- The user (you) uses it on a real Icelabz job in the first month post-release.

### 5.4 Quality gates (these are go/no-go for v1 ship)

- [ ] `cmake --fresh -DPLUGIN_STANDARD_QREGISTRATION=ON` builds clean on Windows + macOS + Linux.
- [ ] `cmake --build build --target check-format` passes.
- [ ] All four CI jobs green with the plugin ON.
- [ ] Manual end-to-end test of the §3.1 user story (12 scans, 2 clusters, lock + reference, parent register) on a real dataset — not a synthetic test.
- [ ] Save/load round-trip on a 20-scan registered project.
- [ ] CLI run on a 5-job batch with output verified.

---

## 6. The four risks I'm worried about (in priority order)

### 6.1 One developer, one build, ambitious scope (HIGH risk, HIGH impact)

This is a multi-thousand-line plugin that touches 5+ CloudCompare subsystems. If Icelabz loses dev time (illness, busy season, paid-client work), the plugin stalls. Mitigation: **strictly scope v1** — anything that can be deferred is deferred to v2/v3. See [`07-roadmap.md`](07-roadmap.md).

### 6.2 CloudCompare core changes during v1 (MEDIUM risk, MEDIUM impact)

Upstream is active. If a release during dev changes `ccGLWindow`, `ccPickingHub`, or `ccHObject` APIs, we react. Mitigation: **pin to a specific upstream SHA** during v1 dev, rebase after v1 ships. The plugin-only constraint (see `AGENTS_REGISTRATION.md` §3) means upstream changes are absorbed by re-implementing against the new API, not by re-architecting.

### 6.3 Real-world datasets break assumptions (MEDIUM risk, HIGH impact)

Synthetic tests pass; real Faro / Leica / Trimble data with their quirks (compensated / uncompensated, different coordinate systems, different point densities, dropped points at scan edges) breaks things. Mitigation: **dogfood on real Icelabz data** during dev, not at the end. Every milestone gets verified on real data.

### 6.4 Save/load format churn (LOW risk, MEDIUM impact)

The existing `.bin` entity format is stable. If we add new fields, we need to handle "loaded from old version" cases. Mitigation: **add registration metadata as a new JSON sidecar**, not in the `.bin` blob. Sidecars are easy to version and migrate.

---

## 7. Non-goals (explicit "we are NOT doing this")

| We're not | Why | What we say instead |
|---|---|---|
| Replacing `ccPointPairRegistrationDlg` | It works. Removing it breaks existing users. | The dialog stays. It's the "single-shot align two clouds" path. The new plugin is for the multi-scan workflow. |
| Building a scene management platform | CloudCompare is a desktop tool, not a project manager. | The plugin lives inside CloudCompare's existing project model. No "projects" feature. |
| Scanner vendor integration | Vendor SDKs are NDA-only and break frequently. | User imports the scan files they already have. We register what they load. |
| Replacing SCENE for *all* survey workflows | SCENE has features we won't build (VR view, on-site registration, WebShare Cloud). | The plugin covers the 80% that doesn't need those. The other 20% keeps using SCENE. |
| Texture / colour-aware picking | Adds significant complexity to the picking listener. | Deferred to v2. |
| On-site / real-time registration | Requires scanner SDK access. | Deferred to v3. |
| Multi-level undo for registration | The existing `ccPointPairRegistrationDlg` doesn't have it either. | v1 has single-level undo on the commit. Multi-level is v3. |

---

## 8. Strategic positioning

Three audiences, three messages:

- A Faro / Leica / Trimble user reading the README should think "this replaces my SCENE seat for 80% of what I do". (Wedge.)
- A CloudCompare user reading the README should think "oh, registration is no longer a chore". (Upsell.)
- A scanner vendor reading this PRD should think "they're serious about the open-source alternative". (Legitimacy.)

We are **not** trying to out-feature SCENE. We are trying to own the **vendor-neutral, free, good-enough** segment. SCENE for the high-end. RealWorks for the Trimble-loyal. CloudCompare + this plugin for everyone else.

---

## 9. Pointers

- **The "what"** — [`02-features.md`](02-features.md)
- **The "how users do it"** — [`03-user-workflows.md`](03-user-workflows.md)
- **The "what it looks like"** — [`04-ui-pages.md`](04-ui-pages.md)
- **The "how it's built"** — [`06-architecture-and-api.md`](06-architecture-and-api.md)
- **The "what we cut"** — [`07-roadmap.md`](07-roadmap.md)
- **The agent-facing implementation plan** — [`../../AGENTS_REGISTRATION.md`](../../AGENTS_REGISTRATION.md) + [`../../docs/context/registration/`](../../docs/context/registration/)
