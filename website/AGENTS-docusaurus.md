# AGENTS-docusaurus.md — Docusaurus configuration reference

What every key in `website/docusaurus.config.js` and `website/sidebars.js`
controls, and the conventions the site follows. Read this before adding a
new plugin, theme tweak, sidebar category, or docs plugin.

---

## 1. `docusaurus.config.js`

Top-level keys (all required unless noted):

| Key | Value | Why |
|---|---|---|
| `title` | `'CloudCompare'` | Browser tab + sitemap title. |
| `tagline` | `'3D point cloud & mesh processing'` | Hero subtitle on the React homepage. |
| `favicon` | `'img/favicon.ico'` | Lives under `static/img/`. |
| `url` | `'https://bramburn.github.io'` | Production URL; used for canonical links + sitemap. |
| `baseUrl` | `'/CloudCompare/'` | GitHub Pages project-site path. **MUST match `robots.txt`'s sitemap URL.** |
| `organizationName` | `'bramburn'` | GitHub org for the deploy workflow. |
| `projectName` | `'CloudCompare'` | GitHub project name. |
| `deploymentBranch` | `'gh-pages'` | Where the deploy workflow publishes. |
| `trailingSlash` | `false` | URLs are `/docs/foo`, not `/docs/foo/`. **All in-site links must match.** |
| `onBrokenLinks` | `'throw'` | Build fails on a broken in-site link. |
| `markdown.hooks.onBrokenMarkdownLinks` | `'warn'` | Markdown reference-style links warn (not throw). |
| `i18n.defaultLocale` / `locales` | `'en'` / `['en']` | English-only for now. |
| `future.v4` | `true` | Pre-opt into Docusaurus v4 behaviour. |

### 1a. Preset (`classic`) options

Configured under `presets[0][1]` (the second arg of the `classic` preset).
Each preset option maps to a sub-plugin:

| Preset option | Sub-plugin | What it controls |
|---|---|---|
| `sitemap` | `@docusaurus/plugin-sitemap` | `changefreq: 'daily'`, `priority: 0.5`, `ignorePatterns: ['/tags/**']`. The sitemap plugin ships with `preset-classic`; we configure it via the preset (NOT a top-level `plugins:` entry — that would collide on the default plugin ID). |
| `docs` | `@docusaurus/plugin-content-docs` | `sidebarPath: './sidebars.js'`, `editUrl`, `showLastUpdateAuthor`, `showLastUpdateTime`. The `editUrl` controls the "Edit this page" link. |
| `blog` | `@docusaurus/plugin-content-blog` | `blogTitle`, `blogSidebarTitle`, `postsPerPage`, `feedOptions` (RSS + Atom with XSLT), and three `onInline*` warn-mode hooks that catch common footguns. |
| `theme.customCss` | (injected into the classic theme) | Path to `src/css/custom.css`. |

### 1b. Theme config (`themeConfig`)

| Key | Value | Notes |
|---|---|---|
| `image` | `undefined` | Falls back to a generated text card. Replace by shipping `static/img/social-card.*`. |
| `colorMode.defaultMode` | `'light'` | Plus `respectPrefersColorScheme: true` so dark-mode fans get the dark theme automatically. |
| `docs.sidebar.hideable` | `true` | Per-page collapse toggle. |
| `docs.sidebar.autoCollapseCategories` | `true` | Opening one category collapses the others. |
| `navbar.items` | Docs + Changelog + GitHub | Logo at left, GitHub external link at right. |
| `footer.style` | `'light'` | Three link groups: Documentation, Resources, This fork. |
| `prism.theme` / `darkTheme` | `prismThemes.github` / `prismThemes.dracula` | Light/dark syntax highlighting pair. |
| `prism.additionalLanguages` | `['bash','cmake','cpp','diff','json','powershell']` | **Critical for a C++ project:** `cpp` enables full C++17 highlighting. The others cover the build/shell/config surface. |

---

## 2. `sidebars.js`

Manual sidebar tree (not auto-generated). The convention:

- **Top-level categories are concerns, not folders** — the same
  organisational principle as Docusaurus's own docs and the upstream
  CloudCompare Wiki.
- **Order tells a story:** Getting started → Architecture → Build &
  deployment → Plugins → Reference. The reader learns *what the codebase
  is* before *how to deploy it* before *how to extend it*.
- **Every category has a `link` field** pointing at a valid doc id
  (`type: 'doc', id: '<category>/index'`). The category title becomes
  clickable, jumping to the category's overview page.
- **Categories are collapsed by default** EXCEPT "Getting started"
  (the reader wants to see this on first load).
- **Every `.md` under `docs/` must be referenced** — use
  `scripts/verify-sidebar-coverage.js` to check.

The current 5 top-level categories (mirroring the report's
"Getting Started / Architecture / Modules / Guides / Reference" intent,
adapted to the fork's actual docs):

| Category | Doc ids covered | Maps to report's slot |
|---|---|---|
| **Getting started** | `getting-started/{index,overview,prerequisites,quick-tour}` | "Getting Started" |
| **Architecture** | `architecture/{overview,layers,data-model,plugin-system,ui}` | "Architecture" |
| **Build & deployment** | `build/{index,upstream,windows,linux,macos,troubleshooting}` + `ci/{overview,github-pages,windows-build,upstream-matrix}` | "Guides" (the procedural slot) |
| **Plugins** | `plugins/{overview,local-set,disabled-priority,standard,io,gl,authoring}` | "Modules" (the subsystem deep-dive slot) |
| **Reference** | `reference/{coding-standards,naming,build-flags,faq}` | "Reference" |

---

## 3. The `scripts/` folder

| Script | Purpose | When to run |
|---|---|---|
| `strip-ids.py` | Strip `id:` front-matter from every doc except `intro.md`. | After a batch of file moves, if URLs end up with short-ids instead of folder paths. |
| `rewrite-doc-links.py` | Convert relative links to absolute `/docs/...` links. | Same. |
| `fix-broken-links.py` | Legacy first-pass link repair. Superseded by `fix-absolute-doc-links.py`. | Don't run. |
| `fix-absolute-doc-links.py` | Idempotent corrector for `rewrite-doc-links.py` artifacts. | Same. |
| `verify-sidebar-coverage.js` | **Read-only.** Confirms every doc is in the sidebar and every link resolves. | **Run before any sidebar / link-graph change.** |
| `init-queue.js` | Populate the LLM auto-docs queue at `.docstate.json`. | Run once initially (writes `1k+` items); re-run with `--merge` after adding new source files. **Not invoked by the build.** |

All four Python repair scripts are idempotent (a no-op on already-correct files).
`onBrokenLinks: 'throw'` re-validates every link during `npm run build`,
so a wrong rewrite fails the build the same way it would in CI.

### 3a. The `.docstate.json` queue

`website/.docstate.json` is the input to the (future) auto-docs workflow
that LLM-generates one docs page per source file. The schema:

```json
{
  "schemaVersion": 1,
  "lastRun": "<ISO 8601 timestamp>",
  "sourceDirs": ["libs", "plugins", "qCC", "ccViewer"],
  "excludeVendored": true,
  "mode": "initial | merge | reset",
  "queue": [
    {
      "path": "libs/qCC_db/include/ccHObject.h",
      "status": "pending",
      "docPath": "modules/qCC_db/include/ccHObject.h"
    },
    ...
  ],
  "pointer": 0
}
```

Key fields:

- **`path`** — repo-relative source file path (e.g. `libs/qCC_db/include/ccHObject.h`).
- **`docPath`** — Docusaurus doc ID for the generated page. Resolves to URL `/docs/<docPath>` and lives on disk at `website/docs/<docPath>.md`. `CMakeLists.txt` files become `<dir>/build-config`. Collision-prone flat directories (e.g. `qCC/`, `ccViewer/`) get the file extension appended so a `.h` and `.cpp` with the same basename produce distinct URLs.
- **`status`** — `pending` | `done` | `skipped`. The workflow advances one item per run; `--merge` preserves existing statuses; `--reset` forces every item back to `pending`.
- **`pointer`** — cursor into the sorted queue (priority 1=headers, 2=sources, 3=CMakeLists, then alphabetical). Currently informational; the workflow recomputes it on each run.

Initial run produces **1,081 items** (with `--exclude-vendored`; **6,850** without — see the flag in `init-queue.js`). The bulk of the difference is `cc3DFin` (3,802 files) and `qCanupo` (1,489), both vendored submodule-installed plugins with their own upstream docs.

---

## 4. Conventions to follow

1. **ESM over CommonJS** — the config + sidebar use ESM
   (`export default config;`); don't `require(...)`.
2. **Configure bundled plugins via the preset, not as top-level plugins**
   — adding `@docusaurus/plugin-sitemap` to the top-level `plugins:`
   array collides on the default plugin ID and breaks the build.
3. **Every category has a `link: { type: 'doc', id: '<category>/index' }`**
   — without it the category title is non-clickable.
4. **Doc files use kebab-case** (`getting-started/quick-tour.md`), no
   underscores, no spaces. Front-matter is YAML only — no TOML.
5. **Internal links are absolute** (`/docs/...`) — relative links break
   on page moves and the build's `onBrokenLinks` checker doesn't
   resolve them as well.
6. **`trailingSlash: false` everywhere** — links, sitemap, robots.txt,
   navbar routes, footer routes. The one place we relax it is the
   category index URLs (Docusaurus renders `/docs/build` either way).

---

## 5. Pointers

- **Site entry point** — [`./AGENTS.md`](AGENTS.md)
- **Style guide for docs** — [`./AGENTS-style.md`](AGENTS-style.md)
- **Config source** — [`./docusaurus.config.js`](docusaurus.config.js)
- **Sidebar source** — [`./sidebars.js`](sidebars.js)
- **Docusaurus docs** — <https://docusaurus.io/docs>
