#ifdef GEODE_IS_WINDOWS
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>

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

class $modify(cocos2d::CCEGLView) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("cocos2d::CCEGLView::onGLFWKeyCallback", geode::Priority::First);
    }

    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_TAB) ToggleUI();
        } 
    }    
};

#endif