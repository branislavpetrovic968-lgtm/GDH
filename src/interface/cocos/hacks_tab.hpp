#pragma once
#include <Geode/Geode.hpp>
#include "../../core/hacks.hpp"
#include <functional>

class HacksTab : public cocos2d::CCMenu {
public:
    static HacksTab* create();
    void addToggle(GDH::Hack& hck);
    
    void addConfigToggle(const std::string& label, const std::string& key, bool defaultValue = false);
    void addConfigIntInput(const std::string& label, const std::string& key, int defaultValue = 0, int min = 0, int max = 100, std::function<void(int)> callback = nullptr);
    void addConfigFloatInput(const std::string& label, const std::string& key, float defaultValue = 0.f, float min = 0.f, float max = 100.f, std::function<void(float)> callback = nullptr);
    
    void addText(const std::string& text, float scale = 0.5f);
    void prepareNewRow();

    geode::prelude::ScrollLayer* m_scrollLayer;
    cocos2d::CCMenu* m_currentRow = nullptr;

    void addPadding(float height = 0.f);
    void addSeparator(float height = 1.f);
private:
    HacksTab() = default;
    bool init();
};