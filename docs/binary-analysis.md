# Guitar Hero II - Binary Analysis Notes

## XEX Information

- **Title**: Guitar Hero II
- **Platform**: Xbox 360
- **Title ID**: TBD (extract with `parse_xex_imports.py`)
- **Encryption**: AES-128-CBC (standard Xbox 360 retail key)
- **Compression**: LZX or basic block (TBD)

## PE Memory Layout

- **Image Base**: 0x82000000 (standard Xbox 360)
- **Entry Point**: TBD
- **Sections**: TBD (`.text`, `.rdata`, `.data`, `.pdata`, `.idata`, `.reloc`)

## ABI Function Addresses

From re-gh2 config:
- **setjmp**: 0x82690C90
- **longjmp**: 0x82690A40

TODO: Run `find_abi_addrs.py` to get all 10 ABI helpers:
- savegprlr_14, restgprlr_14
- savefpr_14, restfpr_14
- savevmx_14, restvmx_14
- savevmx_64, restvmx_64

## Function Statistics

- **~1,200+ functions** mapped in ReXGlue config (from re-gh2)
- **Address range**: 0x82120008 to ~0x8271D630
- **C library hooks**: memset (0x8239CA70), memcpy (0x8239CB70), memmove (0x8239D4F0)

## Known Kernel/XAM Imports

Stubs needed (from re-gh2):
- XamShowGamerCardUIForXUID
- XamShowMarketplaceUI
- XamUserCreateStatsEnumerator
- NetDll_getsockname
- NetDll_getpeername

Additional stubs may be needed as boot progresses.

## Game Data Files

Guitar Hero II uses the ARK archive system (common to Harmonix games).
The `arkless.cpp` hook allows loading loose files from disk, which is
essential for modding and debugging.

Key file paths:
- `default.xex` - Main executable
- `*.ark` / `*.hdr` - ARK archive files containing game assets
- Song data, charts, audio, textures are packed in ARK archives

## Midasm Hooks

From re-gh2 config:
- **NewFileHook** at 0x82276780 (registers: r3, r4) - File system intercept
