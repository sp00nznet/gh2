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

## Phase 2: Binary Analysis (DONE)

- [x] Disc image structure identified (XDVDFS at 0x0FDA0000)
- [x] XEX2 signatures located (26 found)
- [x] Function mappings imported from re-gh2 (~1,200+ functions)
- [x] setjmp/longjmp addresses known
- [x] memset/memcpy/memmove hooks configured
- [x] Extract game files from ISO (extract-xiso compiled and run)
- [x] Extract PE from XEX (8.9MB, PowerPC, 16 sections, valid MZ header)
- [x] Run find_abi_addrs.py for ABI helpers (8/8 found, setjmp/longjmp from re-gh2)
- [x] Run parse_xex_imports.py (2 libraries: xam.xex, xboxkrnl.exe)
- [x] Run extract_switch_tables.py (74 switch tables, lwzx pattern)
- [ ] Run find_missing_vtable_funcs.py (needs generated code, deferred to Phase 3)

## Phase 3: First Boot (DONE)

- [x] ReXGlue codegen (34,942 functions analyzed, 34,763 recompiled)
- [x] CMake configure (Clang 21.1.8, Ninja Multi-Config)
- [x] Resolve compilation errors (1 missing include)
- [x] First successful build (gh2.exe, 45.7MB)
- [x] Boot to black screen
- [x] Boot to Harmonix/RedOctane logos
- [x] Audio plays on boot
- [x] Keyboard input working
- [x] Boot to main menu

## Phase 4: Gameplay (DONE)

- [x] Menu navigation
- [x] Song selection
- [x] Gameplay renders
- [x] Note highway visible
- [x] Input works (keyboard)
- [ ] Input works (controller)
- [x] Audio plays
- [x] Scoring works
- [x] Star Power works
- [x] Debug unlock all songs (hooks is_unlocked at 0x82285C00)

## Phase 5: Polish (TODO)

- [ ] Guitar controller support (USB/wireless)
- [ ] Whammy bar input
- [ ] Tilt sensor (Star Power activation)
- [ ] Practice mode
- [ ] Multiplayer (local)
- [ ] Performance optimization
- [ ] All songs playable
- [ ] Stable (no crashes during normal play)
