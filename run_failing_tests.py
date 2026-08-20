import subprocess, os

dll_dirs = [
    'C:/dev/CloudCompare/build/libs/CCPluginAPI',
    'C:/dev/CloudCompare/build/libs/qCC_db',
    'C:/dev/CloudCompare/build/libs/qCC_db/extern/CCCoreLib',
    'C:/dev/CloudCompare/build/libs/CCFbo',
    'C:/dev/CloudCompare/build/libs/qCC_glWindow',
    'C:/dev/CloudCompare/build/libs/qCC_io',
    'C:/dev/CloudCompare/build/libs/qCC_io/extern/shapelib',
    'C:/dev/CloudCompare/build/libs/CCAppCommon/devices/3dConnexion/extern/hidapi/src/windows',
    'C:/dev/tools/Qt/6.8.3/msvc2022_64/bin',
]
env = dict(os.environ)
env['PATH'] = ';'.join(dll_dirs) + ';' + env.get('PATH','')

tests = [
    ('TestGlobalShift',      'C:/dev/CloudCompare/build/libs/qCC_io/test/TestGlobalShift.exe'),
    ('TestOctree',          'C:/dev/CloudCompare/build/qCC/test/TestOctree.exe'),
    ('TestDistributions',   'C:/dev/CloudCompare/build/qCC/test/TestDistributions.exe'),
    ('TestStatisticalTesting','C:/dev/CloudCompare/build/qCC/test/TestStatisticalTesting.exe'),
    ('TestPlyFilter',       'C:/dev/CloudCompare/build/libs/qCC_io/test/TestPlyFilter.exe'),
    ('TestDxfFilter',       'C:/dev/CloudCompare/build/libs/qCC_io/test/TestDxfFilter.exe'),
]

for name, exe in tests:
    print('\n' + '='*60)
    print(f'=== {name} ===')
    # Use -txt -o to write to file, then read file
    outfile = f'C:/temp/{name}_out.txt'
    os.makedirs('C:/temp', exist_ok=True)
    result = subprocess.run(
        [exe, '-txt', '-o', outfile],
        capture_output=True, text=True, env=env, timeout=30
    )
    try:
        with open(outfile, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()
        print(content[-4000:] if len(content) > 4000 else content)
    except Exception as e:
        print(f'Could not read output file: {e}')
    print(f'[exit {result.returncode}]')
