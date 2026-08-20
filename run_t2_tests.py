import subprocess, os, re

dll_dirs = [
    'C:/dev/CloudCompare/build/libs/CCPluginAPI',
    'C:/dev/CloudCompare/build/libs/qCC_db',
    'C:/dev/CloudCompare/build/libs/qCC_db/extern/CCCoreLib',
    'C:/dev/CloudCompare/build/libs/CCFbo',
    'C:/dev/CloudCompare/build/libs/qCC_glWindow',
    'C:/dev/CloudCompare/build/libs/qCC_io',
    'C:/dev/CloudCompare/build/libs/qCC_io/extern/shapelib',
    'C:/dev/tools/Qt/6.8.3/msvc2022_64/bin',
]
env = os.environ.copy()
env['PATH'] = ';'.join(dll_dirs) + ';' + env.get('PATH','')

tests = [
    ('TestOctree', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestDistributions', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestPlyFilter', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestDxfFilter', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestGlobalShift', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestStatisticalTesting', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestAsciiFilter', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestGLUtils', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestSensor', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestMesh', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestConsole', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestImageFilter', 'C:/dev/CloudCompare/build/libs/qCC_io/test'),
    ('TestAnimation', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestVoxFall', 'C:/dev/CloudCompare/build/qCC/test'),
    ('TestPCV', 'C:/dev/CloudCompare/build/qCC/test'),
]

total_pass = 0; total_fail = 0; total_skip = 0
for name, path in tests:
    exe = os.path.join(path, name + '.exe')
    out = os.path.join('C:/dev/CloudCompare', 'test_result_' + name + '.txt')
    if not os.path.exists(exe):
        print(name + ': NOT FOUND')
        continue
    try:
        r = subprocess.run([exe, '-txt', '-o', out], env=env, timeout=20, capture_output=True)
    except subprocess.TimeoutExpired:
        print(name + ': TIMEOUT'); continue
    with open(out) as f:
        txt = f.read()
    p = re.search(r'(\d+) passed', txt); fp = int(p.group(1)) if p else 0
    f2 = re.search(r'(\d+) failed', txt); ff = int(f2.group(1)) if f2 else 0
    s = re.search(r'(\d+) skipped', txt); fs = int(s.group(1)) if s else 0
    total_pass += fp; total_fail += ff; total_skip += fs
    status = 'PASS' if ff == 0 else 'FAIL'
    print('%s: %s %dP/%dF/%dS' % (name, status, fp, ff, fs))

print('\nTotal: %d passed, %d failed, %d skipped' % (total_pass, total_fail, total_skip))
