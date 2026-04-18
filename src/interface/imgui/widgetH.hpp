#include <imgui-cocos.hpp>

namespace ImGuiH {
    enum Color {
        Button_Background,
        Button_Background_Hover,
        Button_Background_Active,
        Button_Foreground,
        Button_Foreground_Hover,
        Button_Foreground_Active,
        Button_Border,
        Checkbox_Background_Off,
        Checkbox_Background_Off_Hover,
        Checkbox_Background_Off_Active,
        Checkbox_Background_On,
        Checkbox_Background_On_Hover,
        Checkbox_Background_On_Active,
        Checkbox_Knob_Off,
        Checkbox_Knob_Off_Hover,
        Checkbox_Knob_Off_Active,
        Checkbox_Knob_On,
        Checkbox_Knob_On_Hover,
        Checkbox_Knob_On_Active,
        Checkbox_Border,
        Tooltip_Background,
        Tooltip_Foreground,
        Tooltip_Border,
        Drag_Background,
        Drag_Background_Hover,
        Drag_Background_Active,
        Drag_Middleground,
        Drag_Middleground_Hover,
        Drag_Middleground_Active,
        Drag_Foreground,
        Drag_Foreground_Hover,
        Drag_Foreground_Active,
        Color_Count,
    };

    extern std::map<Color, ImVec4> colors;

    void SetMenuHue(float hue);
    
    bool Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
    bool Checkbox(const char* label, bool* v);
    void Tooltip(const char* text, bool hovered = true);
    
    bool DragFloat(const char* label, float* v, float speed = 1.f, float v_min = 0.f,
        float v_max = 0.f, const char* fmt = "%.2f");

    bool DragInt(const char* label, int* v, float speed = 1.f, int v_min = 0,
        int v_max = 0, const char* fmt = "%d");

    bool ArrowButton(const char* str_id, ImGuiDir dir);
    void AddRadialGradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);
    void GlowWindow();
}
