# AGENTS-style.md — docs-site style guide

What makes a good CloudCompare docs page vs a good AGENTS page. Read this
before writing a new doc page or substantially rewriting an existing one.

---

## 1. Audience — and why we keep two parallel doc sets

The fork ships **two parallel documentation sets**:

| Set | Audience | Path | Tone |
|---|---|---|---|
| **Agent-facing** (`AGENTS*.md`) | LLMs + humans reading source code | `AGENTS.md`, `AGENTS-*.md`, sub-folder `AGENTS.md`, `docs/context/` | Technical, file-path-precise, procedural. Says "edit `libs/qCC_db/include/ccHObject.h` and add the `getPickingHub()` virtual." |
| **User-facing** (this site) | End users + humans who want to read rendered docs | `website/docs/` | Conceptual, behaviour-focused, no code locations. Says "CloudCompare supports point-pair registration with manual picking in two viewports." |

**Do not duplicate content** between the two. If a doc page describes
what a feature does, it lives here. If it describes where the code is
or how to modify it, it lives in `../AGENTS*.md`. Pages can cross-link
between the two sets when both are useful (e.g. "for the build
narrative see [`BUILD-LOCAL.md`](../BUILD-LOCAL.md); for the per-step
commands see the Build category in the sidebar").

---

## 2. Front-matter conventions

Every `.md` file in `docs/` (except `intro.md`) should have this minimal
front-matter block:

```yaml
---
title: <Human-readable title — sentence case>
sidebar_label: <Sidebar label — short, fits the narrow sidebar>
sidebar_position: <integer — relative order within the category>
---
```

| Field | Convention |
|---|---|
| `title` | Sentence case. E.g. `Layer ownership`, not `Layer Ownership`. |
| `sidebar_label` | Optional. If omitted, `title` is used. Use a shorter label when the title is long. |
| `sidebar_position` | Optional. If omitted, Docusaurus uses alphabetical order. Most existing pages omit it and rely on the sidebar's `items` array order instead. |
| `id` | **Do not set.** The default is the file path (`getting-started/prerequisites`), which is what makes the URL `/docs/getting-started/prerequisites` instead of `/docs/prerequisites`. |
| `slug` | **Do not set** except in `intro.md` (where it must be `slug: /` to claim the site root). |
| `description` | Optional. When set, Docusaurus injects it into `<meta name="description">` and the OpenGraph card. Recommended for category index pages. |

The full front-matter block is plain YAML between two `---` lines. **No
TOML**, no JSON — YAML only.

---

## 3. Page structure

A typical doc page has these sections in this order:

```markdown
# <Title>            ← H1 — the title; same as `title:` front-matter

<intro paragraph>    ← 2-3 sentences. What is this? Why does it exist?

## <section>         ← H2 — the page's main sections
...
```

Rules:

- **One H1 per page** (the title). Docusaurus injects the front-matter
  `title` as the rendered H1, so the page body usually starts at H2.
- **Sections are short** — 1-3 paragraphs or a table each. Pages
  longer than ~400 lines are a smell; split.
- **Use tables for dense structured info** (e.g. "key methods" —
  see [`docs/architecture/plugin-system.md`](docs/architecture/plugin-system.md)
  for the canonical example).
- **Use code blocks for shell commands, CMake invocations, and
  config snippets** — Prism auto-detects the language from the
  fence info string (` ```bash `, ` ```cmake `, ` ```cpp `).
- **Diagrams are ASCII or Mermaid** — no images unless they're
  screenshots of UI. (None so far.)

---

## 4. Linking conventions

| Link type | Format | Example |
|---|---|---|
| **Internal** — to another doc page | Absolute `/docs/...` | `[Plugin authoring](/docs/plugins/authoring)` |
| **Internal** — to a category index | Absolute, no `.md` | `[Build](/docs/build)` |
| **Internal** — to the AGENTS network | Relative `../` paths | `[AGENTS.md](../AGENTS.md)`, `[AGENTS-architecture.md](../AGENTS-architecture.md)` |
| **External** — upstream project | Plain URL | `[CloudCompare/CloudCompare](https://github.com/CloudCompare/CloudCompare)` |
| **External** — code on this fork | GitHub URL with line numbers | `qCC/mainwindow.cpp:12113` → `https://github.com/bramburn/CloudCompare/blob/master/qCC/mainwindow.cpp#L12113` |

**Always use absolute paths for in-site links.** Relative paths break
when the source or target file is moved, and `onBrokenLinks: 'throw'`
won't catch every relative-path case.

**Verify before pushing:**

```bash
cd website
node scripts/verify-sidebar-coverage.js    # catches both orphan docs and broken links
```

---

## 5. Code block conventions

Always include the language info string. Prism needs it to apply
syntax highlighting:

````markdown
```bash
cmake -S . -B build -G Ninja
```

```cmake
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL)
```

```cpp
ccHObject* root = MainWindow::dbTreeRoot();
```

```powershell
& C:\dev\tools\cc-build.cmd
```

```json
{"type": "Standard", "name": "qCSF"}
```
````

For a diff or comparison:

````markdown
```diff
- option(PLUGIN_IO_QE57 "E57 reader/writer" OFF)
+ option(PLUGIN_IO_QE57 "E57 reader/writer" ON)
```
````

The `additionalLanguages` list in `docusaurus.config.js`
(`['bash','cmake','cpp','diff','json','powershell']`) is what makes
each of these render with the right colours. If you need a new
language, add it there AND add the language to the front of the list
in this guide.

---

## 6. What does NOT belong in this site

- **Build narrative** (long-form "how I got this build working on my
  machine") → that lives in [`../BUILD-LOCAL.md`](../BUILD-LOCAL.md).
  This site has the clean *invocation* (`cmake -S . -B build -G Ninja`),
  not the troubleshooting journal.
- **Code locations** ("edit `libs/qCC_db/include/ccHObject.h`") → that
  lives in the `AGENTS-*.md` files. This site describes behaviour, not
  source paths.
- **AGENT-style rules** ("never modify `ccGLWindow` without approval") →
  those belong in `../AGENTS.md` and the `AGENTS-plugin-dev.md` /
  `AGENTS-architecture.md` siblings.
- **Personal opinions / "I tried X and it didn't work for me"** → not
  a docs site concern. Use a blog post or a GitHub issue instead.

---

## 7. Voice

- **Active, present tense.** "The plugin loads at startup." Not
  "The plugin will be loaded."
- **Second person for instructions.** "Configure with `--fresh`."
  Not "one should configure with `--fresh`."
- **No marketing language.** "CloudCompare is a powerful 3D point
  cloud processing tool" → just "CloudCompare processes 3D point
  clouds."
- **British or American English — pick one.** The existing pages lean
  American (no -ise spellings, "color" not "colour"). Stay consistent.

---

## 8. Pointers

- **Site entry point** — [`./AGENTS.md`](AGENTS.md)
- **Config reference** — [`./AGENTS-docusaurus.md`](AGENTS-docusaurus.md)
- **Agent-facing equivalent** — [`../AGENTS.md`](../AGENTS.md)
