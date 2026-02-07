#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "core.music_unlocker", "Music Unlocker", "Unlocks RobTop's \"Practice Music Sync\" and Music Customizer", false);

class $modify(MusicUnlockerGameStatsManager, GameStatsManager) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHack("core.music_unlocker");        
        
        hack->addHookPtr(self.getHook("GameStatsManager::isItemUnlocked").unwrap());
    }

    bool isItemUnlocked(UnlockType type, int key) {
        if (GameStatsManager::isItemUnlocked(type, key)) return true;

        if (type == UnlockType::GJItem && key == 17)
            return true;

        if (type == UnlockType::GJItem && key == 16)
            return true;

        return false;
    }
};