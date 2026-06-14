#include "hacks.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "keybinds.hpp"

using namespace GDH;

void Hack::callHandler(bool state) {
    if (m_handlerFunc)
        m_handlerFunc(state);

    geode::log::debug("{} - handler called", m_ID);
}

bool Hack::getEnabled() const {
    return Config::get().get<bool>(m_ID, false);
}

void Hack::setGameVariableID(const std::string& key) {
    auto& config = Config::get();

    this->m_key = key;
    bool enabled = GameManager::get()->getGameVariable(key.c_str());
    config.set<bool>(m_ID, enabled);
}

void Hack::setEarlyInit(bool value) {
    m_earlyInit = value;
}

bool Hack::getEarlyInit() const {
    return m_earlyInit;
}

void Hack::toggle() {
    auto& config = Config::get();
    bool state = !getEnabled();

    config.set(m_ID, state);
    enableHooks(state);

    if (!m_key.empty()) 
        GameManager::get()->setGameVariable(this->m_key.c_str(), state);

    callHandler(state);
}

void Hack::setHandler(std::function<void(bool)> func) {
    auto& config = Config::get();
    m_handlerFunc = std::move(func);

    if (m_earlyInit && config.get(m_ID, false))
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

void GDH::Hack::setKeybind(geode::Keybind const& keybind) {
    m_keybind = keybind;
    GDH::Keybinds::get().rebuildCache();
}