// gh2 - Debug unlock controls
// When enabled, bypasses career mode progression locks so all songs are accessible.

#pragma once

// Global flag: when true, is_unlocked() always returns 1
extern bool g_unlock_all_songs;

// Patch PPCFuncMappings to install our unlock hooks.
// Call this after runtime Setup but before LaunchModule.
void InstallUnlockHooks();
