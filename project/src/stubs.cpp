// Guitar Hero II - Xbox 360 kernel/XAM stub overrides
// These are Xbox 360 APIs not yet implemented in the ReXGlue SDK.

#include "gh2_config.h"
#include <rex/runtime/guest/context.h>
#include <rex/runtime/guest/memory.h>
#include <rex/kernel/kernel_state.h>
#include <cstring>

using namespace rex::runtime::guest;

// Simple stub macro that returns a value
#define GH2_STUB_RETURN(name, val) \
    extern "C" PPC_FUNC(name) { (void)base; ctx.r3.u64 = (val); }

#define GH2_STUB(name) GH2_STUB_RETURN(name, 0)

// ============================================================================
// XAM UI stubs (Guitar Hero II calls these for marketplace/profile UI)
// ============================================================================
GH2_STUB(__imp__XamShowGamerCardUIForXUID)
GH2_STUB(__imp__XamShowMarketplaceUI)
GH2_STUB_RETURN(__imp__XamUserCreateStatsEnumerator, 1)  // fail = no stats

// ============================================================================
// Networking stubs (GH2 has online leaderboards we don't need)
// ============================================================================
GH2_STUB(__imp__NetDll_getsockname)
GH2_STUB(__imp__NetDll_getpeername)

// ============================================================================
// Content license bypass
// ============================================================================
// Override the license check to return full license mask.
// This allows the game to run without Xbox Live validation.
// The generated code declares this as PPC_WEAK_FUNC, so our
// strong definition takes precedence.
//
// TODO: Identify the actual license check function address in GH2.
//       For now, this is a placeholder pattern from the template.
//       Will need to find the specific sub_XXXXXXXX that wraps
//       XamContentGetLicenseMask in the GH2 binary.

// ============================================================================
// XAM User stubs (single-player focus for now)
// ============================================================================

// XamUserGetSigninState(user_index) -> signin_state
// Return 1 (signed in locally) for user 0 only.
extern "C" PPC_FUNC(__imp__XamUserGetSigninState) {
    uint32_t user_index = ctx.r3.u32;
    ctx.r3.u64 = (user_index == 0) ? 1 : 0;
}

// XamUserGetSigninInfo(user_index, flags, info_ptr) -> HRESULT
extern "C" PPC_FUNC(__imp__XamUserGetSigninInfo) {
    uint32_t user_index = ctx.r3.u32;
    uint32_t info_ptr = ctx.r5.u32;

    if (!info_ptr) {
        ctx.r3.u64 = 0x80070057;  // E_INVALIDARG
        return;
    }

    std::memset(base + info_ptr, 0, 40);

    if (user_index != 0) {
        ctx.r3.u64 = 0x80070490;  // E_NO_SUCH_USER
        return;
    }

    // XUID
    PPC_STORE_U64(info_ptr + 0, 0xB13EBABEBABE0001ULL);
    // signin_state = signed in locally
    PPC_STORE_U32(info_ptr + 12, 1);
    // Gamertag
    const char* name = "GuitarHero";
    std::memcpy(base + info_ptr + 24, name, std::strlen(name) + 1);

    ctx.r3.u64 = 0;  // SUCCESS
}

// XamUserGetXUID(user_index, type_mask, xuid_ptr) -> HRESULT
extern "C" PPC_FUNC(__imp__XamUserGetXUID) {
    uint32_t user_index = ctx.r3.u32;
    uint32_t xuid_ptr = ctx.r5.u32;

    if (!xuid_ptr) {
        ctx.r3.u64 = 0x80070057;
        return;
    }

    if (user_index != 0) {
        PPC_STORE_U64(xuid_ptr, 0);
        ctx.r3.u64 = 0x80070490;
        return;
    }

    PPC_STORE_U64(xuid_ptr, 0xB13EBABEBABE0001ULL);
    ctx.r3.u64 = 0;
}

// XamUserGetName(user_index, buffer, buffer_len) -> result
extern "C" PPC_FUNC(__imp__XamUserGetName) {
    uint32_t user_index = ctx.r3.u32;
    uint32_t buffer_ptr = ctx.r4.u32;
    uint32_t buffer_len = ctx.r5.u32;

    if (user_index != 0) {
        ctx.r3.u64 = 0x80070490;
        return;
    }

    const char* name = "GuitarHero";
    uint32_t copy_len = std::min(buffer_len, (uint32_t)(std::strlen(name) + 1));
    if (buffer_ptr && copy_len > 0) {
        std::memcpy(base + buffer_ptr, name, copy_len);
        *(base + buffer_ptr + copy_len - 1) = 0;
    }

    ctx.r3.u64 = 0;
}

// XamUserCheckPrivilege - grant all privileges
extern "C" PPC_FUNC(__imp__XamUserCheckPrivilege) {
    uint32_t out_ptr = ctx.r5.u32;
    if (out_ptr) PPC_STORE_U32(out_ptr, 0);
    ctx.r3.u64 = 0;
}

// XamUserGetMembershipTier - return Gold
extern "C" PPC_FUNC(__imp__XamUserGetMembershipTier) {
    (void)base;
    ctx.r3.u64 = 6;  // Gold
}

// XamShowSigninUI - broadcast sign-in for user 0
extern "C" PPC_FUNC(__imp__XamShowSigninUI) {
    (void)base;
    auto* ks = rex::kernel::kernel_state();
    if (ks) {
        ks->BroadcastNotification(0x0000000A, 1);  // XN_SYS_SIGNINCHANGED
        ks->BroadcastNotification(0x00000009, 0);   // XN_SYS_UI off
    }
    ctx.r3.u64 = 0;
}

// XamUserIsOnlineEnabled - yes
extern "C" PPC_FUNC(__imp__XamUserIsOnlineEnabled) {
    (void)base;
    ctx.r3.u64 = 1;
}
