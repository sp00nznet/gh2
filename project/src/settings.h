// gh2 - Settings persistence
// Loads/saves user configuration from gh2_settings.toml

#pragma once

#include <string>
#include <filesystem>

struct Gh2Settings {
    // [gfx]
    std::string render_path = "rov";  // "rov" or "rtv"
    int resolution_scale = 1;         // 1 or 2
    bool fullscreen = false;

    // [game]
    bool full_game = true;  // bypass trial/license check

    // [controls]
    std::string controller_1 = "auto";
    std::string controller_2 = "none";

    // [debug]
    bool show_fps = true;
    bool show_console = false;
    bool unlock_all_songs = true;
};

// Load settings from TOML file. Returns defaults if file doesn't exist.
Gh2Settings LoadSettings(const std::filesystem::path& path);

// Save settings to TOML file.
void SaveSettings(const std::filesystem::path& path, const Gh2Settings& settings);
