#!/usr/bin/env python3
"""
Build CloudCompare using Ninja.
Faster than cmake --build because it uses Ninja directly.
Usage: python tools/cc-build.py [--clean]
  --clean   Delete CMakeFiles directories before building (forces full rebuild)
"""
import subprocess
import sys
import os
import glob
import shutil

def main():
    build = r'C:\dev\CloudCompare\build'
    ninja = r'C:\ProgramData\chocolatey\bin\ninja.exe'

    if '--clean' in sys.argv:
        print('Cleaning CMakeFiles directories...')
        for root, dirs, files in os.walk(build):
            for d in dirs:
                if d == 'CMakeFiles':
                    shutil.rmtree(os.path.join(root, d))
        # Also delete ninja build files
        for f in glob.glob(os.path.join(build, '*.ninja')):
            os.remove(f)
        # Delete CMakeCache to force reconfigure
        cache = os.path.join(build, 'CMakeCache.txt')
        if os.path.exists(cache):
            os.remove(cache)
        print('Clean done. Run: python tools/cc-configure.py first, then this script again.')

    cmd = [ninja, '-C', build, '-j16']
    print(f'Building: {" ".join(cmd)}')
    result = subprocess.run(cmd, capture_output=True, text=True)
    print(result.stdout[-6000:])
    if result.stderr:
        print('STDERR:', result.stderr[-2000:])
    print('EXIT:', result.returncode)
    return result.returncode

if __name__ == '__main__':
    sys.exit(main())
