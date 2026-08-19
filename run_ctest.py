import subprocess, os

dll_dirs = [
    'C:/dev/CloudCompare/build/libs/CCPluginAPI',
    'C:/dev/CloudCompare/build/libs/qCC_db',
    'C:/dev/CloudCompare/build/libs/qCC_db/extern/CCCoreLib',
    'C:/dev/CloudCompare/build/libs/CCFbo',
    'C:/dev/CloudCompare/build/libs/qCC_glWindow',
    'C:/dev/CloudCompare/build/libs/qCC_io',
    'C:/dev/CloudCompare/build/libs/qCC_io/extern/shapelib',
    'C:/dev/tools/Qt/6.8.3/msvc2022_64/bin'
]
env = os.environ.copy()
env['PATH'] = ';'.join(dll_dirs) + ';' + env.get('PATH','')

# Run ctest from the build directory
result = subprocess.run(
    ['C:/dev/tools/cmake-4.3.0/bin/ctest.exe', '--test-dir', 'C:/dev/CloudCompare/build', '--output-on-failure', '-j', '4'],
    capture_output=True, text=True, env=env, timeout=120
)
print(result.stdout)
if result.stderr:
    print('STDERR:', result.stderr)
print(f'\nReturn code: {result.returncode}')
