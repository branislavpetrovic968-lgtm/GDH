#include "utils.hpp"
#include <algorithm>
#include <cctype>

std::string GDH::Utils::String::toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string GDH::Utils::String::toUpperCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string GDH::Utils::String::replaceChar(const std::string& input, char from, char to) {
    std::string result = input;
    std::replace(result.begin(), result.end(), from, to);
    return result;
}

std::string GDH::Utils::formatTime(double sec, bool ms = false) {
    int h = sec / 3600, m = (int(sec) % 3600) / 60, s = int(sec) % 60;
    int cs = (sec - int(sec)) * 100;
    
    auto base = h > 0 ? fmt::format("{:02d}:{:02d}:{:02d}", h, m, s)
                      : fmt::format("{:02d}:{:02d}", m, s);
    return ms ? fmt::format("{}.{:02d}", base, cs) : base;
}

void GDH::Utils::updateCursorState(bool show) {
    bool canShowInLevel = true;
    if (auto* playLayer = PlayLayer::get()) {
        canShowInLevel = playLayer->m_hasCompletedLevel ||
                         playLayer->m_isPaused ||
                         GameManager::sharedState()->getGameVariable("0024");
    }
    if (show || canShowInLevel)
        PlatformToolbox::showCursor();
    else
        PlatformToolbox::hideCursor();
}