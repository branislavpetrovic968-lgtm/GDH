#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Level", "Menu Gameplay", "Prevents kicking from certain levels", false);

static bool pushPressed = false;
static bool releasePressed = false;

class $modify(MenuGameplayMenuGameLayer, MenuGameLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Menu Gameplay");        
        
        hack.addHookPtr(self.getHook("MenuGameLayer::tryJump").unwrap());
        hack.addHookPtr(self.getHook("MenuGameLayer::update").unwrap());
        hack.addHookPtr(self.getHook("MenuGameLayer::ccTouchBegan").unwrap());
    }

    void tryJump(float dt) {}

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
        return true;
    }

    void update(float dt) override {
        if (m_playerObject && !m_playerObject->m_isSpider) {
            if (pushPressed) {
                pushPressed = false;
                m_playerObject->pushButton(PlayerButton::Jump);
            }
            else if (releasePressed) {
                releasePressed = false;
                m_playerObject->releaseButton(PlayerButton::Jump);
            }
        }

        MenuGameLayer::update(dt);
    }    
};

class $modify(MenuGameplayCCTouchDispatcher, CCTouchDispatcher) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Menu Gameplay");        
        
        hack.addHookPtr(self.getHook("cocos2d::CCTouchDispatcher::touches").unwrap());
    }

	void touches(CCSet* touches, CCEvent* event, unsigned int type) {
		if (type == CCTOUCHBEGAN)
            pushPressed = true;
		else if (type == CCTOUCHENDED || type == CCTOUCHCANCELLED)
            releasePressed = true;

        CCTouchDispatcher::touches(touches, event, type);
	}
};


