#pragma once
#include <Geode/Geode.hpp>

class HacksLayer : public geode::Popup {
    bool init();

    static HacksLayer* instance;
public:
    ~HacksLayer();

    static HacksLayer* create();
    static HacksLayer* get();

    static bool isOpened();
    void onClose(CCObject* object);
};