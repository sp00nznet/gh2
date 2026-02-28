// gh2 - Settings persistence implementation

#include "settings.h"

#include <toml++/toml.hpp>
#include <fstream>

Gh2Settings LoadSettings(const std::filesystem::path& path) {
    Gh2Settings s;
    if (!std::filesystem::exists(path)) return s;

    try {
        auto tbl = toml::parse_file(path.string());

        s.render_path = tbl["gfx"]["render_path"].value_or(s.render_path);
        s.resolution_scale = tbl["gfx"]["resolution_scale"].value_or(s.resolution_scale);
        s.fullscreen = tbl["gfx"]["fullscreen"].value_or(s.fullscreen);

        s.full_game = tbl["game"]["full_game"].value_or(s.full_game);

        s.controller_1 = tbl["controls"]["controller_1"].value_or(s.controller_1);
        s.controller_2 = tbl["controls"]["controller_2"].value_or(s.controller_2);

        s.show_fps = tbl["debug"]["show_fps"].value_or(s.show_fps);
        s.show_console = tbl["debug"]["show_console"].value_or(s.show_console);
    } catch (const toml::parse_error&) {
        // Parse error: return defaults
    }

    return s;
}

void SaveSettings(const std::filesystem::path& path, const Gh2Settings& s) {
    std::ofstream f(path);
    if (!f) return;

    f << "[gfx]\n";
    f << "render_path = " << toml::value<std::string>(s.render_path) << "\n";
    f << "resolution_scale = " << s.resolution_scale << "\n";
    f << "fullscreen = " << (s.fullscreen ? "true" : "false") << "\n";
    f << "\n";

    f << "[game]\n";
    f << "full_game = " << (s.full_game ? "true" : "false") << "\n";
    f << "\n";

    f << "[controls]\n";
    f << "controller_1 = " << toml::value<std::string>(s.controller_1) << "\n";
    f << "controller_2 = " << toml::value<std::string>(s.controller_2) << "\n";
    f << "\n";

    f << "[debug]\n";
    f << "show_fps = " << (s.show_fps ? "true" : "false") << "\n";
    f << "show_console = " << (s.show_console ? "true" : "false") << "\n";
}
