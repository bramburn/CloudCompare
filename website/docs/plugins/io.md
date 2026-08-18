---
title: I/O plugins
sidebar_label: I/O
sidebar_position: 5
---

# I/O plugins

I/O plugins add a file format. They register under
**File > Open** and **File > Save** as new file-extension filters.

## The interface

```cpp
class FileIOFilter : public ccPluginInterface {
public:
  // The file extensions this filter handles, e.g. {"las", "laz"}.
  virtual QStringList getSupportedFormats() const = 0;

  // True if this filter is willing to try to load `filename`.
  // Most filters check the extension here.
  virtual bool canLoad(QString filename) const = 0;

  // True if this filter can write the given entity tree.
  // Usually true for clouds and meshes, false for custom hierarchies.
  virtual bool canSave(CC_CLASS_ENUM entityType) const = 0;

  // The actual load. Returns a list of new entities to add to the
  // db-tree. Use ccHObjectCaster to walk the entity tree.
  virtual CC_FILE_ERROR loadFile(
    QString filename,
    ccHObject& container,
    LoadParameters& parameters) = 0;

  // The actual save. Returns CC_FILE_ERROR.
  virtual CC_FILE_ERROR saveToFile(
    ccHObject* entity,
    QString filename,
    SaveParameters& parameters) = 0;
};
```

`CC_FILE_ERROR` is an enum in `CCCoreLib`. The interesting values:
`CC_FERR_NO_ERROR`, `CC_FERR_BAD_ARGUMENT`, `CC_FERR_UNKNOWN_FILE`,
`CC_FERR_READING`, `CC_FERR_WRITING`, `CC_FERR_NOT_ENOUGH_MEMORY`,
`CC_FERR_CONSOLE_ERROR` (emitted to the user as an error dialog).

## A canonical example: qLASIO

`qLASIO` reads and writes LAS / LAZ. The relevant files:

```
plugins/core/IO/qLASIO/
├── CMakeLists.txt
├── LASIOFilter.h
├── LASIOFilter.cpp
├── LasOpenDialog.h
├── LasOpenDialog.cpp
├── LasSaveDialog.h
├── LasSaveDialog.cpp
└── Resources/
    └── icon.png
```

`LASIOFilter::loadFile` flow:

1. Open the file with `laszip_open_reader(lasio_filename, &reader)`.
2. Iterate the point records and build a `ccPointCloud`.
3. If the LAS file has `LASpoint::Classification` set, attach it as a
   scalar field called "Classification".
4. Add the cloud to the `container` (`container.addChild(cloud)`).
5. Return `CC_FERR_NO_ERROR`.

`LASIOFilter::saveToFile` flow:

1. Walk the entity tree to find compatible point clouds
   (`ccHObjectCaster::ToPointCloud(entity)`).
2. If the user has selected specific scalar fields to export, set them
   on the `laszip_point` writer.
3. Iterate the cloud's points and write each.
4. Return `CC_FERR_NO_ERROR`.

## Multiple filters per plugin

A single plugin DLL can host **multiple** `FileIOFilter` subclasses. The
`qCoreIO` plugin, for example, ships one filter per format (OBJ, PLY,
BIN, PTX, E57 (legacy), VTK, …). Each filter's `getSupportedFormats()`
returns a different extension list; the host shows them in
**File > Open** as separate entries.

## When to use the I/O pattern

Reach for an I/O plugin when:

- You want to **add a new file format** (the most common case).
- The format is **read/write both ways** — write a save path even if
  it's lossy, it lets users round-trip their data.
- The format has its own **metadata** (CRS, scan angle, GPS time, …)
  that should land in the loaded entity as `ccGenericMetadata` or
  scalar fields.

If your tool transforms an existing format's data on the fly (e.g. a
"decompress and decimate" wrapper), prefer a Standard plugin that
loads through an existing I/O filter and then operates on the result.
