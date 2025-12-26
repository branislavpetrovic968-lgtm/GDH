#include "hacks.hpp"

using namespace GDH;

void Hack::callHandler(bool state) {
    if (m_handlerFunc)
        m_handlerFunc(state);
}

void Hack::addHookPtr(geode::Hook* ptr) { 
    auto& config = Config::get();
    m_hooksPtr.insert(ptr); 
    (void)ptr->setAutoEnable(config.get(m_ID, false));
}

void Hack::enableHooks(bool state) {
    for (geode::Hook* hookPtr : m_hooksPtr) {
        (void)hookPtr->toggle(state);
    }
}