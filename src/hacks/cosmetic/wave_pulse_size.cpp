#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <imgui-cocos.hpp>
#include "../../core/config.hpp"
#include "../../core/gui.hpp"
#include "../../interface/imgui/widget_helper.hpp"

GUI_HACK_CREATE("Cosmetic", "Wave Pulse Size", "Resizes the wave trail", false);

class $modify(MyHardStreak, HardStreak) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Cosmetic").findHackByName("Wave Pulse Size");        

        auto multiplyKey = hack.formatAdditionalSetting("multiply");
        auto noPulseKey = hack.formatAdditionalSetting("noPulse");
        auto valueKey = hack.formatAdditionalSetting("value");

        hack.addHookPtr(self.getHook("HardStreak::updateStroke").unwrap());
        hack.setCustomWindowImGui([multiplyKey, noPulseKey, valueKey]() {
            ImGuiWidgetConfig::Checkbox("No Pulse", noPulseKey, false);
            ImGuiWidgetConfig::Checkbox("Multiply pulsation", multiplyKey, true);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGuiWidgetConfig::DragFloat("##PulseSizeValue", valueKey, 0.01f, 0, FLT_MAX, 0.5, "Pulse Size: %.2f");
        });
    }

    void updateStroke(float p0) {
        auto& config = Config::get();

        bool noPulse = config.get<bool>("cosmetic.wave_pulse_size::noPulse", false);

        if (!noPulse) {
            bool multiply = config.get<bool>("cosmetic.wave_pulse_size::multiply", true);
            float value = config.get<float>("cosmetic.wave_pulse_size::value", 0.5f);

            if (multiply)
                m_pulseSize *= value;
            else 
                m_pulseSize = value;
        }
        else {
            m_pulseSize = 2.f;
        }

        HardStreak::updateStroke(p0);
    }
};