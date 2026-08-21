# Doxygen Documentation Progress

## Overview
Systematically add Doxygen comments throughout the CloudCompare application code.

## Doxygen Comment Style
```cpp
/**
 * @brief Brief description of the class or function.
 *
 * Detailed description of the class or function, including:
 * - What it does
 * - How it does it
 * - Any important notes for developers
 *
 * @param param_name Description of parameter
 * @return Description of return value
 * @see RelatedFunction, RelatedClass
 */
```

## Files Status

### qCC/ (Main Application)
| File | Status | Notes |
|------|--------|-------|
| main.cpp | ⬜ Not started | |
| ccConsole.cpp | ✅ Done | Committed: 83a4dacb |
| ccConsole.h | ✅ Done | Committed: 83a4dacb |
| ccUtils.cpp | ✅ Done | Committed: bf35923f |
| ccUtils.h | ✅ Done | Committed: bf35923f |
| ccCommandLineParser.cpp | ✅ Done | Committed: 23f55d26 |
| ccCommandLineParser.h | ✅ Done | Already documented |
| ccRecentFiles.cpp | ✅ Done | Committed: 5c6a47ed |
| ccRecentFiles.h | ✅ Done | Committed: 5c6a47ed |
| ccApplication.cpp | ✅ Done | Already documented |
| ccApplication.h | ✅ Done | Already documented |
| ccMainAppInterface.cpp | ⬜ Not started | |
| ccMainAppInterface.h | ✅ Done | Already documented (CCPluginAPI) |
| ccDBRoot.cpp | ⬜ Not started | |
| ccDBRoot.h | ⬜ Not started | |
| ccGLWindow.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccGLWindow.h | ⬜ Not started | |
| ccOverlayDialog.cpp | ✅ Done | Committed: 3de70ab4 |
| ccOverlayDialog.h | ✅ Done | Committed: 3de70ab4 |

### libs/qCC_db/
| File | Status | Notes |
|------|--------|-------|
| ccViewportParameters.cpp | ✅ Done | Committed: 76821b33 |
| ccViewportParameters.h | ✅ Done | Committed: 76821b33 |
| ccLog.cpp | ✅ Done | Committed: b40a9168 |
| ccLog.h | ✅ Done | Committed: b40a9168 |
| ccHObject.cpp | ⬜ Not started | Very large |
| ccHObject.h | ⬜ Not started | |
| ccPointCloud.cpp | ⬜ Not started | Very large |
| ccPointCloud.h | ⬜ Not started | |
| ccGenericPointCloud.cpp | ⬜ Not started | |
| ccGenericPointCloud.h | ⬜ Not started | |

### libs/qCC_io/
| File | Status | Notes |
|------|--------|-------|
| FileIOFilter.cpp | ⬜ Not started | |
| FileIOFilter.h | ✅ Done | Already well documented |
| AsciiSaveDlg.cpp | ⬜ Not started | |
| AsciiSaveDlg.h | ⬜ Not started | |

### libs/CCCoreLib/
| File | Status | Notes |
|------|--------|-------|
| GenericDistribution.h | ✅ Done | Committed: N/A (in git submodule — not committable from here) |
| DgmOctree.cpp | ⬜ Not started | Very large |
| DgmOctree.h | ⬜ Not started | |
| ScalarField.cpp | ⬜ Not started | |
| ScalarField.h | ⬜ Not started | |

### libs/qCC_glWindow/
| File | Status | Notes |
|------|--------|-------|
| ccGLWindowInterface.cpp | ⬜ Not started | Very large (~7000 lines) |
| ccGLWindowInterface.h | ✅ Done | Already well documented |
| ccViewportParameters.h | ✅ Done | (see qCC_db above) |

### libs/CCAppCommon/
| File | Status | Notes |
|------|--------|-------|
| ccApplication.cpp | ⬜ Not started | |
| ccApplication.h | ⬜ Not started | |

### plugins/core/Standard/ (Sample)
| File | Status | Notes |
|------|--------|-------|
| qCSF.cpp | ⬜ Not started | |
| qCSF.h | ⬜ Not started | |
| qM3C2.cpp | ⬜ Not started | |
| qM3C2.h | ⬜ Not started | |

---

## Progress Summary
- **Total files tracked**: ~40+
- **Completed**: 16
- **In progress**: 0
- **Percentage**: ~40%

## Submodule Note
`libs/qCC_db/extern/CCCoreLib/` is a git submodule. Files inside it (e.g., GenericDistribution.h) cannot be committed from the parent repo. Commit inside the submodule separately.

## Commit Log
| Commit | Files | Notes |
|--------|-------|-------|
| 5c6a47ed | ccRecentFiles.h, ccRecentFiles.cpp | ~ path contraction, auto-cleanup |
| b40a9168 | ccLog.h, ccLog.cpp | Static facade, backup system, DEBUG_FLAG |
| 76821b33 | ccViewportParameters.h, ccViewportParameters.cpp | Camera math, serialization |
| 3de70ab4 | ccOverlayDialog.h, ccOverlayDialog.cpp | Lifecycle, eventFilter, shortcuts |
| 23f55d26 | ccCommandLineParser.cpp | Enhanced Doxygen with @param/@return/@tparam |
| bf35923f | ccUtils.h, ccUtils.cpp | Enhanced Doxygen for ccUtils namespace |
| 83a4dacb | ccConsole.h, ccConsole.cpp, doxygen-state.md | Full Doxygen for ccConsole classes |

## Last Updated
2026-08-21
