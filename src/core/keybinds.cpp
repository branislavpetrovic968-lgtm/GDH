#include "keybinds.hpp"
#include "gui.hpp"
#include "config.hpp"

using namespace geode::prelude;
using namespace GDH;

void Keybinds::init() {
    load();
    rebuildCache();

    auto listener = geode::KeyboardInputEvent().listen([this](geode::KeyboardInputData& evt) {
        if (evt.action == geode::KeyboardInputData::Action::Press) {
            
            if (m_isRecordingMode) {
                cocos2d::enumKeyCodes key = evt.key;

                if (key == cocos2d::KEY_LeftControl || key == cocos2d::KEY_RightControl ||
                    key == cocos2d::KEY_LeftShift || key == cocos2d::KEY_RightShift ||
                    key == cocos2d::KEY_Alt) {
                    return ListenerResult::Stop;
                }

                auto& hack = Gui::get().getWindow(m_recordingWindow).findHackByName(m_recordingHack);

                if (key == cocos2d::KEY_Escape) {
                    hack.setKeybind(geode::Keybind(cocos2d::KEY_None, geode::KeyboardModifier::None));
                } else {
                    hack.setKeybind(geode::Keybind(key, evt.modifiers));
                }

                stopRecording();
                return ListenerResult::Stop;
            }
            else if (!m_isKeybindsMode) {
                geode::Keybind pressedBind(evt.key, evt.modifiers);
                if (m_bindsMap.count(pressedBind)) {
                    auto& hacksList = m_bindsMap[pressedBind];
                    
                    for (const auto& [windowName, hackName] : hacksList) {
                        Gui::get().getWindow(windowName).findHackByName(hackName).toggle();
                    }
                    
                    return ListenerResult::Stop;
                }
            }

        }
        return ListenerResult::Propagate;
    });

    listener.leak();
}

void Keybinds::rebuildCache() {
    m_bindsMap.clear();
    for (auto& window : Gui::get().getWindows()) {
        for (auto& hack : window.getHacks()) {
            geode::Keybind bind = hack.getKeybind();
            if (bind.key != cocos2d::enumKeyCodes::KEY_None) {
                m_bindsMap[bind].push_back(std::make_pair(window.getName(), hack.getName()));
            }
        }
    }
}

void Keybinds::load() {
    std::ifstream inFile(keybindsDataPath);
    if (inFile.is_open()) {
        std::string file_contents((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        nlohmann::json j = nlohmann::json::parse(file_contents, nullptr, false);
        if (j.is_discarded()) {
            return;
        }

        for (const auto& [jsonKey, bindValue] : j.items()) {
            if (!bindValue.is_string()) continue;

            size_t delimiter = jsonKey.find("::");
            if (delimiter == std::string::npos) continue;

            std::string windowName = jsonKey.substr(0, delimiter);
            std::string hackName = jsonKey.substr(delimiter + 2);

            auto parseResult = geode::Keybind::fromString(bindValue.get<std::string>());
            if (parseResult.isErr()) {
                continue;
            }

            Gui::get().getWindow(windowName).findHackByName(hackName).setKeybind(parseResult.unwrap());
        }
    }
}

void Keybinds::save() const {
    std::ofstream outFile(keybindsDataPath);
    if (outFile.is_open()) {
        nlohmann::json j = nlohmann::json::object();

        for (auto& window : Gui::get().getWindows()) {
            for (auto& hack : window.getHacks()) {
                geode::Keybind bind = hack.getKeybind();
                if (bind.key != cocos2d::enumKeyCodes::KEY_None) {
                    j[fmt::format("{}::{}", window.getName(), hack.getName())] = bind.toString();
                }
            }
        }

        outFile << j.dump(4);
        outFile.close();
    }
}

void Keybinds::startRecording(const std::string& windowName, const std::string& hackName) {
    m_recordingWindow = windowName;
    m_recordingHack = hackName;
    m_isRecordingMode = true;
}

bool Keybinds::isRecording(const std::string& windowName, const std::string& hackName) const {
    return m_isRecordingMode && (m_recordingWindow == windowName) && (m_recordingHack == hackName);
}

void Keybinds::stopRecording() {
    m_isRecordingMode = false;
    m_recordingWindow = "";
    m_recordingHack = "";
}