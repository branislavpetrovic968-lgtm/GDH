#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../core/utils.hpp"
#include "../../interface/imgui/widget_helper.hpp"
#include <imgui-cocos.hpp>

GUI_HACK_CREATE("Cosmetic", "Accurate Percentage", "Shows decimals in level progress", false);

class $modify(AccuratePercentagePlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Cosmetic").findHackByName("Accurate Percentage");

        hack.addHookPtr(self.getHook("PlayLayer::updateProgressbar").unwrap());

        auto hackID = hack.getID();
        auto decimalsKey = hack.formatAdditionalSetting("demicals");
        auto showMillisecondsKey = hack.formatAdditionalSetting("showMilliseconds");

        hack.setCustomWindowImGui([hackID, decimalsKey, showMillisecondsKey]{
            ImGuiWidgetConfig::InputInt("Demicals", decimalsKey, 2);
            ImGuiWidgetConfig::Checkbox("Show time in Milliseconds", showMillisecondsKey, true);
        });
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();
        if (m_percentageLabel == nullptr) return;

        auto& config = Config::get();
        if (m_level->isPlatformer()) {
            bool showMilliseconds = config.get<bool>("level.accurate_percentage::showMilliseconds", true);
            m_percentageLabel->setString(GDH::Utils::formatTime(m_timePlayed, showMilliseconds).c_str());
        }
        else {
            float percentage = getCurrentPercent();
            int decimals = config.get<int>("level.accurate_percentage::demicals", 2);
            m_percentageLabel->setString(fmt::format("{:.{}f}%", percentage, decimals).c_str());
        }
    }
};