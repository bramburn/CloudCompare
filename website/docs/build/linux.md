---
title: Build on Linux
sidebar_label: Linux
sidebar_position: 3
---

# Build on Linux

The fork does not pin a specific Linux distribution. The instructions below
target Ubuntu 22.04 LTS (the version the upstream CI uses) but the same
pattern works on Debian, Fedora, and Arch with their respective package
managers.

## Required packages (Ubuntu 22.04)

```bash
sudo apt update
sudo apt install -y \
  build-essential cmake ninja-build git \
  libqt6-dev qt6-base-dev qt6-svg-dev qt6-tools-dev qt6-5compat-dev \
  libgl1-mesa-dev libglu1-mesa-dev libeigen3-dev \
  libboost-all-dev libavcodec-dev libavformat-dev libswscale-dev
```

For the full self-contained plugin set the upstream tests, also add:

```bash
sudo apt install -y \
  libxerces-c-dev liblaszip-dev libpcl-dev libpdal-dev \
  libe57-1.1-dev libdraco-dev libzip-dev zlib1g-dev
```

## Configure and build

```bash
git clone --recursive https://github.com/bramburn/CloudCompare.git
cd CloudCompare

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6

cmake --build build --config Release --parallel $(nproc)
```

The output is at `build/qCC/CloudCompare` and
`build/ccViewer/ccViewer`. The plugins land in
`build/plugins/*/libq*.so`.

## Run

```bash
./build/qCC/CloudCompare
```

## Differences from the Windows build

- No `deployqt\` bundle. The binary expects Qt 6 to be on
  `LD_LIBRARY_PATH` (Debian/Ubuntu usually put it in
  `/usr/lib/x86_64-linux-gnu` which is on the default path).
- No vcpkg dance — the `apt install` covers most plugin dependencies.
  Only `qPCL` and `qPDALIO` sometimes need extra symlinks.
- The unit test target is `cc-test-lib`; run with
  `ctest --test-dir build --output-on-failure`.
