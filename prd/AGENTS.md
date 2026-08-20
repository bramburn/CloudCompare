# prd/ — Agent Instructions

> Read this **in addition to** the root [`AGENTS.md`](../AGENTS.md).
> This file adds the rules for writing and maintaining
> product-requirement documents (`PRD*.md`) under this folder.

## Purpose

`prd/` is where long-form product-requirement docs live. The
convention is one folder per "feature area" or "deliverable
stream" (e.g. `prd/scan-view/`, `prd/rust/`) and one `PRD*.md`
per major release slice inside that folder.

PRDs are **durable**: a PRD may live for the lifetime of a feature,
getting versioned and updated as the design evolves. PRDs are
**not** throwaway like `experimental/` sessions.

## Structure

```
prd/
├── AGENTS.md                       ← this file
├── rust/                           ← feature area: Rust migration
│   ├── README.md                   ← (legacy) overview
│   ├── 01-foundation.md
│   ├── 02-scalarfield.md
│   └── ...
└── scan-view/                     ← feature area: spherical scan browser
    ├── PRD.md                      ← the canonical spec for scan-view
    └── ...
```

A new feature area gets a new folder. A new release slice of an
existing feature area gets a new `PRD-<topic>.md` (or `0X-<topic>.md`)
inside the existing folder.

## How to write a PRD

A PRD is a **narrative** that an LLM agent (or the user, six months
later) can read to understand what the feature is, why it exists,
what's in scope, what's out of scope, and what the open questions
are. Format:

```md
# <Feature name> — PRD

## Problem
<2–4 sentences. What user pain is this addressing?>

## Goals
<bulleted. Measurable where possible.>

## Non-goals
<bulleted. Equally important — what are we NOT doing?>

## User stories
<bulleted. "As a <role>, I want <capability>, so that <reason>.">

## Design
<the meat. Architecture, data flow, API surface, edge cases, error
handling. Show code, schemas, and sequence diagrams where helpful.>

## Risks
<bulleted. What could go wrong, and what's the mitigation?>

## Open questions
<bulleted. Things we don't know yet and need to resolve before
shipping.>

## References
<links to upstream docs, RFCs, prior art, related PRDs.>
```

Match the format the user has used in `prd/scan-view/PRD.md` and
`prd/rust/01-foundation.md`. Don't invent a new layout.

## Hard rules (for agents)

- **Read the relevant prior art first.** Before writing or editing
  a PRD, read the existing PRDs in this folder, the root
  `AGENTS.md`, the goal-level `AGENTS_*.md` files at the repo
  root, and any session / decision record in
  [`experimental/docs/`](../experimental/docs/) that touches the
  same area. If something close exists, **extend** it rather than
  duplicating.
- **PRDs are not design docs.** PRDs describe the **what** and
  **why**; they are the contract between the user and the agent.
  Detailed implementation steps belong in `experimental/sessions/`
  (for prototypes) or in the plugin's own README (for shipped
  features). Link out, don't inline.
- **Mark status explicitly.** Use a status banner at the top:
  ```md
  > Status: **draft** | **accepted** | **shipped** | **superseded by [PRD-X](…)**
  ```
  Anything without a status is a draft and should not be acted on
  without asking the user.
- **Update the PRD when scope changes.** A PRD that drifts from
  the implementation is worse than no PRD. When you add a new
  feature to the plugin, edit the PRD's `Design` section in the
  same commit. When you decide something is out of scope, move it
  from `Goals` to `Non-goals` (don't delete — the record is the
  point).
- **Decisions live in `decisions.md` files**, not in PRD prose.
  PRDs reference decisions; they don't contain them. The pattern
  is `prd/<area>/decisions/<topic>.md` for the long form, with a
  one-line pointer in the PRD.

## When to create a new PRD vs extend an existing one

| If you want to… | Then… |
|---|---|
| Add a new release slice to an existing feature | Add a new `PRD-<slice>.md` next to the existing one. |
| Document a brand-new feature area | Create a new `prd/<area>/` folder with a `PRD.md`. |
| Capture an architectural decision | Add a new `prd/<area>/decisions/<topic>.md`. |
| Capture a one-line "we picked X" call | Update the `decisions.md` summary at the `prd/<area>/` root. |

## Cross-references

- Root [`AGENTS.md`](../AGENTS.md) — repo-wide workflow, build
  commands, the experimental-vs-production gate.
- Top-level `AGENTS_REGISTRATION.md` and the `AGENTS_*.md` files
  at the repo root — per-goal scoped AGENTS files. Add a new one
  if the feature is large enough to deserve its own; add a PRD
  here if the feature is small.
- [`experimental/AGENTS.md`](../experimental/AGENTS.md) — for
  prototypes and concept tests, the *experimentation* half of the
  feature lifecycle. A PRD is the *delivery contract* half.
- Top-level [`AGENTS_REGISTRATION.md`](../AGENTS_REGISTRATION.md) —
  the current list of in-flight feature goals.
