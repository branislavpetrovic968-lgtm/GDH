#pragma once
#include <Geode/Geode.hpp>

namespace GDH {
    class Keybinds {
    public:
        static Keybinds& get() {
            static Keybinds instance;
            return instance;
        }

        Keybinds& operator=(const Keybinds&) = delete;
        Keybinds(const Keybinds&) = delete;

        void init();
        void rebuildCache();

        void startRecording(const std::string& windowName, const std::string& hackName);
        bool isRecording(const std::string& windowName, const std::string& hackName) const;
        void stopRecording();

        void load();
        void save() const;
        
        bool m_isKeybindsMode = false;
    private:
        Keybinds() = default;

        std::unordered_map<geode::Keybind, std::vector<std::pair<std::string, std::string>>> m_bindsMap;

        bool m_isRecordingMode = false;

        std::string m_recordingWindow = "";
        std::string m_recordingHack = "";
    };
}