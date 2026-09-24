#!/usr/bin/env python3
"""Run after make check, under an X display (for actual FLTK creation)."""
import itertools
import json
import os
import shlex
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

build = Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
source = Path(__file__).resolve().parent.parent
host = build / 'plugin-class-host'
modules = sorted((build / 'Plugins').glob('*/*/.libs/*Plugin_*.so'))
assert modules, 'Build the plugins first'
with tempfile.TemporaryDirectory(prefix='ssm-class-tests-') as tmp:
    root = Path(tmp)
    paths = []
    for module in modules:
        kind, plugin = module.parts[-4:-2]
        relative = Path(kind.lower()) / plugin / module.name
        dest = root / relative
        dest.parent.mkdir(parents=True)
        shutil.copyfile(module, dest)
        shutil.copyfile(source / 'Plugins' / kind / plugin / 'info.json', dest.parent / 'info.json')
        paths.append(str(relative))

    def run(order, dsp, gui, diagnostic=None):
        listing = root / 'modules.txt'
        listing.write_text('\n'.join(order) + '\n')
        result = subprocess.run([str(host), str(root), str(listing), str(dsp), str(gui)],
                                text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode or (diagnostic and diagnostic not in result.stderr):
            raise AssertionError(result.stdout + result.stderr)

    dsp = sum('_DSP.so' in p for p in paths)
    gui = sum('_GUI.so' in p for p in paths)
    run(list(reversed(paths)), dsp, gui)
    print(f'All manifests: {dsp} DSP, {gui} GUI')
    selected = [p for p in paths if Path(p).parent.name in ('AmpPlugin', 'WaveShaperPlugin')]
    assert len(selected) == 4
    for mode in ('manifest', 'mixed', 'bare'):
        for p in selected:
            manifest = root / Path(p).parent / 'info.json'
            if mode == 'bare' or (mode == 'mixed' and '_GUI.so' in p):
                manifest.unlink(missing_ok=True)
        for order in itertools.permutations(selected):
            run(order, 2, 2)
    print('72 Amp/WaveShaper discovery permutations passed')
    for manifest in root.glob('*/*/info.json'):
        manifest.unlink()
    run(list(reversed(paths)), dsp, gui)
    print(f'Bare binaries: {dsp} DSP, {gui} GUI')

    amp_dsp = next(p for p in selected if p.endswith('AmpPlugin_DSP.so'))
    amp_gui = next(p for p in selected if p.endswith('AmpPlugin_GUI.so'))
    manifest = root / Path(amp_gui).parent / 'info.json'
    original = json.loads((source / 'Plugins/GUI/AmpPlugin/info.json').read_text())
    manifest.write_text(json.dumps(original))
    run([amp_gui, amp_dsp], 1, 0, 'Missing or cyclic manifest dependency')
    # Described GUIs cannot borrow dependencies from the later bare phase.
    original['host']['abi'] = 'wrong-abi'
    manifest.write_text(json.dumps(original))
    run([amp_gui, amp_dsp], 1, 0, 'different SSM version or host ABI')
    original['host']['abi'] = 'ssm-fltk-channel-class-1'
    original['dependencies'] = [{'type': 'invalid', 'id': 9}]
    manifest.write_text(json.dumps(original))
    run([amp_gui, amp_dsp], 1, 0, 'Invalid plugin manifest')
    original['dependencies'] = [{'type': 'gui', 'id': 9}]
    manifest.write_text(json.dumps(original))
    run([amp_gui, amp_dsp], 1, 0, 'Invalid plugin manifest')
    original['dependencies'] = [{'type': 'dsp', 'id': 9}]
    manifest.write_text(json.dumps(original))
    dsp_manifest = root / Path(amp_dsp).parent / 'info.json'
    dsp_info = json.loads((source / 'Plugins/DSP/AmpPlugin/info.json').read_text())
    dsp_info['dependencies'] = [{'type': 'gui', 'id': 9}]
    dsp_manifest.write_text(json.dumps(dsp_info))
    run([amp_gui, amp_dsp], 0, 0, 'Missing or cyclic manifest dependency')
    print('Missing, cyclic, self, malformed dependency and manifest ABI rejections passed')

    rejected = root / 'rejected.so'
    subprocess.run(shlex.split(os.environ.get('CXX', 'c++')) +
                   ['-std=c++03', '-shared', '-fPIC', '-I' + str(source),
                    str(source / 'tests/rejected-plugin.cpp'), '-o', str(rejected)], check=True)
    run(['rejected.so'], 0, 0, 'Missing or incompatible plugin ABI/initializer')
    print('Binary ABI rejected before initialization')
