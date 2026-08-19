# AGENTS.md — `website/` (Docusaurus docs site)

Human-facing documentation site for the Icelabz fork of CloudCompare.
Built with **Docusaurus 3.10.2** (preset-classic + `@docusaurus/faster`),
React 19, MDX 3. Deployed to GitHub Pages by
[`.github/workflows/deploy-docs.yml`](../.github/workflows/deploy-docs.yml)
on every push to `master` that touches `website/**`.

> **Audience split:** this site is for **end users** + **humans reading the
> rendered docs**. The **agent-facing** reference for the same codebase lives
> at [`../AGENTS.md`](../AGENTS.md) + the `../AGENTS-*.md` siblings + the
> `../docs/context/` folders. Don't duplicate procedural content between the
> two audiences — the site describes *what each feature does*, the AGENTS
> network describes *where the code lives*.

---

## 0. How to use this file

| Topic | File | What it covers |
|---|---|---|
| The Docusaurus config + sidebar + theme conventions | [`website/AGENTS-docusaurus.md`](AGENTS-docusaurus.md) | What `docusaurus.config.js` controls, how `sidebars.js` is structured, what the `scripts/` helpers do. |
| Doc style guide | [`./AGENTS-style.md`](AGENTS-style.md) | Front-matter conventions, link conventions, what makes a good docs page vs a good AGENTS page. |
| Build / preview / deploy | [`../BUILD-LOCAL.md`](../BUILD-LOCAL.md) §"docs site" | Local `npm run start` + `npm run build` commands; the deploy workflow. |

---

## 1. Site map

```
website/
├── docusaurus.config.js    ← THE config (title, theme, navbar, footer, plugins)
├── sidebars.js             ← manual sidebar tree (NOT auto-generated)
├── package.json            ← Node 20+, Docusaurus 3.10.2, React 19
│
├── docs/                   ← the actual docs (markdown + mdx)
│   ├── intro.md            ← site root ("/")
│   ├── getting-started/    ← 4 pages — clone, build, run, verify
│   ├── architecture/       ← 5 pages — layers, data model, plugins, ui
│   ├── build/              ← 6 pages — upstream + per-OS build guides
│   ├── plugins/            ← 7 pages — overview + local set + authoring
│   ├── ci/                 ← 4 pages — workflows + deploy-docs
│   └── reference/          ← 4 pages — coding standards, naming, flags, FAQ
│
├── blog/                   ← changelog (Docusaurus blog plugin)
│
├── src/                    ← custom React + CSS (homepage lives here)
│   ├── components/HomepageFeatures/
│   ├── pages/index.js      ← the homepage React component
│   └── css/custom.css
│
├── static/                 ← served as-is at site root
│   ├── img/                ← logo, favicon, feature SVGs
│   └── robots.txt          ← SEO entry point
│
└── scripts/                ← one-off repair helpers + read-only verifiers
    ├── README.md           ← catalog of what each script does
    ├── strip-ids.py
    ├── rewrite-doc-links.py
    ├── fix-broken-links.py
    ├── fix-absolute-doc-links.py
    └── verify-sidebar-coverage.js
```

---

## 2. The contract every doc must satisfy

`onBrokenLinks: 'throw'` is set in `docusaurus.config.js`. **The build will
fail** if any of the following are not true:

1. **Every `.md` under `docs/` is referenced by `sidebars.js`** — otherwise
   Docusaurus doesn't render it in the sidebar and the build warns.
2. **Every markdown link resolves** — both relative paths (`../foo.md`) and
   absolute Docusaurus paths (`/docs/foo`). Broken links throw.
3. **Every category has a `link` field** pointing at a valid doc ID — the
   category title becomes clickable.

Quick local check before pushing:

```bash
cd website
node scripts/verify-sidebar-coverage.js    # 31/31 docs, 58/58 links
npm run build                              # full build + link integrity
```

---

## 3. Local commands

```bash
cd website
npm install                                # one-time
npm start                                  # dev server at http://localhost:3000/CloudCompare/
npm run build                              # static build → ./build/
npm run serve                              # serve the ./build/ output locally
```

---

## 4. Deployment

Driven by [`.github/workflows/deploy-docs.yml`](../.github/workflows/deploy-docs.yml).

- **Triggers:** push to `master` touching `website/**` or the workflow file; manual dispatch.
- **Concurrency:** `group: pages`, `cancel-in-progress: false` — never cancel a deploy mid-flight.
- **Output:** published to GitHub Pages at <https://bramburn.github.io/CloudCompare/>.

---

## 5. Pointers

- **Top-level project entry** — [`../AGENTS.md`](../AGENTS.md)
- **Architecture overview** (the agent-facing counterpart to this site) — [`../AGENTS-architecture.md`](../AGENTS-architecture.md)
- **Site-specific conventions** — [`./AGENTS-docusaurus.md`](AGENTS-docusaurus.md), [`./AGENTS-style.md`](AGENTS-style.md)
- **Deploy workflow** — [`.github/workflows/deploy-docs.yml`](../.github/workflows/deploy-docs.yml)
- **Local build narrative** — [`../BUILD-LOCAL.md`](../BUILD-LOCAL.md)
