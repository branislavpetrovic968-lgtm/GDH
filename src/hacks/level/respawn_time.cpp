#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../interface/imgui/widget_helper.hpp"

GUI_HACK_CREATE("Level", "Respawn Time", "Changes respawn time on death", false);

class $modify(RespawnTimePlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Respawn Time");        
            
        auto hackID = hack.getID();
        auto delayKey = hack.formatAdditionalSetting("delay");

        hack.setCustomWindowImGui([hackID, delayKey]{
            ImGuiWidgetConfig::DragFloat("##Respawn_Time_value", delayKey, 0.01f, 0, FLT_MAX, 1.f, "Respawn Time: %.2fs");
        });

        hack.addHookPtr(self.getHook("PlayLayer::destroyPlayer").unwrap());
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);

        auto& config = Config::get();
        if (auto* respawnSequence = getActionByTag(0x10)) {
            stopAction(respawnSequence);
            auto* newSequence = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(config.get<float>("level.respawn_time::delay", 1.f)),
                cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                nullptr
            );
            newSequence->setTag(0x10);
            runAction(newSequence);
        }
    }
};