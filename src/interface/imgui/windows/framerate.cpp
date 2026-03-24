#include "imgui.h"
#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/utils.hpp"
#include "../widget_helper.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& window = gui.getWindow("Framerate");

    window.setCustomWindowImGui([&config, &gui] {
        // ImGuiWidgetConfig::DrawValueToggle("fps_value", 1, 1, FLT_MAX, 60.f, "%0.f FPS");
        // ImGuiWidgetConfig::DrawValueToggle("tps_value", 1, 1, FLT_MAX, 240.f, "%0.f TPS");
        const ImGuiStyle& style = ImGui::GetStyle();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (43.f + style.ItemSpacing.x));
        ImGuiWidgetConfig::DragFloat("##Speedhack_Value", "invisible.speedhack::value", 0.01f, 0, FLT_MAX, 1.f, "Speed: %.2fx");
        ImGui::SameLine();
        ImGuiWidgetConfig::HackCheckbox("##Speedhack_Enabled", "invisible.speedhack", false);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (43.f + style.ItemSpacing.x));
        if (ImGuiWidgetConfig::DragInt("##Pitch_Shifter_Value", "invisible.pitch_shifter::value", 1.f, -12, 12, 0, "Pitch: %d ST")) {
            bool enabled = config.get<bool>("invisible.pitch_shifter", false);
            GDH::Utils::setPitchShifter(enabled ? config.get<int>("invisible.pitch_shifter::value", 0) : 0);
        }
        ImGui::SameLine();
        ImGuiWidgetConfig::HackCheckbox("##Pitch_Shifter_Enabled", "invisible.pitch_shifter", false);
        
        ImGuiWidgetConfig::HackCheckbox("Speedhack Audio", "invisible.speedhack_audio", false);
        ImGuiWidgetConfig::HackCheckbox("Vertical Sync", "invisible.vertical_sync", false);
    });
}
#endif
