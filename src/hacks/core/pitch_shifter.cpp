#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../core/utils.hpp"

GUI_HACK_CREATE("Invisible", "Pitch Shifter", "", false);

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& hack = gui.getWindow("Invisible").findHackByName("Pitch Shifter");   
    hack.setEarlyInit(false);

    auto valueKey = hack.formatAdditionalSetting("value"); 
    hack.setHandler([&config, valueKey](bool enabled) {
        GDH::Utils::setPitchShifter(enabled ? config.get<int>(valueKey, 0.f) : 0.f);
    });
};