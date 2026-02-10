#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"

GUI_HACK_CREATE("Invisible", "Speedhack", "", false);

class $modify(SpeedhackCCScheduler, cocos2d::CCScheduler) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Invisible").findHackByName("Speedhack");        
        
        hack->addHookPtr(self.getHook("cocos2d::CCScheduler::update").unwrap());
    }

    void update(float dt) {
        auto &config = Config::get();
        dt *= config.get<float>("invisible.speedhack::value", 1.f);

        CCScheduler::update(dt);
    }
};