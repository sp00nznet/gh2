// gh2 - Debug unlock overrides
// Patches PPCFuncMappings at runtime to intercept career unlock checks.
// This avoids weak-symbol issues on Windows COFF by replacing function
// pointers in the mapping table directly.

#include "unlocks.h"
#include "gh2_config.h"
#include <rex/runtime/guest/context.h>
#include <cstdio>

using namespace rex::runtime::guest;

bool g_unlock_all_songs = false;

// Original function pointers (saved before patching)
static PPCFunc* s_orig_is_unlocked = nullptr;        // 0x82285C00
static PPCFunc* s_orig_num_unlocked = nullptr;        // 0x822855E8
static PPCFunc* s_orig_career_progressed = nullptr;   // 0x82285DB8

// Hook: is_unlocked(career_obj, item_symbol) -> bool
static void Hook_IsUnlocked(PPCContext& ctx, uint8_t* base) {
    if (g_unlock_all_songs) {
        ctx.r3.u64 = 1;
        return;
    }
    s_orig_is_unlocked(ctx, base);
}

// Hook: get_num_unlocked_songs(career_obj) -> count
static void Hook_NumUnlocked(PPCContext& ctx, uint8_t* base) {
    if (g_unlock_all_songs) {
        ctx.r3.u64 = 9999;
        return;
    }
    s_orig_num_unlocked(ctx, base);
}

// Hook: has_career_progressed(career_obj) -> bool
static void Hook_CareerProgressed(PPCContext& ctx, uint8_t* base) {
    if (g_unlock_all_songs) {
        ctx.r3.u64 = 1;
        return;
    }
    s_orig_career_progressed(ctx, base);
}

// Target addresses to patch
struct HookEntry {
    size_t address;
    PPCFunc** original;
    PPCFunc* hook;
};

static const HookEntry kHooks[] = {
    { 0x82285C00, &s_orig_is_unlocked,       Hook_IsUnlocked },
    { 0x822855E8, &s_orig_num_unlocked,       Hook_NumUnlocked },
    { 0x82285DB8, &s_orig_career_progressed,  Hook_CareerProgressed },
};

void InstallUnlockHooks() {
    int patched = 0;
    for (int i = 0; PPCFuncMappings[i].host != nullptr; i++) {
        for (const auto& hook : kHooks) {
            if (PPCFuncMappings[i].guest == hook.address) {
                *hook.original = PPCFuncMappings[i].host;
                PPCFuncMappings[i].host = hook.hook;
                patched++;
            }
        }
        if (patched == 3) break;
    }
}
