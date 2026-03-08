#include "widget_helper.hpp"
#include "../../core/config.hpp"
#include "../../core/gui.hpp"
#include "widgetH.hpp"
#include <fmt/format.h>
#include <imgui-cocos.hpp>

bool ImGuiWidgetConfig::HackCheckbox(const char* label, const std::string& window, const std::string& config_key, bool default_value = false) {
    auto& config = Config::get();
    
    bool value = config.get<bool>(config_key, default_value);
    if (ImGuiH::Checkbox(label, &value))
    {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow(window).findHackByID(config_key);
        if (hack != nullptr) hack->toggle();     
    }
    return false;
}

bool ImGuiWidgetConfig::Checkbox(const char* label, const std::string& config_key, bool default_value = false) {
    auto& config = Config::get();
    bool value = config.get<bool>(config_key, default_value);
    if (ImGuiH::Checkbox(label, &value))
    {
        config.set<bool>(config_key, value);
        return true;
    }
    return false;
}

bool ImGuiWidgetConfig::InputInt(const char* label, const std::string& config_key, int default_value) {
    auto& config = Config::get();
    int value = config.get<int>(config_key, default_value);
    if (ImGui::InputInt(label, &value, 1))
    {
        config.set<int>(config_key, value);
        return true;
    }
    return false;
}

bool ImGuiWidgetConfig::DragFloat(const char* label, const std::string& config_key, float step, float min, float max, float default_value, const char* format)
{
    auto& config = Config::get();
    float value = config.get<float>(config_key, default_value);
    if (ImGuiH::DragFloat(label, &value, step, min, max, format))
    {
        config.set<float>(config_key, value);
        return true;
    }
    return false;
}