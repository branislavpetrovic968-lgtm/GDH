#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../core/utils.hpp"
#include "Geode/cocos/CCDirector.h"
#include "imgui.h"
#include <imgui-cocos.hpp>

GUI_HACK_CREATE("Level", "Accurate Percentage", "Shows decimals in level progress", false);

class $modify(AccuratePercentagePlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Level").findHackByName("Accurate Percentage");        
        
        hack->addHookPtr(self.getHook("PlayLayer::updateProgressbar").unwrap());
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

$execute {
    auto& config = Config::get();
    auto& gui = GDH::Gui::get();
    auto* hack = gui.getWindow("Level").findHackByName("Accurate Percentage");   

    auto hackID = hack->getID();
    auto decimalsKey = hack->formatAdditionalSetting("demicals");
    auto showMillisecondsKey = hack->formatAdditionalSetting("showMilliseconds");

    hack->setCustomWindowImGui([&config, hackID, decimalsKey, showMillisecondsKey]{
        int demicals = config.get<int>(decimalsKey, 2);
        if (ImGui::InputInt("Demicals", &demicals, 1))
            config.set<int>(decimalsKey, demicals);

        bool showMilliseconds = config.get<bool>(showMillisecondsKey, true);
        if (ImGui::Checkbox("Show time in Milliseconds", &showMilliseconds))
            config.set<bool>(showMillisecondsKey, showMilliseconds);
    });
}
