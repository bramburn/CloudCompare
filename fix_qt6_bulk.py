"""
Bulk Qt 6 compatibility fixes for CloudCompare.
Fixes: Q_EMIT->emit, Q_SIGNALS->signals:, Q_SLOT->slots:, QStandardPaths
Excludes vendored libs (QCustomPlot, qPhotoscanIO/quazip test, hidapi).
"""
import os
import re

ROOT = r"C:\dev\CloudCompare"

# Files/dirs to skip entirely (vendored or out of scope)
SKIP_PATHS = {
    r"qCC\extern\QCustomPlot",
    r"plugins\core\IO\qPhotoscanIO\extern\quazip",
    r"libs\CCAppCommon\devices\3dConnexion\extern\hidapi",
}

def should_skip(path):
    for skip in SKIP_PATHS:
        if skip in path:
            return True
    return False

# Q_EMIT files and their counts
Q_EMIT_FILES = [
    (r"plugins\core\Standard\qPCL\PclUtils\filters\StatisticalOutliersRemover.cpp", 1),
    (r"plugins\core\Standard\qPCL\PclUtils\filters\NormalEstimation.cpp", 1),
    (r"plugins\core\Standard\qPCL\PclUtils\filters\MLSSmoothingUpsampling.cpp", 1),
    (r"plugins\core\Standard\qPCL\PclUtils\filters\FastGlobalRegistrationFilter.cpp", 1),
    (r"plugins\core\Standard\qPCL\PclUtils\filters\ExtractSIFT.cpp", 1),
    (r"plugins\core\Standard\qPCL\PclUtils\filters\BaseFilter.cpp", 1),
    (r"plugins\core\Standard\qFacets\src\stereogramDlg.cpp", 2),
    (r"plugins\core\Standard\qMPlane\src\ccMPlaneDlg.cpp", 7),
    (r"plugins\core\Standard\qJSonRPCPlugin\src\jsonrpcserver.cpp", 1),
    (r"plugins\core\Standard\qCloudLayers\src\ccAsprsModel.cpp", 5),
    (r"libs\qCC_io\src\PlyOpenDlg.cpp", 2),
    (r"libs\qCC_glWindow\src\ccGLWindowInterface.cpp", 43),
    (r"libs\qCC_db\src\ccOctree.cpp", 1),
    (r"libs\qCC_db\src\ccClipBox.cpp", 5),
    (r"libs\CCAppCommon\src\ccDisplaySettingsDlg.cpp", 2),
    (r"libs\CCPluginAPI\src\ccColorScaleEditorWidget.cpp", 6),
    (r"libs\CCPluginAPI\src\ccColorScaleSelector.cpp", 1),
    (r"libs\CCAppCommon\devices\3dConnexion\Mouse3DInput_hid.cpp", 4),
    (r"libs\CCAppCommon\devices\3dConnexion\Mouse3DInput.cpp", 7),
    (r"libs\CCPluginAPI\src\ccOverlayDialog.cpp", 3),
]

# Q_SIGNALS files
Q_SIGNALS_FILES = [
    r"libs\qCC_io\include\PlyOpenDlg.h",
    r"libs\qCC_glWindow\include\ccGLWindowSignalEmitter.h",
    r"libs\CCAppCommon\include\ccDisplaySettingsDlg.h",
    r"libs\CCAppCommon\devices\3dConnexion\Mouse3DInput_hid.h",
    r"libs\CCAppCommon\devices\3dConnexion\Mouse3DInput.h",
    r"libs\qCC_db\include\ccOctree.h",
    r"libs\qCC_db\include\ccClipBox.h",
    r"libs\CCPluginAPI\include\ccOverlayDialog.h",
    r"libs\CCPluginAPI\include\ccColorScaleSelector.h",
    r"libs\CCPluginAPI\include\ccColorScaleEditorWidget.h",
    r"plugins\core\Standard\qPCL\PclUtils\filters\BaseFilter.h",
    r"plugins\core\Standard\qCloudLayers\include\ccAsprsModel.h",
    r"plugins\core\Standard\qMPlane\src\ccMPlaneDlg.h",
    r"plugins\core\Standard\qJSonRPCPlugin\include\jsonrpcserver.h",
]

# Q_SLOT files (project code only, not QCustomPlot)
Q_SLOT_FILES = [
    r"libs\qCC_glWindow\include\ccGLWindow.h",
    r"libs\qCC_glWindow\include\ccGLWindowStereo.h",
]

# Also fix Q_SLOT in plugin headers (not in QCustomPlot)
Q_SLOT_PLUGIN_FILES = [
    r"plugins\core\Standard\qCloudLayers\include\ccCloudLayersDlg.h",
    r"plugins\core\Standard\qCloudLayers\include\ccAsprsModel.h",
    r"plugins\core\Standard\qCork\include\ccCorkDlg.h",
    r"plugins\core\Standard\qMeshBoolean\include\ccMeshBooleanDialog.h",
    r"plugins\core\IO\qLASIO\include\LasSaveDialog.h",
]

# QStandardPaths fixes
QSTANDARDPATHS_FILES = [
    (r"libs\CCPluginAPI\src\ccRenderToFileDlg.cpp", [
        ("QStandardPaths::DocumentsLocation", "QStandardPaths::StandardLocation::Documents"),
        ("QStandardPaths::HomeLocation", "QStandardPaths::StandardLocation::Home"),
        ("QStandardPaths::TempLocation", "QStandardPaths::StandardLocation::Temp"),
    ]),
    (r"plugins\core\Standard\q3DMASC\qTrain3DMASCDialog.cpp", [
        ("QStandardPaths::DocumentsLocation", "QStandardPaths::StandardLocation::Documents"),
    ]),
    (r"plugins\core\Standard\cc3DFin\src\cc3DFinDlg.cpp", [
        ("QStandardPaths::DocumentsLocation", "QStandardPaths::StandardLocation::Documents"),
        ("QStandardPaths::DesktopLocation", "QStandardPaths::StandardLocation::Desktop"),
        ("QStandardPaths::HomeLocation", "QStandardPaths::StandardLocation::Home"),
    ]),
]

def fix_file(path, replacements):
    """Apply a list of (old, new) string replacements to a file."""
    full = os.path.join(ROOT, path)
    if not os.path.exists(full):
        print(f"  MISSING: {path}")
        return False
    with open(full, "r", encoding="utf-8") as f:
        content = f.read()
    original = content
    for old, new in replacements:
        content = content.replace(old, new)
    if content != original:
        with open(full, "w", encoding="utf-8") as f:
            f.write(content)
        return True
    return False

def fix_file_regex(path, pattern, replacement):
    """Apply a regex replacement to a file."""
    full = os.path.join(ROOT, path)
    if not os.path.exists(full):
        print(f"  MISSING: {path}")
        return False
    with open(full, "r", encoding="utf-8") as f:
        content = f.read()
    new_content, n = re.subn(pattern, replacement, content)
    if n > 0:
        with open(full, "w", encoding="utf-8") as f:
            f.write(new_content)
        return n
    return 0

# --- Fix Q_EMIT -> emit ---
print("=== Q_EMIT -> emit ===")
total_emit = 0
for path, count in Q_EMIT_FILES:
    n = fix_file_regex(path, r'\bQ_EMIT\b', 'emit')
    if n:
        print(f"  {path}: {n} replacements")
        total_emit += n
print(f"Total: {total_emit}")

# --- Fix Q_SIGNALS -> signals: ---
print("\n=== Q_SIGNALS -> signals: ===")
total_signals = 0
for path in Q_SIGNALS_FILES:
    n = fix_file_regex(path, r'\bQ_SIGNALS\b', 'signals:')
    if n:
        print(f"  {path}: {n} replacements")
        total_signals += n
print(f"Total: {total_signals}")

# --- Fix Q_SLOT -> slots: ---
print("\n=== Q_SLOT -> slots: ===")
total_slot = 0
for path in Q_SLOT_FILES:
    n = fix_file_regex(path, r'\bQ_SLOT\b', 'slots:')
    if n:
        print(f"  {path}: {n} replacements")
        total_slot += n
for path in Q_SLOT_PLUGIN_FILES:
    n = fix_file_regex(path, r'\bQ_SLOT\b', 'slots:')
    if n:
        print(f"  {path}: {n} replacements")
        total_slot += n
print(f"Total: {total_slot}")

# --- Fix QStandardPaths ---
print("\n=== QStandardPaths enum migration ===")
total_qsp = 0
for path, replacements in QSTANDARDPATHS_FILES:
    changed = fix_file(path, replacements)
    if changed:
        print(f"  {path}: fixed")
        total_qsp += len(replacements)
print(f"Total: {total_qsp} replacements")

print("\n=== Done ===")
