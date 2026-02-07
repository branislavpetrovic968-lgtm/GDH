#include <Geode/Geode.hpp>
#include <Geode/Modify/PlayerObject.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "core.no_death_effect", "No Death Effect", "Upon death, the cube will not emit an exploding effect", false);

class $modify(NoDeathEffectPlayerObject, PlayerObject) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.no_death_effect");        
        
        hack->addHookPtr(self.getHook("PlayerObject::playDeathEffect").unwrap());
    }

    void playDeathEffect() {}
};