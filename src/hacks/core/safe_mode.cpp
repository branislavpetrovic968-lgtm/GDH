#include <Geode/Geode.hpp>
#include <Geode/Modify/PlayLayer.hpp>
#include <Geode/Modify/PlayerObject.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "core.safe_mode", "Safe Mode", "Disables progress on levels", false);

class $modify(SafeModePlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.safe_mode");        
        
        hack->addHookPtr(self.getHook("PlayLayer::destroyPlayer").unwrap());
        hack->addHookPtr(self.getHook("PlayLayer::resetLevel").unwrap());
        hack->addHookPtr(self.getHook("PlayLayer::levelComplete").unwrap());
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        bool testmode = m_isTestMode;
        m_isTestMode = true;
        PlayLayer::destroyPlayer(player, obj);
        m_isTestMode = testmode;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        m_level->m_attempts = m_level->m_attempts - 1;
    }

    void levelComplete() {
        bool testmode = m_isTestMode;
        m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = testmode;
    }
};

class $modify(SafeModePlayerObject, PlayerObject) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.safe_mode");        
        
        hack->addHookPtr(self.getHook("PlayerObject::incrementJumps").unwrap());
    }

    void incrementJumps() {}
};