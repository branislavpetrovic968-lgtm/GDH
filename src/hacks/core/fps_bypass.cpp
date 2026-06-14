#ifdef GEODE_IS_WINDOWS
#include <Geode/Geode.hpp>
#include <Geode/binding/AppDelegate.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"

GUI_HACK_CREATE("Invisible", "FPS Bypass", "", false);

$execute {
    auto& config = Config::get();
    auto& gui = GDH::Gui::get();
    auto& hack = gui.getWindow("Invisible").findHackByName("FPS");   
    hack.setGameVariableID(GameVar::UnlockFPS);
    hack.setEarlyInit(false);

    auto valueKey = hack.formatAdditionalSetting("value");

    hack.setHandler([&gui, &config, valueKey](bool enabled) {
        auto& hack = gui.getWindow("Invisible").findHackByName("Vertical Sync");  
        if (hack.getEnabled() && enabled) hack.toggle();

        auto gm = GameManager::get();
        if (gm->m_customFPSTarget == 0) gm->m_customFPSTarget = 60.f;

        auto fps_value = config.get<float>(valueKey, gm->m_customFPSTarget);
        fps_value = std::clamp(fps_value, 10.f, 10000.f);
        gm->m_customFPSTarget = fps_value;

        float dt = 1.f / (enabled ? fps_value : 60.f);
        CCDirector::get()->setAnimationInterval(dt);
    });
}
#endif