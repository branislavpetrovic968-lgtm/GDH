#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "core.noclip", "Noclip", "The player will be invincible to obstacles", true);

class $modify(NoclipPlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.noclip");        
        
        hack->addHookPtr(self.getHook("PlayLayer::destroyPlayer").unwrap());
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (m_anticheatSpike && obj == m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, obj);
    }
};