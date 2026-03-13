#include "imgui.h"
#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../widgetH.hpp"
#include "../widget_helper.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& window = gui.getWindow("GDH Settings");

    Config &c = Config::get();
    ImGuiH::SetMenuHue(c.get<float>("menu-hue", 240.0f)/360.0f);

    window.setCustomWindowImGui([&config, &gui] {
        Config &c = Config::get();
        float hue = c.get<float>("menu-hue", 240.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGuiH::DragFloat("##Menu Hue", &hue, 1.0f, 0.0f, 360.0f, "Menu Hue: %.1fdeg")) {
            c.set<float>("menu-hue", hue);
            ImGuiH::SetMenuHue(hue/360.0f);
        }
    });
}
#endif
