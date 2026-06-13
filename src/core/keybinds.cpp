#include "keybinds.hpp"
#include "gui.hpp"
#include "config.hpp"

using namespace geode::prelude;
using namespace GDH;

void Keybinds::init() {
    load();
    rebuildCache();

    auto listener = geode::KeyboardInputEvent().listen([this](geode::KeyboardInputData& event) {
        if (event.action == geode::KeyboardInputData::Action::Press) {
            m_pressedKeys.insert(event.key);
            
            if (!m_recordingWindow.empty() || !m_recordingCustomId.empty()) {
                if (event.key == cocos2d::KEY_LeftControl || event.key == cocos2d::KEY_RightControl ||
                    event.key == cocos2d::KEY_LeftShift || event.key == cocos2d::KEY_RightShift || event.key == cocos2d::KEY_Alt) {
                    return ListenerResult::Stop;
                }

                auto bind = (event.key == cocos2d::KEY_Escape) ? geode::Keybind() : geode::Keybind(event.key, event.modifiers);

                if (!m_recordingCustomId.empty()) {
                    changeBind(m_recordingCustomId, bind);
                } else {
                    Gui::get().getWindow(m_recordingWindow).findHackByName(m_recordingHack).setKeybind(bind);
                }

                stopRecording();
                save();
                rebuildCache(); 
                return ListenerResult::Stop;
            }
            
            geode::Keybind pressedBind(event.key, event.modifiers);
            
            for (const auto& [id, customPair] : m_customBinds) {
                if (customPair.first == pressedBind && customPair.second) {
                    customPair.second();
                    return ListenerResult::Stop;
                }
            }

            if (m_bindsMap.contains(pressedBind)) {
                for (const auto& [windowName, hackName] : m_bindsMap[pressedBind]) {
                    Gui::get().getWindow(windowName).findHackByName(hackName).toggle();
                }
                return ListenerResult::Stop;
            }
        }
        else if (event.action == geode::KeyboardInputData::Action::Release) {
            m_pressedKeys.erase(event.key);
            
            if (!m_recordingWindow.empty() || !m_recordingCustomId.empty()) {
                return ListenerResult::Stop;
            }
        }

        return ListenerResult::Propagate;
    });
    listener.leak();

    auto mouseListener = geode::MouseInputEvent().listen([this](geode::MouseInputData& event) {
        if (event.action == geode::MouseInputData::Action::Press) {
            m_pressedMouseButtons.insert(event.button);
        } else if (event.action == geode::MouseInputData::Action::Release) {
            m_pressedMouseButtons.erase(event.button);
        }
        return ListenerResult::Propagate;
    });
    mouseListener.leak();
}

void Keybinds::rebuildCache() {
    m_bindsMap.clear();
    for (auto& window : Gui::get().getWindows()) {
        for (auto& hack : window.getHacks()) {
            if (hack.getKeybind().key != cocos2d::KEY_None) {
                m_bindsMap[hack.getKeybind()].push_back({window.getName(), hack.getName()});
            }
        }
    }
}

void Keybinds::load() {
    std::ifstream file(keybindsDataPath);
    if (!file.is_open()) return;

    nlohmann::json jsonObject = nlohmann::json::parse(file, nullptr, false);
    if (jsonObject.is_discarded()) return;

    if (jsonObject.contains("_custom_actions") && jsonObject["_custom_actions"].is_object()) {
        for (const auto& [id, bindValue] : jsonObject["_custom_actions"].items()) {
            if (bindValue.is_object() && bindValue.contains("key") && bindValue.contains("modifiers")) {
                auto key = static_cast<cocos2d::enumKeyCodes>(bindValue["key"].get<int>());
                auto modifiers = static_cast<geode::KeyboardModifier>(bindValue["modifiers"].get<int>());
                m_customBinds[id].first = geode::Keybind(key, modifiers);
            }
        }
    }

    for (const auto& [jsonKey, bindValue] : jsonObject.items()) {
        if (jsonKey == "_custom_actions" || !bindValue.is_object()) continue;
        size_t delimiter = jsonKey.find("::");
        if (delimiter == std::string::npos) continue;

        if (bindValue.contains("key") && bindValue.contains("modifiers")) {
            auto key = static_cast<cocos2d::enumKeyCodes>(bindValue["key"].get<int>());
            auto modifiers = static_cast<geode::KeyboardModifier>(bindValue["modifiers"].get<int>());
            
            std::string windowName = jsonKey.substr(0, delimiter);
            std::string hackName = jsonKey.substr(delimiter + 2);
            Gui::get().getWindow(windowName).findHackByName(hackName).setKeybind(geode::Keybind(key, modifiers));
        }
    }
}

void Keybinds::save() const {
    std::ofstream file(keybindsDataPath);
    if (!file.is_open()) return;

    nlohmann::json jsonObject = nlohmann::json::object();
    
    for (auto& window : Gui::get().getWindows()) {
        for (auto& hack : window.getHacks()) {
            auto bind = hack.getKeybind();
            if (bind.key != cocos2d::KEY_None) {
                nlohmann::json bindJson;
                bindJson["key"] = static_cast<int>(bind.key);
                bindJson["modifiers"] = static_cast<int>(bind.modifiers);
                jsonObject[fmt::format("{}::{}", window.getName(), hack.getName())] = bindJson;
            }
        }
    }

    nlohmann::json customJson = nlohmann::json::object();
    for (const auto& [id, customPair] : m_customBinds) {
        auto bind = customPair.first;
        if (bind.key != cocos2d::KEY_None) {
            nlohmann::json bindJson;
            bindJson["key"] = static_cast<int>(bind.key);
            bindJson["modifiers"] = static_cast<int>(bind.modifiers);
            customJson[id] = bindJson;
        }
    }
    jsonObject["_custom_actions"] = customJson;

    file << jsonObject.dump(4);
}

void Keybinds::startRecording(const std::string& windowName, const std::string& hackName) { 
    stopRecording();
    m_recordingWindow = windowName;
    m_recordingHack = hackName;
}

void Keybinds::startRecordingCustom(const std::string& id) { 
    stopRecording();
    m_recordingCustomId = id;
}

bool Keybinds::isRecording(const std::string& windowName, const std::string& hackName) const { 
    return m_recordingWindow == windowName && m_recordingHack == hackName;
}

bool Keybinds::isRecordingCustom(const std::string& id) const { 
    return m_recordingCustomId == id;
}

void Keybinds::stopRecording() { 
    m_recordingWindow = "";
    m_recordingHack = "";
    m_recordingCustomId = "";
}

bool Keybinds::isKeyDown(cocos2d::enumKeyCodes key) const { 
    return m_pressedKeys.contains(key);
}

bool Keybinds::isMouseButtonDown(geode::MouseInputData::Button button) const { 
    return m_pressedMouseButtons.contains(button);
}

void Keybinds::addCallback(const std::string& id, geode::Keybind defaultBind, std::function<void()> callback) {
    if (m_customBinds.contains(id)) {
        m_customBinds[id].second = callback;
    } else {
        m_customBinds[id] = {defaultBind, callback};
    }
}

void Keybinds::changeBind(const std::string& id, geode::Keybind newBind) {
    m_customBinds[id].first = newBind;
}

geode::Keybind Keybinds::getBind(const std::string& id) const {
    if (m_customBinds.contains(id)) {
        return m_customBinds.at(id).first;
    }
    return geode::Keybind();
}

void Keybinds::removeCallback(const std::string& id) {
    if (m_customBinds.contains(id)) {
        m_customBinds.erase(id);
    }
}