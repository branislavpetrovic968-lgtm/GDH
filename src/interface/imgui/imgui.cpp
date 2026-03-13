#include <Geode/Geode.hpp>
#ifdef GEODE_IS_WINDOWS
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "layout.hpp"
#include "popup.hpp"

#include "widgetH.hpp"
#include "theme.hpp"
#include "font.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../core/utils.hpp"
#include "../../core/labels.hpp"

static bool g_show = false;
static bool g_isAnimating = false;
static bool g_isFadingIn = false;
static float g_animTime = 0.0f;

static bool g_inited = false;
static std::string g_search_text = "";

static std::vector<std::vector<std::string>> g_layout = {
    {"Core", "Bypass"},
    {"Cosmetic"},
    {"Level", "Framerate", "GDH Settings"},
    {"Creator", "Labels"},
    {"Shortcuts"}
};

static std::vector<GDH::Layout::WindowInfo> g_fixedWindowSizes = {
    {"Labels", 240.f, 350.f},
    {"Shortcuts", 180.f, 0.f}
};

void onOpen() {
    GDH::Utils::updateCursorState(g_show);
}

void onClose() {
    GDH::Utils::updateCursorState(g_show);
    Config::get().save(fileDataPath);
    GDH::Labels::save();
    g_search_text = "";
}

void animateAlpha()
{
    if (!g_isAnimating)
        return;

    ImGuiStyle& style = ImGui::GetStyle();
    float deltaTime   = ImGui::GetIO().DeltaTime;

    float duration = 100 / 1000.0f;
    g_animTime += deltaTime;

    float t = g_animTime / duration;
    if (t >= 1.0f)
    {
        style.Alpha = g_isFadingIn ? 1.0f : 0.0f;
        g_isAnimating = false;

        if (!g_isFadingIn)
        {
            g_show = false;
            onClose();
        }

        return;
    }

    style.Alpha = g_isFadingIn ? t : 1.0f - t;
    style.Alpha = GDH::Utils::easeInOut(style.Alpha);
}

void PushAnimateFoundColor(const std::string& hackName) {
    static std::unordered_map<std::string, float> anim;

    std::string search_name = hackName;
    std::string search_item = g_search_text;

    std::transform(search_item.begin(), search_item.end(), search_item.begin(), ::tolower);
    std::transform(search_name.begin(), search_name.end(), search_name.begin(), ::tolower);

    bool founded = search_item.empty() ? true : (search_name.find(search_item) != std::string::npos);

    float& t = anim[hackName];

    float speed = 16.0f;
    float target = founded ? 1.0f : 0.0f;

    t = ImLerp(t, target, ImGui::GetIO().DeltaTime * speed);

    ImVec4 normal = ImGui::GetStyle().Colors[ImGuiCol_Text];
    ImVec4 faded  = ImColor(100, 100, 100);

    ImVec4 col = ImLerp(faded, normal, t);

    ImGui::PushStyleColor(ImGuiCol_Text, col);
}

void ToggleUI()
{
    if (g_isAnimating)
        return;

    g_isFadingIn = !g_show;
    g_isAnimating = true;
    g_animTime = 0.0f;

    if (g_isFadingIn) {
        g_show = true;
        onOpen();
    }
}

void SettingsRender() {
    auto& layoutManager = GDH::Layout::Manager::get();
    auto& config = Config::get();

    std::string windowName = "GDH Settings";
    layoutManager.applyWindowTransform(windowName);

    ImGui::Begin("GDH Settings");

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##Search", "Search:", &g_search_text);

    float hue = config.get<float>("menu-hue", 240.0f);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGuiH::DragFloat("##Menu Hue", &hue, 1.0f, 0.0f, 360.0f, "Menu Hue: %.1fdeg")) {
        config.set<float>("menu-hue", hue);
        ImGuiH::SetMenuHue(hue/360.0f);
    }

    if (layoutManager.isCollecting()) {
        auto size = ImGui::GetWindowSize();
        layoutManager.addWindowInfo(windowName, size.x, size.y);
    }

    ImGui::End();
}

void RenderMain() {
    auto& gui = GDH::Gui::get();
    auto& popup = GDH::Popup::get();
    auto& windows = gui.getWindows();
    auto& config = Config::get();
    auto& layoutManager = GDH::Layout::Manager::get();

    animateAlpha();
    popup.render();

    if (!g_show) return;

    SettingsRender();
    for (auto& window : windows) {
        std::string windowName = window.getName();
        if (windowName == "Invisible") continue;
        
        layoutManager.applyWindowTransform(windowName);

        ImGui::Begin(windowName.c_str());

        if (window.avaibleCustomWindowImGui()) window.callCustomWindowImGui();

        for (auto& hack : window.getHacks()) {
            std::string id = hack.getID();
            std::string hackName = hack.getName();

            bool state = config.get(id, false);

            // if (ImGui::Checkbox(hackName.c_str(), &state)) {


            PushAnimateFoundColor(hackName);
            if (ImGuiH::Checkbox(hackName.c_str(), &state)) {
                hack.toggle();
            }
            ImGui::PopStyleColor();

            ImGuiH::Tooltip(hack.getDesc().c_str(), !hack.getDesc().empty() && ImGui::IsItemHovered());

            if (hack.avaibleCustomWindowImGui()) {
                ImGui::SameLine();
                if (ImGuiH::ArrowButton(fmt::format("{} Settings", hackName).c_str(), ImGuiDir_Right)) {
                    ImGui::OpenPopup(fmt::format("{} Settings##Popup", hackName).c_str());
                }

                if (ImGui::BeginPopup(fmt::format("{} Settings##Popup", hackName).c_str(), NULL)) {
                    hack.callCustomWindowImGui();

                    // if (ImGui::Button("Close", {400, NULL}))
                    //     ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
        }

        if (layoutManager.isCollecting()) {
            auto size = ImGui::GetWindowSize();
            layoutManager.addWindowInfo(windowName, size.x, size.y);
        }
        
        ImGui::End();

    }


    if (layoutManager.isCollecting())
        layoutManager.finishCollecting();
    else if (layoutManager.isApplying())
        layoutManager.finishApplying();
    
    static bool g_inited = false;
    if (!g_inited) {
        layoutManager.setLayout(g_layout);
        layoutManager.setFixedWindowSizeInfo(g_fixedWindowSizes);

        layoutManager.startCollecting();
        g_inited = true;
    }
}

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
		static bool inited = false;
        if (!inited) {
            ImGuiCocos::get().setup([] {
                auto& config = Config::get();
                ImGuiIO &io = ImGui::GetIO();
                io.IniFilename = NULL;
                ApplyGuiColors(false);
                ApplyStyle(1.f);

                io.Fonts->AddFontFromMemoryCompressedTTF(roboto_font_data, roboto_font_size, 18.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
                ImGuiH::SetMenuHue(config.get<float>("menu-hue", 240.0f)/360.0f);
            }).draw([] {
                RenderMain();
            });
            inited = true;
        }

		return true;
    }
};

class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double time) {
        if (down || repeat) {
            if (key == KEY_Tab) ToggleUI();
        }
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, time);
	}
};

#endif
