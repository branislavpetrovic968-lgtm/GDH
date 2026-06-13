#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../core/keybinds.hpp"
#include "../../interface/imgui/widget_helper.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"

using namespace geode::prelude;

GUI_HACK_CREATE("Level", "Startpos Switcher", "The ability to switch between starting positions using the keys that you setted in keybinds", false);

class $modify(StartposSwitcherPlayLayer, PlayLayer) {
    struct Fields {
        std::vector<StartPosObject*> m_startPositions;
        int m_selectedStartpos = -1;
        cocos2d::CCMenu* startposSwitcherUI = nullptr;

        ~Fields() {
            auto& kb = GDH::Keybinds::get();
            kb.removeCallback("startpos_switcher::left_key");
            kb.removeCallback("startpos_switcher::right_key");
        }
    };

    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Startpos Switcher");        
        
        hack.addHookPtr(self.getHook("PlayLayer::addObject").unwrap());
        hack.addHookPtr(self.getHook("PlayLayer::createObjectsFromSetupFinished").unwrap());

        auto resetCamera_key = hack.formatAdditionalSetting("reset_camera");

        hack.setCustomWindowImGui([resetCamera_key] {
            ImGuiWidgetConfig::Checkbox("Reset Camera", resetCamera_key, false);
            ImGuiWidgetConfig::DrawCustomKeybindButton("startpos_switcher::left", "Left Switch", geode::Keybind(cocos2d::KEY_Q, KeyboardModifier::None));
            ImGuiWidgetConfig::DrawCustomKeybindButton("startpos_switcher::right", "Right Switch", geode::Keybind(cocos2d::KEY_E, KeyboardModifier::None));
        });
    }

    void switchStartPos(int incBy, bool direction = true) {
        auto fields = m_fields.self();
        auto pl = PlayLayer::get();
        auto& config = Config::get();

        if (!pl || fields->m_startPositions.empty() || pl->m_levelEndAnimationStarted) return;

        fields->m_selectedStartpos += incBy;

        if (fields->m_selectedStartpos < -1)
            fields->m_selectedStartpos = fields->m_startPositions.size() - 1;

        if (fields->m_selectedStartpos >= fields->m_startPositions.size())
            fields->m_selectedStartpos = -1;

        if (direction) {
            StartPosObject* obj = fields->m_selectedStartpos == -1 ? nullptr : fields->m_startPositions[fields->m_selectedStartpos];
            
            bool practiceMode = pl->m_isPracticeMode;
            if (practiceMode) {
                pl->togglePracticeMode(false);
            }

            pl->m_isTestMode = (obj != nullptr);
            pl->m_currentCheckpoint = nullptr;
            pl->setStartPosObject(obj);
            pl->resetLevel();

            if (config.get<bool>("level.startpos_switcher::reset_camera", false))
                pl->resetCamera();

            pl->startMusic();

            if (practiceMode) {
                pl->togglePracticeMode(true);
            }
        }
    }

    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);

        if (obj->m_objectID == 31) {
            m_fields->m_startPositions.push_back(static_cast<StartPosObject*>(obj));
        }
    }

    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        
        auto& config = Config::get();
        auto fields = m_fields.self();

        std::sort(fields->m_startPositions.begin(), fields->m_startPositions.end(), 
            [](StartPosObject* a, StartPosObject* b) {
                return a->getPositionX() < b->getPositionX();
            });

        fields->m_selectedStartpos = -1;
        if (m_startPosObject) {
            auto& positions = fields->m_startPositions;
            for (int i = 0; i < positions.size(); i++) {
                if (positions[i] == m_startPosObject) {
                    fields->m_selectedStartpos = i;
                    break;
                }
            }
        }

        if (!fields->m_startPositions.empty()) {
            auto director = cocos2d::CCDirector::sharedDirector();
            auto win_size = director->getWinSize();
            float bottom = director->getScreenBottom();

            auto label = cocos2d::CCLabelBMFont::create(fmt::format("{}/{}", fields->m_selectedStartpos+1, fields->m_startPositions.size()).c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setPosition(win_size.width / 2, 20.f);
            label->setOpacity(100);
            label->setID("startposSwitcherLabels"_spr);

            auto left_arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
            left_arrow->setScale(0.5f);
            auto left_arrowClick = geode::cocos::CCMenuItemExt::createSpriteExtra(left_arrow, [this, label, fields](CCMenuItemSpriteExtra* sender) {
                switchStartPos(-1);
                label->setCString(fmt::format("{}/{}", fields->m_selectedStartpos+1, fields->m_startPositions.size()).c_str());
            });
            left_arrowClick->setPosition(win_size.width / 2 - 50, bottom + left_arrowClick->getScaledContentHeight());
            left_arrowClick->setOpacity(100);
            left_arrowClick->setID("startposSwitcherLeftArrowClick"_spr);

            auto right_arrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");  
            right_arrow->setScale(0.5f);
            right_arrow->setFlipX(true);
            auto right_arrowClick = geode::cocos::CCMenuItemExt::createSpriteExtra(right_arrow, [this, label, fields](CCMenuItemSpriteExtra* sender) {
                switchStartPos(1);
                label->setCString(fmt::format("{}/{}", fields->m_selectedStartpos+1, fields->m_startPositions.size()).c_str());
            });       
            right_arrowClick->setPosition(win_size.width / 2 + 50, bottom + right_arrowClick->getScaledContentHeight());
            right_arrowClick->setOpacity(100);
            right_arrowClick->setID("startpos_switcher_rightArrowClick"_spr);

            fields->startposSwitcherUI = cocos2d::CCMenu::create();
            fields->startposSwitcherUI->setID("startposSwitcherUI"_spr);
            fields->startposSwitcherUI->setPosition(0, 0);
            fields->startposSwitcherUI->setZOrder(999);

            fields->startposSwitcherUI->addChild(left_arrowClick);
            fields->startposSwitcherUI->addChild(right_arrowClick);
            fields->startposSwitcherUI->addChild(label);

            m_uiLayer->addChild(fields->startposSwitcherUI);

            auto& kb = GDH::Keybinds::get();
            kb.addCallback("startpos_switcher::left", geode::Keybind(cocos2d::KEY_Q, KeyboardModifier::None), [left_arrowClick]() {
                if (auto pl = PlayLayer::get()) 
                    left_arrowClick->activate();
            });

            kb.addCallback("startpos_switcher::right", geode::Keybind(cocos2d::KEY_E, KeyboardModifier::None), [right_arrowClick]() {
                if (auto pl = PlayLayer::get()) 
                    right_arrowClick->activate();
            });
        }
    }

    // void togglePracticeMode(bool practiceMode) {
    //     auto fields = m_fields.self();
    //     if (fields->startposSwitcherUI) {
    //         fields->startposSwitcherUI->setVisible(!practiceMode);
    //     }
    //     PlayLayer::togglePracticeMode(practiceMode);
    // }
};