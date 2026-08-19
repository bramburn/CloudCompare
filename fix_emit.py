#!/usr/bin/env python3
"""Remove 'emit' keyword from Qt signal emissions (Qt 6 compatibility).

In Qt 5, 'emit signal(...)' was required.
In Qt 6, 'emit' is gone — just call 'signal(...)' directly.
"""
import os
import re
import sys

# Files that use emit as a non-Qt identifier (skip these)
SKIP_DIRS = {
    'extern',
    'third_party',
    '3rd-party',
    '.git',
    'PiRecon',   # PoissonRecon JPEG uses emit as param name
    'assimp',
    'tbb',
    'PclUtils',  # PCL library
    'glTF2',     # assimp glTF
    'FBX',       # assimp FBX
    'Pbrt',      # assimp Pbrt
    'examples',  # dlib logger example
}

# Files that are known to have Qt emit (CloudCompare source only)
ALLOWED_FILES = [
    'qCC/test',
    'qCC/db_tree',
    'qCC/gl_window',
    'qCC/ccHistogramWindow.cpp',
    'qCC/ccGraphicalSegmentationTool.cpp',
    'libs/qCC_db/src',
    'libs/qCC_io/src',
    'libs/qCC_glWindow/src',
    'libs/CCAppCommon/src',
    'libs/CCPluginAPI/src',
    'plugins/core/Standard/qMPlane/src',
    'plugins/core/Standard/qCloudLayers/src',
    'plugins/core/Standard/qG3Point/src',
    'plugins/core/Standard/qFacets/src',
    'plugins/core/Standard/qJSonRPCPlugin/src',
    'plugins/core/IO/qLASIO/src',
]

def should_process(filepath):
    """Check if file should be processed."""
    filepath = filepath.replace('\\', '/')
    for skip in SKIP_DIRS:
        if f'/{skip}/' in filepath or filepath.endswith(f'/{skip}/'):
            return False
    # Check if in allowed paths
    for allowed in ALLOWED_FILES:
        if allowed in filepath:
            return True
    return False

def fix_emit_in_file(filepath):
    """Remove 'emit ' prefix from Qt signal calls in one file."""
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    # Replace 'emit ' followed by an identifier (signal call)
    # But NOT 'emit' as a standalone word (e.g., emit = something)
    new_content = re.sub(r'\bemit\s+(?=[a-zA-Z_])', '', content)

    if new_content != content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        count = len(re.findall(r'\bemit\s+', content))
        return count
    return 0

def main():
    root = r'C:\dev\CloudCompare'
    total = 0
    for dirpath, dirs, files in os.walk(root):
        # Skip third-party dirs
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for filename in files:
            if filename.endswith('.cpp') or filename.endswith('.h'):
                filepath = os.path.join(dirpath, filename)
                if should_process(filepath):
                    n = fix_emit_in_file(filepath)
                    if n > 0:
                        print(f"  Fixed {n}x: {filepath}")
                        total += n

    print(f"\nTotal: {total} emit keywords removed")

if __name__ == '__main__':
    main()
