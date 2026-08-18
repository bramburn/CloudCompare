# libs/qCC_io/AGENTS.md

The file-format I/O layer. The **single most important class** is `FileIOFilter` (`include/FileIOFilter.h`); every file-format plugin subclasses it.

**CMake target:** `QCC_IO_LIB` (SHARED, LGPL).
**Public headers:** `include/`.
**Vendored under it:** `extern/dxflib` (default ON), `extern/shapelib` (default ON), optionally GDAL.

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §3 for the public surface; [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §5 for the registry semantics.

## Folder layout

```
qCC_io/
├── CMakeLists.txt
├── include/                       # public headers
├── src/                           # implementations
├── ui/                            # .ui files for Open/Save dialogs
├── cmake/                         # GDAL glue + others
├── extern/
│   ├── dxflib/                    # AutoCAD DXF (vendored)
│   ├── shapelib/                  # ESRI Shapefile (vendored)
│   └── gdal/                      # raster (optional)
└── test/                          # unit tests (BUILD_TESTING=ON)
```

## Built-in formats (always available)

| Header | Format |
|---|---|
| `BinFilter.h` | CloudCompare `.bin` (full fidelity, own format) |
| `AsciiFilter.h` | Generic ASCII cloud/mesh |
| `PlyFilter.h` | Stanford PLY |
| `DxfFilter.h` | AutoCAD DXF (needs `OPTION_USE_DXF_LIB=ON`) |
| `ShpFilter.h` | ESRI Shapefile (needs `OPTION_USE_SHAPE_LIB=ON`) |
| `RasterGridFilter.h` | GeoTIFF etc. (needs `OPTION_USE_GDAL=ON`) |
| `ImageFileFilter.h` | PNG/JPG/etc. attached to clouds |
| `DepthMapFileFilter.h` | Depth maps |

## Don't add new formats here

**Make an I/O plugin instead.** See [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1.2. Reference shipped I/O plugins: `qCoreIO`, `qLASIO`, `qE57IO`, `qDracoIO`, `qFBXIO`, `qPDALIO`, `qPhotoscanIO`, `qRDBIO`, `qStepCADImport`, `qCSVMatrixIO`, `qAdditionalIO`.

If you really must add a **core** format (always-on, no flag), it's a one-line change in `src/FileIOFilter.cpp::InitInternalFilters()`. Read [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §5 first.

## The registry (the contract)

```cpp
// in your plugin
ccIOPluginInterface::FilterList MyPlugin::getFilters()
{
    return { FileIOFilter::Shared(new MyFormatFilter) };
}
```

`ccPluginManager` calls this once at startup; the filters are inserted into the priority-sorted registry. Default priority (`FileIOFilter::DEFAULT_PRIORITY`) is the right choice unless you want to **override** a built-in filter.

## Don't

- Don't `#include` from `src/` in a plugin — only `include/` is public.
- Don't throw across `loadFile()` / `saveToFile()` — return a `CC_FILE_ERROR` enum.
- Don't edit a `.ui` file by hand and expect Qt Designer to round-trip cleanly.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §5 (registry semantics)
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §3
- [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1.2 (how to add an I/O plugin)
