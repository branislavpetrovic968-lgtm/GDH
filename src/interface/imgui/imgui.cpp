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

static bool m_show = false;
static bool m_isAnimating = false;
static bool m_isFadingIn = false;
static float m_animTime = 0.0f;

static bool m_inited = false;

static std::vector<std::vector<std::string>> m_layout = {
    {"Core", "Bypass"},
    {"Cosmetic"},
    {"Level", "Framerate"},
    {"Creator", "Labels"},
    {"Shortcuts"}
};

static std::vector<GDH::Layout::WindowInfo> m_fixedWindowSizes = {
    {"Labels", 240.f, 350.f},
    {"Shortcuts", 180.f, 0.f}
};

void onOpen() {
    GDH::Utils::updateCursorState(m_show);
}

void onClose() {
    GDH::Utils::updateCursorState(m_show);
    Config::get().save(fileDataPath);
}

void animateAlpha()
{
    if (!m_isAnimating)
        return;

    ImGuiStyle& style = ImGui::GetStyle();
    float deltaTime   = ImGui::GetIO().DeltaTime;

    float duration = 100 / 1000.0f;
    m_animTime += deltaTime;

    float t = m_animTime / duration;
    if (t >= 1.0f)
    {
        style.Alpha = m_isFadingIn ? 1.0f : 0.0f;
        m_isAnimating = false;

        if (!m_isFadingIn)
        {
            m_show = false;
            onClose();
        }

        return;
    }

    style.Alpha = m_isFadingIn ? t : 1.0f - t;
    style.Alpha = GDH::Utils::easeInOut(style.Alpha);
}

void ToggleUI()
{
    if (m_isAnimating)
        return;

    m_isFadingIn = !m_show;
    m_isAnimating = true;
    m_animTime = 0.0f;

    if (m_isFadingIn) {
        m_show = true;
        onOpen();
    }
}

void RenderMain() {
    auto& gui = GDH::Gui::get();
    auto& popup = GDH::Popup::get();
    auto& windows = gui.getWindows();
    auto& config = Config::get();
    auto& layoutManager = GDH::Layout::Manager::get();

    animateAlpha();
    popup.render();

    if (!m_show) return;

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
            if (ImGuiH::Checkbox(hackName.c_str(), &state)) {
                hack.toggle();
            }

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
    
    static bool m_inited = false;
    if (!m_inited) {
        layoutManager.setLayout(m_layout);
        layoutManager.setFixedWindowSizeInfo(m_fixedWindowSizes);

        layoutManager.startCollecting();
        m_inited = true;
    }
}

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
		static bool inited = false;
        if (!inited) {
            ImGuiCocos::get().setup([] {
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
