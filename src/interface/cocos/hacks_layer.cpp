#include "hacks_layer.hpp"

HacksLayer* HacksLayer::instance = nullptr;

HacksLayer::~HacksLayer() {
    instance = nullptr;
}

bool HacksLayer::init() {
    if (!Popup::init(240.f, 160.f))
        return false;

    this->setTitle("GDH");

    return true;
}

HacksLayer* HacksLayer::create() {
    auto* ret = new HacksLayer();
    if (ret->init()) { ret->autorelease(); return ret; }
    delete ret;
    return nullptr;
}

HacksLayer* HacksLayer::get() {
    if (!instance) instance = create();
    return instance;
}

bool HacksLayer::isOpened() {
    return instance != nullptr;
}

void HacksLayer::onClose(CCObject* object) {
    Popup::onClose(object);
    instance = nullptr;
}