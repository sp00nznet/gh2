# Guitar Hero II - Xbox 360 Static Recompilation

## Project Overview
Static recompilation of Guitar Hero II (Xbox 360) for native Windows 11, using
the ReXGlue SDK and XenonRecomp toolchain. Based on YoshiCrystal9's re-gh2 POC.

## Architecture
- **Static recompilation**: PPC machine code -> C++ -> native x86-64
- **ReXGlue SDK**: Provides Xbox 360 kernel, graphics (D3D12), audio, input runtime
- **XenonRecomp**: PPC disassembler and C++ code generator
- **Build**: CMake 3.25+ with Clang (not MSVC), Ninja, C++23

## Key Directories
- `config/` - TOML configs for XenonRecomp and ReXGlue codegen
- `project/` - CMake build system and hand-written C++ source
- `project/src/` - Main app source (main.cpp, stubs.cpp, arkless.cpp, etc.)
- `generated/` - Auto-generated C++ from codegen (gitignored)
- `ppc/` - PPC config headers included by generated code
- `tools/` - Python extraction/analysis scripts from 360tools
- `assets/` - Extracted game files (gitignored, user-provided)
- `docs/` - Build workflow, binary analysis, speed fixes

## Build Commands
```bash
cd project
cmake --preset win-amd64
cmake --build out/build/win-amd64 --target gh2_codegen --config Release
cmake --build out/build/win-amd64 --target gh2 --config Release
```

## Game-Specific Notes
- GH2 uses ARK archive files; arkless.cpp hooks file loading for loose files
- ~1,200+ functions mapped in config TOML
- setjmp at 0x82690C90, longjmp at 0x82690A40
- memset/memcpy/memmove hooked with native implementations
- XAM UI stubs needed: XamShowGamerCardUIForXUID, XamShowMarketplaceUI, etc.
- Network stubs needed: NetDll_getsockname, NetDll_getpeername

## Conventions
- Follow sp00nznet project patterns (see 360tools templates)
- Use PPC_FUNC macro for kernel stub overrides
- Use GUEST_FUNCTION_HOOK for native function replacements
- Use GUEST_FUNCTION_STUB for no-op stubs
- Settings persisted via toml++ in gh2_settings.toml
