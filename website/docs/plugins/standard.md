---
title: Standard plugins
sidebar_label: Standard
sidebar_position: 4
---

# Standard plugins

Standard plugins are analysis tools. They register under
**Plugins > Standard Plugins** in the menu and use the
`IStandardPlugin` interface to plug into the main app.

## The interface

The key types live in `libs/CCPluginAPI/`:

```cpp
class IStandardPlugin : public ccPluginInterface {
public:
  // The descriptor shown in the menu
  virtual QString getName() const = 0;
  virtual QString getDescription() const = 0;
  virtual QIcon getIcon() const = 0;

  // Called once on load. Use this to grab the app interface and
  // register actions / dialogs.
  virtual bool onLoad(ccMainAppInterface* app) = 0;

  // The actual entry point. Most plugins push a dialog from here.
  virtual void doAction(ccMainAppInterface* app) = 0;

  // Optional: return a new instance when the host calls getActions().
  virtual QList<QAction*> getActions() override;
};
```

The `ccMainAppInterface` gives the plugin access to the db-tree, the
current selection, the 3D viewport, and the console. See
[`ccMainAppInterface.h`](https://github.com/bramburn/CloudCompare/blob/master/libs/CCPluginAPI/include/ccMainAppInterface.h)
in the upstream repo for the full surface.

## A canonical example: qCSF

`qCSF` (Cloth Simulation Filtering) is the smallest Standard plugin that
still does real work. The relevant files:

```
plugins/core/Standard/qCSF/
├── CMakeLists.txt
├── qCSF.h           // class Q_CSF : public QObject, public ccStdPluginInterface
├── qCSF.cpp
├── qCSFDlg.h        // the dialog
├── qCSFDlg.cpp
├── qCSFDlg.ui       // Qt Designer file
└── Resources/
    └── icon.png
```

The flow:

1. **Constructor** — `Q_CSF::Q_CSF(QObject* parent) : QObject(parent) {}`
2. **`getActions()`** — builds a single `QAction` with the icon and the
   "CSF Ground Filter" label, and returns a list of one.
3. **Menu registration** — the main app picks up the action and puts it
   in the right place.
4. **`Q_CSF::doAction()`** — gets the active GL window via
   `m_app->getActiveGLWindow()`, checks for a point cloud in the db-tree,
   and opens `qCSFDlg`.
5. **`qCSFDlg::accept()`** — runs the actual CSF algorithm in
   `CCCoreLib`, applies the result to the loaded cloud as a new scalar
   field, and refreshes the viewport.

## The Q_OBJECT / export dance

Every Standard plugin has this at the bottom of its main `.cpp`:

```cpp
Q_EXPORT_PLUGIN2(ExamplePlugin, ExamplePlugin);
```

Where `ExamplePlugin` is the class name. `Q_EXPORT_PLUGIN2` is the macro
that puts the `qt_plugin_instance` symbol in the shared library. The
`ccPluginManager` calls `qobject_cast<IStandardPlugin*>(QPluginLoader(...))`
on each `.dll` and adds the result to its plugin list.

## When to use the Standard pattern

Reach for a Standard plugin when your tool:

- Operates on the **current selection** of points in the db-tree.
- Needs a **dialog** for parameters.
- Produces a **new entity** (cloud, mesh, scalar field) that should
  appear in the db-tree.

If your tool is an OpenGL post-filter (operates on the viewport's
framebuffer), use a [GL plugin](/docs/plugins/gl) instead. If it reads or writes a file
format, use an [I/O plugin](/docs/plugins/io). If it doesn't fit any of these three,
you might be looking at a core change — see
[Architecture / Plugin system](/docs/architecture/plugin-system) for the
decision matrix.
