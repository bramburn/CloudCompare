// @ts-check

/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
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
        'getting-started/quick-tour',
      ],
    },
    {
      type: 'category',
      label: 'Build',
      collapsed: true,
      link: {type: 'doc', id: 'build/index'},
      items: [
        'build/upstream',
        'build/windows',
        'build/linux',
        'build/macos',
        'build/troubleshooting',
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
        'plugins/authoring',
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
      ],
    },
    {
      type: 'category',
      label: 'CI & deployment',
      collapsed: true,
      link: {type: 'doc', id: 'ci/overview'},
      items: [
        'ci/github-pages',
        'ci/windows-build',
        'ci/upstream-matrix',
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
      ],
    },
  ],
};

export default sidebars;
