#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/AchievementNotifier.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Level", "Hide Achievements", "", false);

class $modify(HideAchievementsAchievementNotifier, AchievementNotifier) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Hide Achievements");        
        
        hack.addHookPtr(self.getHook("AchievementNotifier::notifyAchievement").unwrap());
    }

    void notifyAchievement(char const* title, char const* desc, char const* icon, bool quest) {}
};