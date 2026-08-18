---
title: GitHub Pages
sidebar_label: GitHub Pages
sidebar_position: 2
---

# GitHub Pages

This site is published to GitHub Pages on every push to `master` that
touches `website/**` or the workflow file itself.

The live site lives at **`https://bramburn.github.io/CloudCompare/`**.

## The workflow

`.github/workflows/deploy-docs.yml`:

```yaml
name: Deploy docs site to GitHub Pages
on:
  push:
    branches: [master]
    paths: ['website/**', '.github/workflows/deploy-docs.yml']
  workflow_dispatch:

permissions:
  contents: read
  pages: write
  id-token: write

concurrency:
  group: pages
  cancel-in-progress: false

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0
      - uses: actions/setup-node@v4
        with:
          node-version: 22
          cache: npm
          cache-dependency-path: website/package-lock.json
      - run: npm ci
        working-directory: website
      - run: npm run build
        working-directory: website
      - uses: actions/configure-pages@v5
      - uses: actions/upload-pages-artifact@v3
        with:
          path: website/build
  deploy:
    needs: build
    runs-on: ubuntu-latest
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    steps:
      - id: deployment
        uses: actions/deploy-pages@v4
```

The workflow:

1. Triggers on push to `master` when `website/**` or the workflow
   file changed. Also `workflow_dispatch`-able from the GitHub UI.
2. Checks out the repo with full history (`fetch-depth: 0`) so
   Docusaurus's `lastUpdated` timestamps resolve.
3. Sets up Node 22 with the npm cache keyed on
   `website/package-lock.json`.
4. Runs `npm ci` (not `npm install`) — `npm ci` is reproducible and
   faster when the lockfile is up to date.
5. Runs `npm run build` — Docusaurus 3 builds the static site to
   `website/build/`.
6. Uploads `website/build/` as a Pages artifact.
7. The `deploy` job uses the official
   `actions/deploy-pages@v4` to push the artifact to the
   `gh-pages` branch.

## GitHub repo settings

For the workflow to work, the GitHub Pages settings on the
`bramburn/CloudCompare` repo must be:

- **Source**: "GitHub Actions" (not "Deploy from a branch").
- **Visibility**: public.

Both are the default for a public repo, but it's worth verifying
under **Settings > Pages** if the deploy is failing silently.

## Custom domain

To use a custom domain (e.g. `docs.bramburn.dev`):

1. Add a `static/CNAME` file to the Docusaurus site containing
   `docs.bramburn.dev` (one line, no protocol).
2. Add a CNAME record from `docs.bramburn.dev` to
   `bramburn.github.io`.
3. Re-trigger the workflow. The CNAME is preserved across deploys
   because it's part of the static site.

## Verifying the deploy

1. Push to `master` (or merge a PR).
2. Watch **GitHub > Actions > "Deploy docs site to GitHub Pages"**.
3. The "deploy" job's environment link points to the live URL.
4. Hard-refresh the browser at
   `https://bramburn.github.io/CloudCompare/`.

The whole build is ~1-2 minutes on the GitHub-hosted runners; the
slowest step is `npm ci` (~30&nbsp;s with a warm cache).

## Local preview

Before pushing, preview the site locally:

```bash
cd website
npm install
npm start          # http://localhost:3000/CloudCompare/
```

The dev server hot-reloads on file changes. Use `Ctrl+C` to stop.
