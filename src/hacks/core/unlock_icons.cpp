#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Core", "Unlock Icons", "Unlock All Icons", false);

class $modify(UnlockItemsGameManager, GameManager) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Core").findHackByName("Unlock Icons");        
        
        hack->addHookPtr(self.getHook("GameManager::isColorUnlocked").unwrap());
        hack->addHookPtr(self.getHook("GameManager::isIconUnlocked").unwrap());
    }

    bool isColorUnlocked(int key, UnlockType type) {
        return true;
    }

    bool isIconUnlocked(int key, IconType type) {
        return true;
    }
};