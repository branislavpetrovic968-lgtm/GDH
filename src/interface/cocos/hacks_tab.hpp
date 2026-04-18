#pragma once
#include <Geode/Geode.hpp>
#include "../../core/hacks.hpp"

class HacksTab : public cocos2d::CCMenu {
public:
    static HacksTab* create();
    void addToggle(GDH::Hack& hck);
    
    geode::prelude::ScrollLayer* m_scrollLayer;
    cocos2d::CCMenu* m_currentRow = nullptr;

    void addPadding(float height = 0.f);
private:
    HacksTab() = default;
    bool init();
};