#ifdef GEODE_IS_WINDOWS
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

#include "../../core/gui.hpp"

bool toggle = false;

void ToggleUI() {
    toggle = !toggle;
    if (!toggle) {
        Config::get().save(fileDataPath);
    }
}

void RenderMain() {
    auto& gui = GDH::Gui::get();
    auto& windows = gui.getWindows();
    auto& config = Config::get();

    if (!toggle) return;

    for (auto& window : windows) {
        ImGui::Begin(window.getName().c_str());

        for (auto& hack : window.getHacks()) {
            std::string id = hack.getID();
            bool state = config.get(id, false);

            if (ImGui::Checkbox(hack.getName().c_str(), &state)) {
                config.set(id, state);
                hack.enableHooks(state);
                hack.callHandler(state);
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", hack.getDesc().c_str());

            if (hack.avaibleCustomWindowImGui()) {
                ImGui::SameLine();
                if (ImGui::ArrowButton(fmt::format("{} Settings", hack.getName()).c_str(), ImGuiDir_Right)) {
                    ImGui::OpenPopup(fmt::format("{} Settings##Popup", hack.getName()).c_str());
                }

                if (ImGui::BeginPopupModal(fmt::format("{} Settings##Popup", hack.getName()).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    hack.callCustomWindowImGui();

                    if (ImGui::Button("Close", {400, NULL}))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
            }
        }

        ImGui::End();
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