// Sidebar-coverage + internal-link verifier.
//
//   1. Walk website/docs/, extract every .md/.mdx, and confirm every
//      doc id appears somewhere in sidebars.js (no orphan docs).
//   2. Scan every doc for markdown links and confirm the targets
//      resolve — both relative paths (../foo.md) and absolute Docusaurus
//      paths (/docs/foo).
//
// Run after any sidebar or cross-link change. Idempotent. Kept in
// website/scripts/ so it can be wired into the deploy-docs workflow
// later as an extra check.
const fs = require('fs');
const path = require('path');

function walk(dir, out = []) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const p = path.join(dir, entry.name);
    if (entry.isDirectory()) walk(p, out);
    else if (entry.name.endsWith('.md') || entry.name.endsWith('.mdx')) out.push(p);
  }
  return out;
}

const ROOT = path.join(__dirname, '..');
const norm = (s) => s.replace(/\\/g, '/');

// 1. Sidebar coverage
const docsDir = path.join(ROOT, 'docs');
const allDocs = walk(docsDir)
  .map((p) => norm(p))
  .map((p) => p.replace(norm(docsDir) + '/', ''))
  .map((p) => p.replace(/\.(mdx?|md)$/, ''));

const sidebarRaw = fs.readFileSync(path.join(ROOT, 'sidebars.js'), 'utf8');
const idPattern = /'([a-z][a-z0-9-]*(?:\/[a-z0-9-]+)*)'/g;
const sidebarIds = new Set();
for (const m of sidebarRaw.matchAll(idPattern)) sidebarIds.add(m[1]);

const missing = allDocs.filter((d) => !sidebarIds.has(d));
console.log(`Sidebar coverage: ${allDocs.length - missing.length}/${allDocs.length} docs referenced`);
for (const m of missing) console.log('  MISSING -', m);

// 2. Internal link integrity
const docs = walk(docsDir);
const mdLink = /\[[^\]]*\]\(([^)]+)\)/g;
let total = 0, broken = [];
for (const f of docs) {
  const c = fs.readFileSync(f, 'utf8');
  const fileRel = path.relative(ROOT, f).replace(/\\/g, '/');
  for (const m of c.matchAll(mdLink)) {
    const link = m[1].split('#')[0];  // drop fragment
    if (!link || link.startsWith('http://') || link.startsWith('https://') || link.startsWith('mailto:')) continue;
    total++;
    let target;
    if (link.startsWith('/docs/')) {
      // Absolute Docusaurus path → resolve against docs/ root.
      // Strip leading '/docs/' and the trailing '.md' if any.
      // trailingSlash: false means '/docs/build/' → 'docs/build/index.md'.
      let id = link.replace(/^\/docs\//, '').replace(/\.(mdx?|md)$/, '');
      id = id.replace(/\/$/, '');
      target = path.join(docsDir, id + '.md');
      if (!fs.existsSync(target)) {
        // Category index fallback: /docs/foo/ → docs/foo/index.md
        const idx = path.join(docsDir, id, 'index.md');
        if (fs.existsSync(idx)) target = idx;
      }
    } else {
      target = path.resolve(path.dirname(f), link);
    }
    if (!fs.existsSync(target)) broken.push(`${fileRel}: ${link}`);
  }
}
console.log(`Internal links: ${total - broken.length}/${total} resolve`);
for (const b of broken) console.log('  BROKEN  -', b);
