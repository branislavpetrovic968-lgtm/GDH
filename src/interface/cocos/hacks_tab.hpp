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

    void addPadding(float height = 0.f);
private:
    HacksTab() = default;

    bool init();
};