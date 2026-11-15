"""Run actual shared protocol and button logic without hardware."""
from pathlib import Path
import subprocess
import tempfile
root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    binary = Path(tmp) / 'demo'
    subprocess.run(['c++', '-std=c++11', '-include', 'initializer_list',
                    '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
                    '-I', str(root / 'include'), str(root / 'tests/test_demo.cpp'),
                    '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
