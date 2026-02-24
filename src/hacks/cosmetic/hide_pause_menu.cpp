#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Cosmetic", "Hide Pause Menu", "Hides the pause menu, useful for taking screenshots", false);

class $modify(HidePauseButtonPauseLayer, PauseLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Cosmetic").findHackByName("Hide Pause Menu");  
        
        hack.addHookPtr(self.getHook("PauseLayer::customSetup").unwrap());
        hack.setHandler([](bool enabled) {
            if (auto scene = cocos2d::CCScene::get()) {
                auto pauseLayer = scene->getChildByType<PauseLayer*>(0);
                if (pauseLayer) pauseLayer->setVisible(!enabled);
            }
        });
    }
    
    void customSetup() {
        this->setVisible(false);
        PauseLayer::customSetup();
    }
};