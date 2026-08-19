import subprocess, os, sys

# Test compile ccOctree.cpp to isolate the error
vcvars = r'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\vcvars64.bat'
qt = r'C:\dev\tools\Qt\6.8.3\msvc2022_64'
sdkinc = r'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um'
ucrt = r'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt'
pch = r'C:\dev\CloudCompare\build\libs\qCC_db\CMakeFiles\QCC_DB_LIB.dir\src\qCC_db_PCH.h.pch'
src_dir = r'C:\dev\CloudCompare\build\libs\qCC_db\CMakeFiles\QCC_DB_LIB.dir\src'
ccdb_inc = r'C:\dev\CloudCompare\libs\qCC_db\include'
ccc_inc = r'C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include'
build_ccc = r'C:\dev\CloudCompare\build\libs\qCC_db\extern\CCCoreLib'
ccmath = r'C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include'
src_octree = r'C:\dev\CloudCompare\libs\qCC_db\src\ccOctree.cpp'
src_cone = r'C:\dev\CloudCompare\libs\qCC_db\src\ccCone.cpp'
test_obj = os.path.join(src_dir, 'test_ccOctree.obj')

cmd_base = (f'"{vcvars}" >nul && cl.exe /c /TP /nologo /W3 /permissive- /std:c++17 /Zc:__cplusplus /EHsc /MD /O2 '
            f'/I"{ccdb_inc}" '
            f'/I"{ccc_inc}" '
            f'/I"{build_ccc}" '
            f'/I"{ccmath}" '
            f'/I"{qt}\\include" '
            f'/I"{qt}\\include\\QtCore" '
            f'/I"{qt}\\include\\QtGui" '
            f'/I"{qt}\\include\\QtWidgets" '
            f'/I"{qt}\\include\\QtOpenGL" '
            f'/I"{sdkinc}" '
            f'/I"{ucrt}" '
            f'/YuqCC_db_PCH.h '
            f'/Fp"{pch}" '
            f'/Fo"{test_obj}" ')

# Test 1: ccOctree.cpp
cmd1 = cmd_base + f'"{src_octree}"'
print("=== Testing ccOctree.cpp ===")
r = subprocess.run(cmd1, shell=True, capture_output=True, text=True, timeout=60)
if r.stdout: print("STDOUT:", r.stdout[:2000])
if r.stderr: print("STDERR:", r.stderr[:2000])
print("RC:", r.returncode)

# Test 2: ccCone.cpp
test_obj2 = os.path.join(src_dir, 'test_ccCone.obj')
cmd2 = cmd_base.replace(test_obj, test_obj2) + f'"{src_cone}"'
print("\n=== Testing ccCone.cpp ===")
r2 = subprocess.run(cmd2, shell=True, capture_output=True, text=True, timeout=60)
if r2.stdout: print("STDOUT:", r2.stdout[:2000])
if r2.stderr: print("STDERR:", r2.stderr[:2000])
print("RC:", r2.returncode)
