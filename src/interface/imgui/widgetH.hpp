#include <imgui-cocos.hpp>

namespace ImGuiH {
    bool Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
    bool Checkbox(const char* label, bool* v);
    void Tooltip(const char* text, bool hovered = true);
    
    bool DragFloat(const char* label, float* v, float speed = 1.f, float v_min = 0.f,
        float v_max = 0.f, const char* fmt = "%.2f");

    bool DragInt(const char* label, int* v, float speed = 1.f, int v_min = 0,
        int v_max = 0, const char* fmt = "%d");
}