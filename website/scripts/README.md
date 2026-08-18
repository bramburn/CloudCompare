# website/scripts

One-off helpers used to repair the docs site's cross-doc links after a
batch of file moves. They are not part of the build pipeline; they are
here for the next time the link graph needs surgery.

| Script | What it does |
|---|---|
| `strip-ids.py` | Strip the `id:` front-matter line from every doc except `intro.md` (so the default file-path ID is used and the URL is `<folder>/<file>` instead of `<folder>/<short-id>`). |
| `rewrite-doc-links.py` | Convert every relative `](sibling)` / `](../sibling)` markdown link in `docs/` to an absolute `](/docs/<resolved>)` link, resolved against the current file's docs-relative folder. |
| `fix-broken-links.py` | Older first-pass fix that only addressed a subset of the broken-link patterns. Superseded by `fix-absolute-doc-links.py`. |
| `fix-absolute-doc-links.py` | Correct the corrupted `/docs/<short>` paths that the earlier rewrites produced, scoping each replacement to the current file's docs-relative folder. Idempotent. |
| `verify-sidebar-coverage.js` | Read-only checker. Walks `docs/` and confirms every doc id appears in `sidebars.js`, and that every internal markdown link resolves (handles both relative paths and absolute `/docs/...` paths, plus the trailing-slash → `index.md` convention). Run before/after a sidebar or doc-graph change. |
| `init-queue.js` | Populate the LLM auto-docs queue at `../.docstate.json`. Walks `libs/`, `plugins/`, `qCC/`, `ccViewer/` (excluding `extern/`, `build/`, `.git/`, `website/`), produces a JSON list of every `.cpp`/`.h`/`.hpp`/`.cc`/`.cxx`/`CMakeLists.txt`, sorted by priority (headers first → sources → CMakeLists), with collision-free `docPath` mapping into the future `docs/modules/` subtree. Flags: `--dry-run` (counts only), `--merge` (preserve existing statuses), `--reset` (force every item back to `pending`), `--exclude-vendored` (skip submodule-installed plugins like `cc3DFin`, `qCanupo`). **Not invoked by the build; run once initially and re-run with `--merge` when new files are added.** |

These are all idempotent — running them on an already-correct file is a
no-op. The full build (`npm run build`) re-validates every link as
part of `onBrokenLinks: 'throw'`, so a wrong rewrite fails the build
the same way it would fail in CI.
