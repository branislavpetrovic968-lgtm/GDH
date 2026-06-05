#ifdef GEODE_IS_WINDOWS
#include "imgui.h"
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "layout.hpp"

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
static bool g_resetLayoutCalled = false;
static bool g_isAnimating = false;
static bool g_isFadingIn = false;
static float g_animTime = 0.0f;

static bool g_inited = false;
static bool g_layoutReady = false;
static std::string g_search_text = "";

static std::vector<std::vector<std::string>> g_layout = {
    {"Core", "Bypass"},
    {"Cosmetic"},
    {"Level", "Framerate", "GDH Settings"},
    {"Creator", "Labels"},
    {"Replay Engine"},
    {"Shortcuts"}
};

static std::vector<GDH::Layout::WindowInfo> g_fixedWindowSizes = {
    {"Cosmetic", 0.f, 675.f},
    {"Labels", 240.f, 350.f},
    {"Replay Engine", 300.f, 200.f},
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
    float deltaTime = ImGui::GetIO().DeltaTime;

    bool cascade = Config::get().get<bool>("gui::cascade_animation", false);
    int default_dur = cascade ? 300 : 100;
    float duration = Config::get().get<int>("gui::anim_durr", default_dur) / 1000.0f;
    
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
}

ImVec2 getRandomOffscreenPos(const std::string& name, ImVec2 displaySize) {
    std::hash<std::string> hasher;
    size_t hash = hasher(name);
    int side = hash % 4;
    float offset = 500.0f;
    
    float randX = ((hash % 100) / 100.0f) * displaySize.x;
    float randY = (((hash >> 2) % 100) / 100.0f) * displaySize.y;

    if (side == 0) return ImVec2(randX, -offset);
    if (side == 1) return ImVec2(randX, displaySize.y + offset);
    if (side == 2) return ImVec2(-offset, randY);
    return ImVec2(displaySize.x + offset, randY);
}

void applyAnimationTransform(const std::string& windowName, GDH::Layout::Manager& layoutManager) {
    auto& config = Config::get();
    layoutManager.applyWindowTransform(windowName);

    bool cascade = config.get<bool>("gui::cascade_animation", false);
    if (!cascade) return;

    if (!g_layoutReady) {
        ImGui::SetNextWindowPos(ImVec2(-10000.f, -10000.f), ImGuiCond_Always);
    } else if (auto* info = layoutManager.getWindowInfo(windowName)) {
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImVec2 startPos = getRandomOffscreenPos(windowName, displaySize);
        
        float eased = 1.0f;
        if (g_isAnimating) {
            float duration = config.get<int>("gui::anim_durr", 300) / 1000.0f;

            float t = ImClamp(g_animTime / duration, 0.0f, 1.0f);
            if (g_isFadingIn) {
                float inv = 1.0f - t;
                eased = 1.0f - (inv * inv * inv);
            } else {
                float inv = 1.0f - t;
                eased = inv * inv * inv;
            }
        }

        ImVec2 currentPos = ImVec2(
            startPos.x + (info->x - startPos.x) * eased,
            startPos.y + (info->y - startPos.y) * eased
        );
        ImGui::SetNextWindowPos(currentPos, ImGuiCond_Always);
    }
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
    ImVec4 faded = ImGui::GetStyle().Colors[ImGuiCol_TextDisabled];

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
    applyAnimationTransform(windowName, layoutManager);

    ImGui::Begin("GDH Settings");

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputTextWithHint("##Search", "Search:", &g_search_text);

    bool cascade_animation = config.get<bool>("gui::cascade_animation", false);
    if (ImGuiH::Checkbox("Cascade Animation", &cascade_animation)) {
        config.set<bool>("gui::cascade_animation", cascade_animation);
        config.set<int>("gui::anim_durr", cascade_animation ? 300 : 100);
    }

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    
    int default_dur = cascade_animation ? 300 : 100;
    int duration = config.get<int>("gui::anim_durr", default_dur);
    if (ImGuiH::DragInt("##gui_anim_durr", &duration, 50.f, 1, 500, "Duration Anim: %dms")) {
        config.set<int>("gui::anim_durr", duration);
    }

    if (ImGuiH::Button("Refresh Layout", {ImGui::GetContentRegionAvail().x, 0})) {
        g_resetLayoutCalled = true;
    }

    if (layoutManager.isCollecting()) {
        auto size = ImGui::GetWindowSize();
        layoutManager.addWindowInfo(windowName, size.x, size.y);
    }

    ImGui::End();
}

void RenderVersionBadge() {
    ImGuiIO& io = ImGui::GetIO();
    auto& layout = GDH::Layout::Manager::get();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImFont* font = ImGui::GetFont();
    
    float fontSize = layout.multipleScale(32); 
    std::string versionText = geode::Mod::get()->getVersion().toVString();

    ImVec2 labelSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, "GDH");
    ImVec2 versionSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, versionText.c_str());

    float pillWidth = layout.multipleScale(24.0f) + labelSize.x;
    float pillHeight = labelSize.y + layout.multipleScale(10.0f);
    float sidePadding = layout.multipleScale(12.0f); 
    float totalWidth = sidePadding + pillWidth + layout.multipleScale(12.0f) + versionSize.x + sidePadding;
    float totalHeight = pillHeight + layout.multipleScale(16.0f);

    float screenPadding = layout.multipleScale(10.0f);
    ImVec2 badgePosMin = ImVec2(io.DisplaySize.x - totalWidth - screenPadding, io.DisplaySize.y - totalHeight - screenPadding);
    ImVec2 badgePosMax = ImVec2(io.DisplaySize.x - screenPadding, io.DisplaySize.y - screenPadding);
    
    ImVec2 pillPosMin = ImVec2(badgePosMin.x + sidePadding, badgePosMin.y + layout.multipleScale(8.0f));
    ImVec2 pillPosMax = ImVec2(pillPosMin.x + pillWidth, pillPosMin.y + pillHeight);
    drawList->AddRectFilled(badgePosMin, badgePosMax, ImGui::GetColorU32(IM_COL32(0, 0, 0, 125)), 999.f);
    drawList->AddRectFilled(pillPosMin, pillPosMax, ImGui::GetColorU32(IM_COL32(213, 196, 255, 225)), 999.f);
    
    ImVec2 labelTextPos = ImVec2(pillPosMin.x + layout.multipleScale(12.0f), pillPosMin.y + layout.multipleScale(5.0f));
    ImVec2 versionTextPos = ImVec2(pillPosMax.x + layout.multipleScale(12.0f), badgePosMin.y + layout.multipleScale(13.0f));
    drawList->AddText(font, fontSize, labelTextPos, ImGui::GetColorU32(IM_COL32(33, 33, 78, 225)), "GDH");
    drawList->AddText(font, fontSize, versionTextPos, ImGui::GetColorU32(IM_COL32(213, 196, 255, 225)), versionText.c_str());
}

void RenderMain() {
    auto& gui = GDH::Gui::get();
    auto& windows = gui.getWindows();
    auto& config = Config::get();
    auto& layoutManager = GDH::Layout::Manager::get();
    
    animateAlpha();

    ImGuiH::RenderPopups();
    
    if (!g_show) return;

    RenderVersionBadge();
    SettingsRender();
    for (auto& window : windows) {
        std::string windowName = window.getName();
        if (windowName == "Invisible") continue;
        
        applyAnimationTransform(windowName, layoutManager);

        (windowName == "Framerate") ? ImGui::Begin(windowName.c_str()) : ImGuiH::BeginSmoothScroll(windowName.c_str());
        
        if (window.avaibleCustomWindowImGui()) window.callCustomWindowImGui();

        for (auto& hack : window.getHacks()) {
            std::string id = hack.getID();
            std::string hackName = hack.getName();

            bool state = config.get(id, false);

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
                    ImGui::EndPopup();
                }
            }
        }

        if (layoutManager.isCollecting()) {
            auto size = ImGui::GetWindowSize();
            layoutManager.addWindowInfo(windowName, size.x, size.y);
        }
        
        ImGuiH::EndSmoothScroll();
    }

    if (layoutManager.isCollecting()) layoutManager.finishCollecting();
    else if (layoutManager.isApplying()) layoutManager.finishApplying();
    
    if (g_inited) {
        static ImVec2 lastSize = ImVec2(0, 0);
        ImVec2 currentSize = ImGui::GetIO().DisplaySize;
    
        if (currentSize.x != lastSize.x || currentSize.y != lastSize.y) {
            layoutManager.startCollecting(); 
            lastSize = currentSize;
        }
    
        if (g_resetLayoutCalled) {
            g_resetLayoutCalled = false;
            layoutManager.startApplying();
        }
    }
    else {
        layoutManager.setLayout(g_layout);
        layoutManager.setFixedWindowSizeInfo(g_fixedWindowSizes);

        layoutManager.startCollecting();
        g_inited = true;
    }

    if (!g_layoutReady && g_inited) {
        g_layoutReady = true;
    }
}

class $modify(ImGuiInitMenuLayer, MenuLayer) {
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