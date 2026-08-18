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

These are all idempotent — running them on an already-correct file is a
no-op. The full build (`npm run build`) re-validates every link as
part of `onBrokenLinks: 'throw'`, so a wrong rewrite fails the build
the same way it would fail in CI.
