// gh2 - Menu bar and config dialogs implementation

#include "menu.h"
#include "settings.h"
#include "unlocks.h"

#include <rex/ui/menu_item.h>
#include <rex/ui/window.h>
#include <rex/ui/windowed_app.h>
#include <rex/ui/imgui_dialog.h>
#include <rex/ui/imgui_drawer.h>
#include <rex/runtime.h>

#include <imgui.h>

#include <fstream>
#include <vector>
#include <string>

using namespace rex::ui;

static void RightAlignedButtons(float button_width = 80.0f) {
    float avail = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - button_width * 2 - 8);
}

// ============================================================================
// Graphics dialog
// ============================================================================

class GraphicsDialog : public ImGuiDialog {
public:
    GraphicsDialog(ImGuiDrawer* drawer, WindowedAppContext* app_context,
                   Window* window, Gh2Settings* settings,
                   const std::filesystem::path& settings_path,
                   std::function<void()> on_done)
        : ImGuiDialog(drawer), app_context_(app_context), window_(window),
          settings_(settings), settings_path_(settings_path),
          on_done_(std::move(on_done)) {
        render_path_idx_ = (settings->render_path == "rtv") ? 1 : 0;
        resolution_scale_idx_ = (settings->resolution_scale >= 2) ? 1 : 0;
        fullscreen_ = settings->fullscreen;
    }

protected:
    void OnDraw(ImGuiIO& io) override {
        (void)io;
        ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Graphics##gh2", nullptr,
                         ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize)) {
            ImGui::Text("Render Path:");
            ImGui::SameLine(160);
            ImGui::SetNextItemWidth(180);
            const char* path_items[] = {"ROV (Recommended)", "RTV"};
            ImGui::Combo("##render_path", &render_path_idx_, path_items, 2);

            ImGui::Text("Resolution Scale:");
            ImGui::SameLine(160);
            ImGui::SetNextItemWidth(180);
            const char* scale_items[] = {"1x", "2x"};
            ImGui::Combo("##resolution_scale", &resolution_scale_idx_, scale_items, 2);

            ImGui::Checkbox("Fullscreen (F11)", &fullscreen_);

            ImGui::Spacing();
            ImGui::TextDisabled("Render path and resolution scale require restart.");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            RightAlignedButtons();
            if (ImGui::Button("OK", ImVec2(80, 0))) {
                settings_->render_path = (render_path_idx_ == 0) ? "rov" : "rtv";
                settings_->resolution_scale = (resolution_scale_idx_ == 0) ? 1 : 2;
                bool fs_changed = (settings_->fullscreen != fullscreen_);
                settings_->fullscreen = fullscreen_;
                SaveSettings(settings_path_, *settings_);
                if (fs_changed && window_ && app_context_) {
                    bool fs = fullscreen_;
                    auto* w = window_;
                    app_context_->CallInUIThreadDeferred([w, fs]() {
                        w->SetFullscreen(fs);
                    });
                }
                Close();
                if (on_done_) on_done_();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(80, 0))) {
                Close();
                if (on_done_) on_done_();
            }
        }
        ImGui::End();
    }

private:
    WindowedAppContext* app_context_;
    Window* window_;
    Gh2Settings* settings_;
    std::filesystem::path settings_path_;
    std::function<void()> on_done_;
    int render_path_idx_ = 0;
    int resolution_scale_idx_ = 0;
    bool fullscreen_ = false;
};

// ============================================================================
// Debug dialog
// ============================================================================

class DebugDialog : public ImGuiDialog {
public:
    DebugDialog(ImGuiDrawer* drawer, Gh2Settings* settings,
                const std::filesystem::path& settings_path,
                std::function<void()> on_done)
        : ImGuiDialog(drawer), settings_(settings),
          settings_path_(settings_path), on_done_(std::move(on_done)) {
        show_fps_ = settings->show_fps;
        show_console_ = settings->show_console;
        unlock_all_songs_ = settings->unlock_all_songs;
    }

protected:
    void OnDraw(ImGuiIO& io) override {
        (void)io;
        ImGui::SetNextWindowSize(ImVec2(370, 220), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Debug Options##gh2", nullptr,
                         ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize)) {
            ImGui::Checkbox("Show FPS overlay", &show_fps_);
            ImGui::Checkbox("Show debug console", &show_console_);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Unlock All Songs (bypass career)", &unlock_all_songs_);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            RightAlignedButtons();
            if (ImGui::Button("OK", ImVec2(80, 0))) {
                settings_->show_fps = show_fps_;
                settings_->show_console = show_console_;
                settings_->unlock_all_songs = unlock_all_songs_;
                g_unlock_all_songs = unlock_all_songs_;
                SaveSettings(settings_path_, *settings_);
                Close();
                if (on_done_) on_done_();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(80, 0))) {
                Close();
                if (on_done_) on_done_();
            }
        }
        ImGui::End();
    }

private:
    Gh2Settings* settings_;
    std::filesystem::path settings_path_;
    std::function<void()> on_done_;
    bool show_fps_ = true;
    bool show_console_ = false;
    bool unlock_all_songs_ = true;
};

// ============================================================================
// MenuSystem implementation
// ============================================================================

struct MenuSystem::Impl {
    ImGuiDrawer* imgui_drawer;
    Window* window;
    WindowedAppContext* app_context;
    rex::Runtime* runtime;
    Gh2Settings* settings;
    std::filesystem::path settings_path;
    std::function<void()> on_settings_changed;

    GraphicsDialog* gfx_dialog = nullptr;
    DebugDialog* debug_dialog = nullptr;

    template <typename T>
    std::function<void()> MakeOnDone(T*& tracking_ptr, bool notify = true) {
        return [this, &tracking_ptr, notify]() {
            app_context->CallInUIThreadDeferred([this, &tracking_ptr, notify]() {
                tracking_ptr = nullptr;
                if (notify && on_settings_changed) on_settings_changed();
            });
        };
    }

    void ShowGraphicsDialog() {
        if (gfx_dialog) return;
        gfx_dialog = new GraphicsDialog(imgui_drawer, app_context, window,
                                        settings, settings_path,
                                        MakeOnDone(gfx_dialog));
    }

    void ShowDebugDialog() {
        if (debug_dialog) return;
        debug_dialog = new DebugDialog(imgui_drawer, settings, settings_path,
                                       MakeOnDone(debug_dialog));
    }

    void ShowAbout() {
        ImGuiDialog::ShowMessageBox(
            imgui_drawer,
            "About Guitar Hero II",
            "Guitar Hero II - Static Recompilation\n\n"
            "Built with ReXGlue SDK\n"
            "https://github.com/sp00nznet/gh2");
    }
};

MenuSystem::MenuSystem(ImGuiDrawer* imgui_drawer, Window* window,
                       WindowedAppContext* app_context,
                       rex::Runtime* runtime,
                       Gh2Settings* settings,
                       const std::filesystem::path& settings_path,
                       std::function<void()> on_settings_changed)
    : impl_(std::make_unique<Impl>()) {
    impl_->imgui_drawer = imgui_drawer;
    impl_->window = window;
    impl_->app_context = app_context;
    impl_->runtime = runtime;
    impl_->settings = settings;
    impl_->settings_path = settings_path;
    impl_->on_settings_changed = std::move(on_settings_changed);
}

MenuSystem::~MenuSystem() = default;

std::unique_ptr<MenuItem> MenuSystem::BuildMenuBar() {
    auto* ctx = impl_.get();

    auto root = MenuItem::Create(MenuItem::Type::kNormal);

    // --- File menu ---
    auto file_menu = MenuItem::Create(MenuItem::Type::kPopup, "File");
    file_menu->AddChild(MenuItem::Create(MenuItem::Type::kSeparator));
    file_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, "Exit",
        [ctx]() { ctx->app_context->QuitFromUIThread(); }));
    root->AddChild(std::move(file_menu));

    // --- Config menu ---
    auto config_menu = MenuItem::Create(MenuItem::Type::kPopup, "Config");
    config_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, "Graphics...",
        [ctx]() { ctx->ShowGraphicsDialog(); }));
    config_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, "Debug...",
        [ctx]() { ctx->ShowDebugDialog(); }));
    root->AddChild(std::move(config_menu));

    // --- Help menu ---
    auto help_menu = MenuItem::Create(MenuItem::Type::kPopup, "Help");
    help_menu->AddChild(MenuItem::Create(
        MenuItem::Type::kString, "About...",
        [ctx]() { ctx->ShowAbout(); }));
    root->AddChild(std::move(help_menu));

    return root;
}
