# Plugin class loading

The public FLTK host uses the same typed `(type, id)` registry model as the
private host. Its common `Plugin` root has no Slick dependency. `SpiralPlugin`
is the DSP/device family; `PluginUIDefinition` adapts FLTK factories without
changing the widget hierarchy or ChannelHandler protocol.

`SpiralPlugin_GetHostABI` is checked before `SpiralPlugin_Initialize` is called.
The initializer registers definitions and returns their typed identity. It must
not create instances, and must tolerate retry when a parent or dependency is
missing. Native library constructors still follow the platform's `dlopen`
semantics. This ABI is `ssm-fltk-channel-class-1`; rebuild all modules together.

Discovery reads adjacent `info.json` manifests before opening modules. Declared
dependencies determine registration order. Unresolved manifest dependencies are
rejected before the bare-binary phase. Modules without manifests are attempted
in discovery order, with retries for dependencies discovered later. Invalid
manifests cannot fall back to bare loading. Manifest and class identity, name,
category, and dependency sets must agree. Errors go to the existing console
alert path. YAJL 2 is now required at configure time because manifests are part
of production loading. Binaries can still load without a manifest.

Public GUI modules currently import native DSP methods, so the loader retains
`RTLD_GLOBAL`. New GUI factories have internal linkage to avoid interposing
another module's identically named C factory. Native inheritance additionally
requires its usual shared-library linkage; registration dependencies do not
replace the platform dynamic linker.

Device port definitions describe defaults, with `input` and `immutable` flags.
Live `PluginInfo` and the existing sample vectors remain instance-owned and can
change. Amp and Mixer use `CreateDefaultPorts`; other constructors preserve their
existing setup. Mixer starts with four inputs (the first two immutable) and one
output; LADSPA starts without ports. Clients must refresh live instance metadata
after changes, rather than treating the class defaults as its current ports.

All new production code and C++ tests support C++03. Run `make check` for the
registry, parser, and existing compatibility tests. After building, run
`python3 tests/run-plugin-loading.py .` under an X display for actual FLTK factory
creation. `SSM_TEST_NO_DISPLAY=1` checks GUI registration without creating widgets;
`SSM_TEST_NO_HARDWARE=1` skips constructing MIDI and JACK devices. The runner tests
manifest, mixed, and bare discovery orders, mutable Mixer defaults, native
WaveShaper dependencies, and rejected metadata/ABI. The host is a noninstalled
test program, not part of the shipped application.

Destroy instances before unloading. Registry views are removed before their
modules, and modules close in reverse registration order. Individual unloads
refuse to remove a class that still has subclasses or dependency consumers.
