# Guitar Hero II Recomp - Development Progress

## Phase 1: Project Foundation (DONE)

- [x] Repository structure (sp00nznet conventions)
- [x] Build system (CMake + Clang + Ninja)
- [x] Source code scaffolding
  - [x] main.cpp - Windowed app with VEH crash handlers
  - [x] stubs.cpp - XAM/kernel stub overrides
  - [x] arkless.cpp - Loose file loading hook
  - [x] settings.cpp/h - TOML settings persistence
  - [x] menu.cpp/h - ImGui config dialogs
  - [x] keyboard_driver.cpp/h - Keyboard input mapping
  - [x] test_boot.cpp - Console crash isolation
- [x] 360tools extraction/analysis scripts ported
- [x] XenonRecomp patches (Altivec/VMX + missing instructions)
- [x] PPC config header with safe indirect call macro
- [x] Documentation (workflow, binary analysis, speed fixes)

## Phase 2: Binary Analysis (IN PROGRESS)

- [x] Disc image structure identified (XDVDFS at 0x0FDA0000)
- [x] XEX2 signatures located (26 found)
- [x] Function mappings imported from re-gh2 (~1,200+ functions)
- [x] setjmp/longjmp addresses known
- [x] memset/memcpy/memmove hooks configured
- [ ] Extract game files from ISO (needs extract-xiso)
- [ ] Extract PE from XEX
- [ ] Run find_abi_addrs.py for ABI helpers
- [ ] Run parse_xex_imports.py for full import list
- [ ] Run extract_switch_tables.py
- [ ] Run find_missing_vtable_funcs.py

## Phase 3: First Boot (TODO)

- [ ] XenonRecomp pass (with patches)
- [ ] ReXGlue codegen
- [ ] Resolve compilation errors
- [ ] First successful build
- [ ] Boot to black screen
- [ ] Boot to Harmonix/RedOctane logos
- [ ] Boot to main menu

## Phase 4: Gameplay (TODO)

- [ ] Menu navigation
- [ ] Song selection
- [ ] Gameplay renders
- [ ] Note highway visible
- [ ] Input works (keyboard)
- [ ] Input works (controller)
- [ ] Audio plays
- [ ] Scoring works
- [ ] Star Power works

## Phase 5: Polish (TODO)

- [ ] Guitar controller support (USB/wireless)
- [ ] Whammy bar input
- [ ] Tilt sensor (Star Power activation)
- [ ] Practice mode
- [ ] Multiplayer (local)
- [ ] Performance optimization
- [ ] All songs playable
- [ ] Stable (no crashes during normal play)
