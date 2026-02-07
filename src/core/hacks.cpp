#include "hacks.hpp"
#include "config.hpp"

using namespace GDH;

void Hack::callHandler(bool state) {
    if (m_handlerFunc)
        m_handlerFunc(state);

    geode::log::debug("{} - handler called", m_ID);
}

void Hack::setHandler(std::function<void(bool)> func) {
    auto& config = Config::get();
    m_handlerFunc = std::move(func);

    if (config.get(m_ID, false))
        callHandler(true);
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