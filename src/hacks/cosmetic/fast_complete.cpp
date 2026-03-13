#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../interface/imgui/widget_helper.hpp"

GUI_HACK_CREATE("Cosmetic", "Fast Level Complete", "Fast level completion without animations", false);

class $modify(FastLevelCompletePlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Cosmetic").findHackByName("Fast Level Complete");     
        auto hackID = hack.getID();
        auto veryFastKey = hack.formatAdditionalSetting("veryFast");

        hack.setCustomWindowImGui([hackID, veryFastKey]{
            ImGuiWidgetConfig::Checkbox("Very Fast", veryFastKey, false);
        });   
        
        hack.addHookPtr(self.getHook("PlayLayer::showCompleteEffect").unwrap());
    }

    void showCompleteEffect() {
        auto& config = Config::get();
        bool veryFast = config.get<bool>("cosmetic.fast_level_complete::veryFast", false);
        runAction(
            cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0),
                cocos2d::CCCallFunc::create(this,
                veryFast ? callfunc_selector(PlayLayer::showEndLayer) : callfunc_selector(PlayLayer::showCompleteText)),
                nullptr
            )
        );
    }
};