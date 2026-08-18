// @ts-check
// `@type` JSDoc annotations allow editor autocompletion and type checking
// (when paired with `@ts-check`).
// There are various equivalent ways to declare your Docusaurus config.
// See: https://docusaurus.io/docs/api/docusaurus-config

import {themes as prismThemes} from 'prism-react-renderer';

// This runs in Node.js - Don't use client-side code here (browser APIs, JSX...)

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'CloudCompare',
  tagline: '3D point cloud & mesh processing',

  // Top-level fields not in the standard schema go under customFields so
  // they can still be read by the theme (e.g. via useDocusaurusContext).
  customFields: {
    description:
      'Documentation for the CloudCompare fork maintained by Icelabz Surveying. ' +
      'C++17 / Qt 6 / OpenGL 2.1+ / CMake 3.10+. Plugins for LAS, E57, PCL, M3C2, CSF, and more.',
  },

  favicon: 'img/favicon.ico',

  // Future flags, see https://docusaurus.io/docs/api/docusaurus-config#future
  future: {
    v4: true,
  },

  // Set the production url of your site here
  url: 'https://bramburn.github.io',
  // Set the /<baseUrl>/ pathname under which your site is served.
  // For GitHub Pages project sites, the baseUrl is '/<projectName>/'.
  baseUrl: '/CloudCompare/',

  // GitHub Pages deployment config.
  organizationName: 'bramburn',
  projectName: 'CloudCompare',
  deploymentBranch: 'gh-pages',
  trailingSlash: false,

  onBrokenLinks: 'throw',
  markdown: {
    hooks: {
      onBrokenMarkdownLinks: 'warn',
    },
  },

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        // Sitemap is bundled into preset-classic; configure its options
        // here instead of registering a second plugin instance at the
        // top level (that would collide on the plugin ID "default").
        //   changefreq/priority are Sitemap-protocol fields.
        //   ignorePatterns keeps tag-archive noise out of the crawl
        //   surface so search engines don't index /tags/**/...
        sitemap: {
          changefreq: 'daily',
          priority: 0.5,
          ignorePatterns: ['/tags/**'],
        },
        docs: {
          sidebarPath: './sidebars.js',
          // Docs live at /docs — the standard Docusaurus convention.
          // Override with `routeBasePath: '/'` if you want docs at the
          // site root (the home page is then a separate React page).
          editUrl:
            'https://github.com/bramburn/CloudCompare/tree/master/website/',
          showLastUpdateAuthor: true,
          showLastUpdateTime: true,
        },
        blog: {
          blogTitle: 'CloudCompare changelog',
          blogDescription:
            'Release notes, plugin updates, and surveying-workflow changes for the Icelabz fork of CloudCompare.',
          blogSidebarTitle: 'Changelog',
          postsPerPage: 10,
          showReadingTime: false,
          feedOptions: {
            type: ['rss', 'atom'],
            xslt: true,
          },
          editUrl:
            'https://github.com/bramburn/CloudCompare/tree/master/website/',
          onInlineTags: 'warn',
          onInlineAuthors: 'warn',
          onUntruncatedBlogPosts: 'warn',
        },
        theme: {
          customCss: './src/css/custom.css',
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      // Open Graph / Twitter card image. Served at the site root for
      // share previews on Twitter, LinkedIn, Slack, etc.
      image: 'img/social-card.png',
      colorMode: {
        defaultMode: 'light',
        respectPrefersColorScheme: true,
      },
      metadata: [
        {name: 'twitter:card', content: 'summary_large_image'},
        {name: 'twitter:site', content: '@bramburn'},
        {name: 'twitter:title', content: 'CloudCompare'},
        {
          name: 'twitter:description',
          content:
            'Open-source 3D point cloud and mesh processing. C++17 / Qt 6 / OpenGL 2.1+ / CMake 3.10+.',
        },
        {name: 'twitter:image', content: 'https://bramburn.github.io/CloudCompare/img/social-card.png'},
      ],
      docs: {
        sidebar: {
          hideable: true,
          autoCollapseCategories: true,
        },
      },
      navbar: {
        title: 'CloudCompare',
        logo: {
          alt: 'CloudCompare logo',
          src: 'img/logo.png',
          srcDark: 'img/logo.png',
          width: 32,
          height: 32,
        },
        items: [
          {
            type: 'docSidebar',
            sidebarId: 'mainSidebar',
            position: 'left',
            label: 'Docs',
          },
          {to: '/blog', label: 'Changelog', position: 'left'},
          {
            href: 'https://github.com/bramburn/CloudCompare',
            label: 'GitHub',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'light',
        links: [
          {
            title: 'Documentation',
            items: [
              {label: 'Getting started', to: '/docs/getting-started/overview'},
              {label: 'Build on Windows', to: '/docs/build/windows'},
              {label: 'Plugin development', to: '/docs/plugins/overview'},
              {label: 'Architecture', to: '/docs/architecture/overview'},
            ],
          },
          {
            title: 'Resources',
            items: [
              {
                label: 'Upstream CloudCompare',
                href: 'https://github.com/CloudCompare/CloudCompare',
              },
              {
                label: 'cloudcompare.org',
                href: 'https://cloudcompare.org',
              },
              {
                label: 'BUILD.md',
                to: '/docs/build/upstream',
              },
            ],
          },
          {
            title: 'This fork',
            items: [
              {
                label: 'bramburn/CloudCompare',
                href: 'https://github.com/bramburn/CloudCompare',
              },
              {
                label: 'Actions',
                href: 'https://github.com/bramburn/CloudCompare/actions',
              },
              {
                label: 'AGENTS.md',
                href: 'https://github.com/bramburn/CloudCompare/blob/master/AGENTS.md',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} CloudCompare contributors. Built with Docusaurus.`,
      },
      prism: {
        theme: prismThemes.github,
        darkTheme: prismThemes.dracula,
        // `cpp` is the critical addition for a C++ codebase — every code
        // block tagged `cpp` (the default for `.cpp`/`.h`/`.cc` files)
        // gets full C++17 syntax highlighting under both light and dark
        // themes. The other languages cover the build / shell / config
        // surface of the repo (CMake, PowerShell, bash, JSON, diff).
        additionalLanguages: [
          'bash',
          'cmake',
          'cpp',
          'diff',
          'json',
          'powershell',
        ],
      },
    }),
};

export default config;
