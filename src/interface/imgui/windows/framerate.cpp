#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../widget_helper.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& window = gui.getWindow("Framerate");

    window.setCustomWindowImGui([&config, &gui] {
        // ImGuiWidgetConfig::DrawValueToggle("fps_value", 1, 1, FLT_MAX, 60.f, "%0.f FPS");
        // ImGuiWidgetConfig::DrawValueToggle("tps_value", 1, 1, FLT_MAX, 240.f, "%0.f TPS");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (35 + 5));
        ImGuiWidgetConfig::DragFloat("##Speedhack_Value", "invisible.speedhack::value", 0.01f, 0, FLT_MAX, 1.f, "Speed: %.2fx");
        ImGui::SameLine();
        ImGuiWidgetConfig::HackCheckbox("##Speedhack Enabled", "Invisible", "invisible.speedhack", false);
        
        ImGuiWidgetConfig::HackCheckbox("Speedhack Audio", "Invisible", "invisible.speedhack_audio", false);
    });
}
#endif
