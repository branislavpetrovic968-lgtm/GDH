#ifdef GEODE_IS_WINDOWS
#include "layout.hpp"
#include "theme.hpp"
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
    if (m_stage != Stage::Collecting) return;

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
            ImGui::SetNextWindowPos(ImVec2(info->x, info->y), ImGuiCond_Always);

            // imgui so badly calculating height of window
            // so i use a hardcoded increment to prevent unnecessary scrollbars
            ImGui::SetNextWindowSize(ImVec2(info->w, info->h + 3.f), ImGuiCond_Always);
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
    if (m_layout.empty()) return;

    if (!m_isBaseCollected && !m_windows.empty()) {
        m_baseWindows = m_windows;
        m_isBaseCollected = true;
    }

    if (m_isBaseCollected) {
        m_windows = m_baseWindows;
    }

    float total_width = PADDING_X;
    float max_layout_height = 0.0f;

    for (const auto& column : m_layout) {
        float col_width = getMaxWidthInColumn(column);
        float col_height = getColumnHeight(column) + (PADDING_Y * 2);
        total_width += col_width + PADDING_X;
        if (col_height > max_layout_height) max_layout_height = col_height;
    }

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float scale_x = (display.x > 0) ? (display.x / total_width) : 1.0f;
    float scale_y = (display.y > 0) ? (display.y / max_layout_height) : 1.0f;
    
    m_scale = std::min({scale_x, scale_y, 1.0f});
    ApplyStyle(m_scale);

    float current_x = PADDING_X * m_scale;
    
    for (const auto& column : m_layout) {
        float max_width = getMaxWidthInColumn(column);
        float current_y = PADDING_Y * m_scale;
        float scaled_max_width = max_width * m_scale;
        
        for (const auto& name : column) {
            WindowInfo* window = findWindow(name);
            if (window) {
                window->x = current_x;
                window->y = current_y;

                window->w = scaled_max_width;
                window->h = window->h * m_scale;
                
                current_y += window->h + SPACING_Y * m_scale;
            }
        }
        
        current_x += scaled_max_width + PADDING_X * m_scale;
    }
    
    m_stage = Stage::Applying;
}
#endif