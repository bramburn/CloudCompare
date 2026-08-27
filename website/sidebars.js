// @ts-check

/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
//
// Sidebar is organised by *concern*, not by folder — the same principle
// as the upstream CloudCompare Wiki's table of contents. Each top-level
// category answers a different reader question:
//
//   1. Getting started       — "how do I clone, build, and run the fork?"
//   2. Architecture          — "what is the layered structure of the code?"
//   3. Build & deployment    — "how do I build on my platform, and how do
//                                the GitHub Actions workflows ship things?"
//                              (combines the per-OS build guides with the
//                               CI/deployment documentation, since both
//                               are about getting the fork out the door)
//   4. Workflows             — "what does a real surveying-company job look
//                                like, end to end?" (the four canonical
//                                pipelines: topo, monitoring, stockpile,
//                                as-built vs design)
//   5. Cookbook              — "how do I do X?" task recipes
//                                (alignment, distance, cropping, scalar
//                                fields, measurements, batch CLI)
//   6. Plugins               — "what is the plugin model, what's enabled,
//                                how do I author one?" (the subsystem
//                                deep-dive — fills the report's "Modules"
//                                slot for this fork)
//   7. Reference             — "where do I look up naming, build flags,
//                                DB tree, performance, formats, and FAQ?"
//
// Every .md under website/docs/ EXCEPT intro.md must appear here, or
// Docusaurus won't render it in the sidebar. intro.md is the explicit
// site root ("/").
const sidebars = {
  mainSidebar: [
    'intro',
    {
      type: 'category',
      label: 'Getting started',
      collapsed: false,
      link: {type: 'doc', id: 'getting-started/index'},
      items: [
        'getting-started/overview',
        'getting-started/prerequisites',
        'getting-started/running',
        'getting-started/quick-tour',
      ],
    },
    {
      type: 'category',
      label: 'Why this fork?',
      collapsed: false,
      link: {type: 'doc', id: 'fork/index'},
      items: [
        'fork/rust-migration',
        'fork/experimental-workspace',
        'fork/topview-registration',
        'fork/upstream-vs-fork',
      ],
    },
    {
      type: 'category',
      label: 'Architecture',
      collapsed: true,
      link: {type: 'doc', id: 'architecture/overview'},
      items: [
        'architecture/layers',
        'architecture/data-model',
        'architecture/plugin-system',
        'architecture/ui',
        'architecture/render-pipeline',
      ],
    },
    {
      type: 'category',
      label: 'Build & deployment',
      collapsed: true,
      link: {type: 'doc', id: 'build/index'},
      items: [
        'build/upstream',
        'build/windows',
        'build/linux',
        'build/macos',
        'build/troubleshooting',
        'testing/index',
        'ci/overview',
        'ci/github-pages',
        'ci/windows-build',
        'ci/upstream-matrix',
      ],
    },
    {
      type: 'category',
      label: 'Workflows',
      collapsed: true,
      link: {type: 'doc', id: 'workflows/index'},
      items: [
        'workflows/topo',
        'workflows/monitoring',
        'workflows/stockpile',
        'workflows/as-built',
      ],
    },
    {
      type: 'category',
      label: 'Cookbook',
      collapsed: true,
      link: {type: 'doc', id: 'cookbook/index'},
      items: [
        'cookbook/icp',
        'cookbook/point-pairs',
        'cookbook/cloud-to-cloud-distance',
        'cookbook/cloud-to-mesh-distance',
        'cookbook/crop',
        'cookbook/subsample',
        'cookbook/cross-section',
        'cookbook/volume-25d',
        'cookbook/scalar-field-colors',
        'cookbook/measurement',
        'cookbook/screenshot',
        'cookbook/batch-cli',
        'cookbook/merge',
        'cookbook/save-session',
      ],
    },
    {
      type: 'category',
      label: 'Plugins',
      collapsed: true,
      link: {type: 'doc', id: 'plugins/overview'},
      items: [
        'plugins/local-set',
        'plugins/disabled-priority',
        'plugins/standard',
        'plugins/io',
        'plugins/gl',
        'plugins/qProjectManager',
        'plugins/qRegistration',
        'plugins/qScanBrowser',
        'plugins/authoring',
      ],
    },
    {
      type: 'category',
      label: 'Reference',
      collapsed: true,
      items: [
        'reference/coding-standards',
        'reference/naming',
        'reference/build-flags',
        'reference/faq',
        'reference/db-tree',
        'reference/performance',
        'reference/formats',
      ],
    },
    {
      type: 'category',
      label: 'Research',
      collapsed: true,
      link: {type: 'doc', id: 'research/index'},
      items: [
        'research/mobile-slam-postprocessing',
      ],
    },
  ],
};

export default sidebars;
