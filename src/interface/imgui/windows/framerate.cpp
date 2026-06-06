#ifdef GEODE_IS_WINDOWS
#include "imgui.h"
#include <Geode/Geode.hpp>
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/utils.hpp"
#include "../widget_helper.hpp"
#include "../layout.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& layout = GDH::Layout::Manager::get();
    auto& window = gui.getWindow("Framerate");

    window.setCustomWindowImGui([&config, &gui, &layout] {
        // ImGuiWidgetConfig::DrawValueToggle("fps_value", 1, 1, FLT_MAX, 60.f, "%0.f FPS");
        // ImGuiWidgetConfig::DrawValueToggle("tps_value", 1, 1, FLT_MAX, 240.f, "%0.f TPS");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - layout.multipleScale(52.f));
        ImGuiWidgetConfig::DragFloat("##TPS_Value", "invisible.tps::value", 1, 1, FLT_MAX, 240.f, "%.0f TPS");
        ImGui::SameLine();
        ImGuiWidgetConfig::HackCheckbox("##TPS_Enabled", "invisible.tps", false);
        
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - layout.multipleScale(52.f));
        ImGuiWidgetConfig::DragFloat("##Speedhack_Value", "invisible.speedhack::value", 0.01f, 0, 500.f, 1.f, "Speed: %.2fx");
        ImGui::SameLine();
        ImGuiWidgetConfig::HackCheckbox("##Speedhack_Enabled", "invisible.speedhack", false);

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - layout.multipleScale(52.f));
        if (ImGuiWidgetConfig::DragInt("##Pitch_Shifter_Value", "invisible.pitch_shifter::value", 1.f, -24, 24, 0, "Pitch: %d ST")) {
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
