CloudCompare
============

Homepage: https://cloudcompare.org

[![GitHub release](https://img.shields.io/github/release/cloudcompare/trunk.svg)](https://github.com/cloudcompare/trunk/releases)

[![Build](https://github.com/bramburn/CloudCompare/workflows/Windows%20Build/badge.svg?branch=master)](https://github.com/bramburn/CloudCompare/actions?query=workflow%3A%22Windows+Build%22)

[![Docs](https://img.shields.io/badge/docs-bramburn.github.io-blue)](https://bramburn.github.io/CloudCompare/)

> **This is the Icelabz Surveying fork** at
> [`bramburn/CloudCompare`](https://github.com/bramburn/CloudCompare).
> It tracks the upstream
> [`CloudCompare/CloudCompare`](https://github.com/CloudCompare/CloudCompare)
> and adds a pinned toolchain, a self-contained Windows bundle, a
> slim CI matrix, and a Docusaurus docs site.

## Documentation

The full docs site is at
**[bramburn.github.io/CloudCompare](https://bramburn.github.io/CloudCompare/)**.
It is built from the [`website/`](website/) directory by
[`.github/workflows/deploy-docs.yml`](.github/workflows/deploy-docs.yml)
on every push to `master`.

Start here:

- [Getting started](https://bramburn.github.io/CloudCompare/docs/getting-started/overview)
  — clone, build, run, verify.
- [Build on Windows](https://bramburn.github.io/CloudCompare/docs/build/windows)
  — the local toolchain and the wrapper scripts at `C:\dev\tools\`.
- [Plugin overview](https://bramburn.github.io/CloudCompare/docs/plugins/overview)
  — the model, the local 18-plugin set, the disabled-priority list.
- [Architecture](https://bramburn.github.io/CloudCompare/docs/architecture/overview)
  — `ccHObject`, `ccPluginManager`, the layer map.
- [CI & deployment](https://bramburn.github.io/CloudCompare/docs/ci/overview)
  — the GitHub Actions workflows and how to trigger them.
- [FAQ](https://bramburn.github.io/CloudCompare/docs/reference/faq)
  — quick answers to the questions that show up repeatedly.

For the **agent-facing** reference (the canonical entry point for AI
coding agents working in this repo), read
[`AGENTS.md`](AGENTS.md). It points at the topical deep-dives in
`AGENTS-*.md`.

For the **long-form** local-build narrative (every issue hit, every
path, every workaround), read
[`BUILD-LOCAL.md`](BUILD-LOCAL.md).

## Introduction

CloudCompare is a 3D point cloud (and triangular mesh) processing software.
It was originally designed to perform comparison between two 3D points clouds
(such as the ones obtained with a laser scanner) or between a point cloud and a
triangular mesh. It relies on an octree structure that is highly optimized for
this particular use-case. It was also meant to deal with huge point
clouds (typically more than 10 million points, and up to 120 million with 2 GB
of memory).

More on CloudCompare [here](http://en.wikipedia.org/wiki/CloudCompare)

## License
------------

This project is under the GPL license: https://www.gnu.org/licenses/gpl-3.0.html

This means that you can use it as is for any purpose. But if you want to distribute
it, or if you want to reuse its code or part of its code in a project you distribute,
you have to comply with the GPL license. In effect, all the code you mix or link with
CloudCompare's code must be made public as well. **This code cannot be used in a
closed source software**.

## Installation
------------

Linux:
- Flathub: https://flathub.org/apps/details/org.cloudcompare.CloudCompare
  ```
  flatpak install flathub org.cloudcompare.CloudCompare
  ```


## Compilation
-----------

Supports: Windows, Linux, and macOS

Refer to the [BUILD.md file](BUILD.md) for up-to-date information.

Basically, you have to:
- clone this repository
- install mandatory dependencies (OpenGL,  etc.) and optional ones if you really need them
(mainly to support particular file formats, or for some plugins)
- launch CMake (from the trunk root)
- enjoy!

## Contributing to CloudCompare
----------------------------

If you want to help us improve CloudCompare or create a new plugin you can start by reading this [guide](CONTRIBUTING.md)

## Supporting the project
--------------------

If you want to help us in another way, you can make donations via <a href='https://donorbox.org/support-cloudcompare' target="_blank"><img src="https://donorbox.org/images/red_logo.png"></a> [donorbox](https://donorbox.org/support-cloudcompare)

Thanks!
