#include <Geode/Geode.hpp>
#include <Geode/Modify/PlayerObject.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "core.no_respawn_blink", "No Respawn Blink", "Upon respawning, the cube will not produce an unpleasant flicker", false);

class $modify(NoRespawnBlinkPlayerObject, PlayerObject) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.no_respawn_blink");        
        
        hack->addHookPtr(self.getHook("PlayerObject::playSpawnEffect").unwrap());
    }

    void playSpawnEffect() {}
};