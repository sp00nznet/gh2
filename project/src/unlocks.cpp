// gh2 - Debug unlock overrides
// Hooks the game's is_unlocked DTB callback (gh2_sub_82285C00) to bypass
// career mode progression locks when g_unlock_all_songs is set.

#include "unlocks.h"
#include "gh2_config.h"
#include <rex/runtime/guest/context.h>

using namespace rex::runtime::guest;

bool g_unlock_all_songs = false;

// Original implementation (defined as __imp__ in generated code, weak-aliased
// to gh2_sub_82285C00). Our strong definition here overrides the weak alias
// while keeping the original reachable via __imp__.
extern "C" PPC_FUNC(__imp__gh2_sub_82285C00);

// Override: is_unlocked(career_obj, item_symbol) -> bool
// When unlock_all_songs is enabled, skip the career progression check
// and return 1 (unlocked) for everything.
extern "C" PPC_FUNC(gh2_sub_82285C00) {
    if (g_unlock_all_songs) {
        ctx.r3.u64 = 1;
        return;
    }
    // Fall through to original recompiled implementation
    __imp__gh2_sub_82285C00(ctx, base);
}
