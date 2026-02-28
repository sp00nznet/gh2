// Guitar Hero II - ARKless file loading
// Hooks the game's file system to load loose files from disk
// instead of from the packed ARK archive.
//
// Based on ihatecompvir's contribution to re-gh2.

#include <rex/runtime/guest/context.h>
#include <rex/runtime/guest/types.h>
#include <rex/logging.h>
#include <filesystem>
#include <system_error>
#include <string_view>

// Sanitize paths containing ".." (used in ARK archive paths)
static std::filesystem::path SanitizePath(const char* cc) {
    std::filesystem::path result;
    for (const auto& part : std::filesystem::path(cc)) {
        if (part == "..") {
            result /= "(..)";
        } else {
            result /= part;
        }
    }
    return result;
}

// Hook for the game's NewFile function.
// When a file exists on disk as a loose file, sets the bypass flag
// so the game loads from the filesystem instead of the ARK archive.
//
// r3 = pointer to filename string (guest address)
// r4 = file flags register
void NewFileHook(PPCRegister& r3, PPCRegister& r4) {
    uint32_t cc_addr = r3.u32;
    if (!cc_addr || !rex::runtime::guest::g_memory_base) return;

    const char* cc = reinterpret_cast<const char*>(
        rex::runtime::guest::g_memory_base + cc_addr);
    if (!cc || !*cc) return;

    std::error_code ec;
    std::filesystem::path sanitized = SanitizePath(cc);

    bool exists = std::filesystem::exists(sanitized, ec);

    // Also check under assets/ prefix
    if (!exists && sanitized.begin() != sanitized.end() &&
        *sanitized.begin() != "assets") {
        exists = std::filesystem::exists(
            std::filesystem::path("assets") / sanitized, ec);
    }

    if (exists) {
        REXLOG_INFO("NewFile: {} [flags={:#x}]", cc, r4.u32);
        // Set the ARK bypass flag (bit 16)
        r4.u64 = r4.u32 | 0x10000;
    } else {
        REXLOG_INFO("NewFile: {} (ARK) [flags={:#x}]", cc, r4.u32);
    }
}
