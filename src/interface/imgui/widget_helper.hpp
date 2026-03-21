#include <string>

namespace ImGuiWidgetConfig {
    bool Checkbox(const char* label, const std::string& config_key, bool default_value);
    bool HackCheckbox(const char* label, const std::string& window, const std::string& config_key, bool default_value);

    bool InputInt(const char* label, const std::string& config_key, int default_value);
    bool DragFloat(const char* label, const std::string& config_key, float step, float min, float max, float default_value, const char* format);
    bool DragInt(const char* label, const std::string& config_key, float step, int min, int max, int default_value, const char* format);
}