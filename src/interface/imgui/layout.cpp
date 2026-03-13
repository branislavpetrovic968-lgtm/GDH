#include "layout.hpp"
#include <imgui.h>

using namespace GDH;

void Layout::Manager::setLayout(const std::vector<std::vector<std::string>>& layout) {
    m_layout = layout;
}

void Layout::Manager::setFixedWindowSizeInfo(const std::vector<WindowInfo>& fixedWindows) {
    m_fixedWindows = fixedWindows;
}

void Layout::Manager::startCollecting() {
    m_windows.clear();
    m_stage = Stage::Collecting;
}

void Layout::Manager::addWindowInfo(const std::string& name, float width, float height) {
    if (WindowInfo* window = findFixedWindow(name)) {
        if (window->w != 0.f) width = window->w;
        if (window->h != 0.f) height = window->h;
    }

    m_windows.push_back({
        name,
        width, height,
        0.f, 0.f
    });
}

void Layout::Manager::finishCollecting() {
    if (m_stage == Stage::Collecting) {
        m_stage = Stage::Calculating;
        calculateWindowPositions();
    }
}

bool Layout::Manager::applyWindowTransform(const std::string& name) {
    if (m_stage == Stage::Applying) {
        WindowInfo* info = getWindowInfo(name);
        if (info) {
            ImGui::SetNextWindowPos(ImVec2(info->x, info->y));
            ImGui::SetNextWindowSize(ImVec2(info->w, info->h));
            return true;
        }
    }
    return false;
}

bool Layout::Manager::isCollecting() {
    return m_stage == Stage::Collecting;
}

void Layout::Manager::startApplying() {
    m_stage = Stage::Applying;
}

bool Layout::Manager::isApplying() {
    return m_stage == Stage::Applying;
}

void Layout::Manager::finishApplying() {
    if (m_stage == Stage::Applying) {
        reset();
    }
}

Layout::WindowInfo* Layout::Manager::getWindowInfo(const std::string& name) {
    return findWindow(name);
}

void Layout::Manager::reset() {
    // m_windows.clear();
    m_stage = Stage::Idle;
}

Layout::WindowInfo* Layout::Manager::findWindow(const std::string& name) {
    for (auto& window : m_windows) {
        if (window.window_name == name) {
            return &window;
        }
    }
    return nullptr;
}

Layout::WindowInfo* Layout::Manager::findFixedWindow(const std::string& name) {
    for (auto& window : m_fixedWindows) {
        if (window.window_name == name) {
            return &window;
        }
    }
    return nullptr;
}

float Layout::Manager::getMaxWidthInColumn(const std::vector<std::string>& column) {
    float max_width = 0.0f;
    for (const auto& name : column) {
        WindowInfo* window = findWindow(name);
        if (window && window->w > max_width) {
            max_width = window->w;
        }
    }
    return max_width;
}

float Layout::Manager::getColumnHeight(const std::vector<std::string>& column) {
    float total_height = 0.0f;
    for (size_t i = 0; i < column.size(); i++) {
        WindowInfo* window = findWindow(column[i]);
        if (window) {
            total_height += window->h;
            if (i < column.size() - 1) {
                total_height += SPACING_Y;
            }
        }
    }
    return total_height;
}

void Layout::Manager::calculateWindowPositions() {
    float current_x = PADDING_X;
    
    for (const auto& column : m_layout) {
        float max_width = getMaxWidthInColumn(column);
        float current_y = PADDING_Y;
        
        for (const auto& name : column) {
            WindowInfo* window = findWindow(name);
            if (window) {
                window->x = current_x;
                window->y = current_y;
                window->w = max_width;
                current_y += window->h + SPACING_Y;
            }
        }
        
        current_x += max_width + PADDING_X;
    }
    
    m_stage = Stage::Applying;
}