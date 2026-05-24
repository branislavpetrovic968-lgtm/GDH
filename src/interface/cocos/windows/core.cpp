#include <Geode/Geode.hpp>
#include "../hacks_tab.hpp"
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/utils.hpp"

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& window = gui.getWindow("Core");

    window.setCustomWindowCocos([&config, &gui](cocos2d::CCNode* node) {
        auto tab = static_cast<HacksTab*>(node);
        
        tab->addSeparator();
        tab->prepareNewRow();

        tab->addConfigFloatInput("Speedhack Value", "invisible.speedhack::value", 1.f, 0.01f, 500.f);
        tab->addConfigToggle("Speedhack Enabled", "invisible.speedhack");
        tab->addConfigToggle("Speedhack Audio", "invisible.speedhack_audio");

        tab->addSeparator();
        tab->prepareNewRow();

        tab->addConfigIntInput("Pitch Shifter (-24 to 24)", "invisible.pitch_shifter::value", 1.f, -12, 12, [&config](int value) {
            bool enabled = config.get<bool>("invisible.pitch_shifter", false);
            GDH::Utils::setPitchShifter(enabled ? config.get<int>("invisible.pitch_shifter::value", 0) : 0);
            geode::log::debug("{}", value);
        });
        tab->addConfigToggle("Pitch Shifter Enabled", "invisible.pitch_shifter");
    });
}