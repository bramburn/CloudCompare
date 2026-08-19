# Fix CSF plugin export macros
import os

headers = [
    ('C:/dev/CloudCompare/plugins/core/Standard/qCSF/include/Particle.h', 'class Particle', 'QCSF_LIB_API'),
    ('C:/dev/CloudCompare/plugins/core/Standard/qCSF/include/Cloth.h', 'class Cloth', 'QCSF_LIB_API'),
    ('C:/dev/CloudCompare/plugins/core/Standard/qCSF/include/CSF.h', 'class CSF', 'QCSF_LIB_API'),
    ('C:/dev/CloudCompare/plugins/core/Standard/qCSF/include/Cloud2CloudDist.h', 'class Cloud2CloudDist', 'QCSF_LIB_API'),
]

add_include = '#include "qCSF.h"'

for path, class_decl, macro in headers:
    with open(path, 'r', encoding='utf-8-sig') as f:
        content = f.read()

    # Add qCSF.h include if not already present
    if '#include "qCSF.h"' not in content:
        lines = content.split('\n')
        insert_idx = 0
        for i, line in enumerate(lines):
            if line.startswith('#include'):
                insert_idx = i
                break
        lines.insert(insert_idx, add_include)
        content = '\n'.join(lines)

    # Add export macro to class declaration
    target_before = class_decl
    target_after = macro + ' ' + class_decl
    if target_after not in content:
        content = content.replace(target_before, target_after, 1)
        with open(path, 'w', encoding='utf-8-sig') as f:
            f.write(content)
        print('Updated:', os.path.basename(path))
    else:
        print('Already updated:', os.path.basename(path))
