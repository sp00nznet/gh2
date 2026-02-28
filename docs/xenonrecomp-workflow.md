# Guitar Hero II - XenonRecomp Workflow

Step-by-step guide from Xbox 360 disc image to native PC executable.

## Prerequisites

- Python 3.8+
- CMake 3.25+
- Clang 18+ (not MSVC for compilation)
- Ninja build system
- XenonRecomp (with patches from `tools/patches/`)
- ReXGlue SDK v0.2.1+

## Step 1: Extract Game Files

If starting from an Xbox 360 disc image (.iso):

```bash
# Extract the ISO contents (use any Xbox 360 ISO tool)
# Place default.xex and all game data files into assets/
```

If starting from an XBLA/STFS package:

```bash
python tools/extract_stfs.py path/to/package.bin assets/
```

## Step 2: Extract PE from XEX

```bash
python tools/extract_pe.py assets/default.xex assets/default.pe
```

This decrypts and decompresses the Xbox 360 executable into a raw PE binary.

## Step 3: Analyze the Binary

```bash
# Find ABI helper addresses
python tools/find_abi_addrs.py assets/default.pe

# Parse XEX imports (identify kernel stubs needed)
python tools/parse_xex_imports.py assets/default.xex

# Extract switch tables
python tools/extract_switch_tables.py assets/default.pe

# Find vtable functions missed by static analysis
python tools/find_missing_vtable_funcs.py assets/default.pe
```

Update `config/gh2_xenonrecomp.toml` with the addresses found.

## Step 4: Build XenonRecomp (with patches)

```bash
git clone https://github.com/hedge-dev/XenonRecomp.git
cd XenonRecomp
git apply ../tools/patches/xenonrecomp-altivec-vmx.patch
git apply ../tools/patches/xenonrecomp-missing-instructions.patch
cmake -B build -G Ninja
cmake --build build
```

## Step 5: Run XenonRecomp

```bash
XenonRecomp config/gh2_xenonrecomp.toml
```

This generates C++ source files in `ppc/`.

## Step 6: Run ReXGlue Codegen

```bash
cd project
cmake --preset win-amd64
cmake --build out/build/win-amd64 --target gh2_codegen --config Release
```

This translates the PPC function mappings into compilable C++ in `generated/`.

## Step 7: Build the Executable

```bash
cmake --build out/build/win-amd64 --target gh2 --config Release
```

## Step 8: Run

```bash
./out/build/win-amd64/Release/gh2.exe ../assets
```

## Iterating

When the game crashes or has issues:
1. Check `gh2_crash.log` and `gh2_stderr.log`
2. Identify missing functions or bad switch table boundaries
3. Update the TOML configs
4. Re-run codegen and rebuild
5. Add stubs in `project/src/stubs.cpp` for missing kernel APIs
