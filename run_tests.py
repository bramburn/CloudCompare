import subprocess, os, sys

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

tests = [
    'TestPCV',
    'TestScalarField',
    'TestGLMatrix',
    'TestAnimation',
    'TestVoxFall',
    'TestInnerRect',
    'TestLibAlgorithms',
    'TestGlobalShift',
    'TestAsciiFilter',
]

for name in tests:
    exe = f'C:/dev/CloudCompare/build/qCC/test/{name}.exe'
    out_file = f'C:/dev/CloudCompare/test_{name}_out.txt'
    print(f'\n{"="*60}')
    print(f'Running {name}...')
    try:
        result = subprocess.run(
            [exe, '-txt', '-o', out_file],
            capture_output=True, text=True, env=env, timeout=60
        )
        if os.path.exists(out_file):
            with open(out_file, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
            print(content[-3000:] if len(content) > 3000 else content)
            try:
                os.remove(out_file)
            except:
                pass
        else:
            print(f'NO OUTPUT FILE: {exe}')
            print('STDOUT:', result.stdout[-1000:] if result.stdout else '(empty)')
            print('STDERR:', result.stderr[-500:] if result.stderr else '(empty)')
    except subprocess.TimeoutExpired:
        print(f'TIMEOUT after 60s')
    except Exception as e:
        print(f'ERROR: {e}')
