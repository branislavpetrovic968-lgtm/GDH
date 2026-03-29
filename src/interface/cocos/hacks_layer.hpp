#pragma once
#include <Geode/Geode.hpp>
#include "../../core/hacks.hpp"

class HacksTab : public cocos2d::CCMenu {
public:
    static HacksTab* create();
    void addToggle(GDH::Hack& hck);
    float x_lastToggle;
    float y_lastToggle;

    geode::prelude::ScrollLayer* m_scrollLayer;

    void addPadding();
private:
    HacksTab() = default;

    bool init();
};

class HacksLayer : public geode::Popup {
private:
    bool init();
    static HacksLayer* instance;

    std::vector<HacksTab*> m_tabs;
    std::vector<CCMenuItemSpriteExtra*> m_buttonTabs;
    int m_index = 0;

    void switchTab(int newIndex);
public:
    ~HacksLayer();

    static HacksLayer* create();
    static HacksLayer* get();

    static bool isOpened();
    void onClose(CCObject* object);
};