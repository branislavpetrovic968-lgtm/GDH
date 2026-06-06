#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "widgetH.hpp"
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "layout.hpp"

#define COLOR_F32(x) ImVec4 { (((x) >> 24) & 0xFF) / 255.0f, (((x) >> 16) & 0xFF) / 255.0f, (((x) >> 8) & 0xFF) / 255.0f, ((x) & 0xFF) / 255.0f }

namespace ImGuiH {
    std::map<Color, ImVec4> colorTable = {
        {Color::Button_Background,              COLOR_F32(0xD5C4FFFF)},
        {Color::Button_Background_Hover,        COLOR_F32(0xE4D7FFFF)},
        {Color::Button_Background_Active,       COLOR_F32(0xB9A5F0FF)},
        {Color::Button_Foreground,              COLOR_F32(0x1A1A49FF)},
        {Color::Button_Foreground_Hover,        COLOR_F32(0x24245AFF)},
        {Color::Button_Foreground_Active,       COLOR_F32(0x121237FF)},
        {Color::Button_Border,                  COLOR_F32(0x00000000)},

        {Color::Checkbox_Background_Off,        COLOR_F32(0x474783FF)},
        {Color::Checkbox_Background_Off_Hover,  COLOR_F32(0x6868BFFF)},
        {Color::Checkbox_Background_Off_Active, COLOR_F32(0x37376EFF)},
        {Color::Checkbox_Background_On,         COLOR_F32(0xD5C4FFFF)},
        {Color::Checkbox_Background_On_Hover,   COLOR_F32(0xE4D7FFFF)},
        {Color::Checkbox_Background_On_Active,  COLOR_F32(0xB9A5F0FF)},
        {Color::Checkbox_Knob_Off,              COLOR_F32(0xC7D4FAFF)},
        {Color::Checkbox_Knob_Off_Hover,        COLOR_F32(0xE0EBFFFF)},
        {Color::Checkbox_Knob_Off_Active,       COLOR_F32(0x949ED2FF)},
        {Color::Checkbox_Knob_On,               COLOR_F32(0x21214EFF)},
        {Color::Checkbox_Knob_On_Hover,         COLOR_F32(0x2E2E63FF)},
        {Color::Checkbox_Knob_On_Active,        COLOR_F32(0x16163AFF)},
        {Color::Checkbox_Border,                COLOR_F32(0x00000000)},

        {Color::RadioButton_Ring_Off,          COLOR_F32(0x8585BFFF)},
        {Color::RadioButton_Ring_Off_Hover,    COLOR_F32(0xA3A3D4FF)},
        {Color::RadioButton_Ring_Off_Active,   COLOR_F32(0x6464A0FF)},
        {Color::RadioButton_Ring_On,           COLOR_F32(0xD5C4FFFF)},
        {Color::RadioButton_Ring_On_Hover,     COLOR_F32(0xE4D7FFFF)},
        {Color::RadioButton_Ring_On_Active,    COLOR_F32(0xB9A5F0FF)},
        {Color::RadioButton_Dot_Off,           COLOR_F32(0x00000000)},
        {Color::RadioButton_Dot_On,            COLOR_F32(0xD5C4FFFF)},
        {Color::RadioButton_Dot_On_Hover,      COLOR_F32(0xE4D7FFFF)},
        {Color::RadioButton_Dot_On_Active,     COLOR_F32(0xB9A5F0FF)},
        {Color::RadioButton_Layer_Off_Hover,   COLOR_F32(0xC7D4FA14)},
        {Color::RadioButton_Layer_Off_Active,  COLOR_F32(0xC7D4FA1F)},
        {Color::RadioButton_Layer_On_Hover,    COLOR_F32(0xD5C4FF14)},
        {Color::RadioButton_Layer_On_Active,   COLOR_F32(0xD5C4FF1F)},

        {Color::Tooltip_Background,             COLOR_F32(0x21214EFF)},
        {Color::Tooltip_Foreground,             COLOR_F32(0xD5C4FFFF)},
        {Color::Tooltip_Border,                 COLOR_F32(0xD5C4FFFF)},
        {Color::Drag_Background,                COLOR_F32(0x474783FF)},
        {Color::Drag_Background_Hover,          COLOR_F32(0x6868BFFF)},
        {Color::Drag_Background_Active,         COLOR_F32(0xD5C4FFFF)},
        {Color::Drag_Middleground,              COLOR_F32(0x635AB4FF)},
        {Color::Drag_Middleground_Hover,        COLOR_F32(0x8578DCFF)},
        {Color::Drag_Middleground_Active,       COLOR_F32(0xB9A5F0FF)},
        {Color::Drag_Foreground,                COLOR_F32(0xC7D4FAFF)},
        {Color::Drag_Foreground_Hover,          COLOR_F32(0xE0EBFFFF)},
        {Color::Drag_Foreground_Active,         COLOR_F32(0x1A1A49FF)},
    };

    static inline void SetColorHSV(Color cid, ImVec4 hsv) {
        float r, g, b; ImGui::ColorConvertHSVtoRGB(hsv.x, hsv.y, hsv.z, r, g, b);
        colorTable[cid] = ImVec4 { r, g, b, hsv.w };
    }

    static inline void SetImGuiColorHSV(ImGuiCol cid, ImVec4 hsv) {
        auto *colors = ImGui::GetStyle().Colors;
        float r, g, b; ImGui::ColorConvertHSVtoRGB(hsv.x, hsv.y, hsv.z, r, g, b);
        colors[cid] = ImVec4 { r, g, b, hsv.w };
    }
    
    static inline ImU32 ToU32(ImVec4 c, float global_alpha)
    {
        c.w *= global_alpha;
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    static inline ImU32 ToU32A(ImVec4 c, float global_alpha, float anim_alpha)
    {
        c.w *= global_alpha * anim_alpha;
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    static inline ImVec4 LerpC(ImVec4 a, ImVec4 b, float t)
    {
        return { a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t };
    }

    bool Button(const char* label, const ImVec2& size_arg)
    {
        struct ButtonState { float th, tHeld; };
        static std::unordered_map<ImGuiID, ButtonState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io = g.IO;
        const float alpha = style.Alpha;

        const ImGuiID id = window->GetID(label);

        const char* lend = ImGui::FindRenderedTextEnd(label);
        const ImVec2 label_sz = ImGui::CalcTextSize(label, lend, false);
        const ImVec2 pad = style.FramePadding;
        const ImVec2 total_sz = ImGui::CalcItemSize(size_arg, label_sz.x + pad.x * 2.f, label_sz.y + pad.y * 2.f);

        const ImVec2 pos = window->DC.CursorPos;
        const ImRect bb = { pos, pos + total_sz };
        ImGui::ItemSize(total_sz, pad.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        auto& st = s_states.emplace(id, ButtonState{ 0.f, 0.f }).first->second;

        bool hovered, held;
        const bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        if (pressed) ImGui::MarkItemEdited(id);

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt; else ImGui::MarkItemEdited(id);
        };
        Lerp(st.th, (hovered || held) ? 1.f : 0.f,  9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f, held ? 10.f : 6.f);

        auto Smootherstep = [](float t) -> float {
            t = ImClamp(t, 0.f, 1.f);
            return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
        };

        const float easedHeld = Smootherstep(st.tHeld);
        const float rnd = style.FrameRounding * (1.f - easedHeld * 0.8f);

        const ImVec4 col_bg_idle = colorTable[Color::Button_Background];
        const ImVec4 col_bg_hov = colorTable[Color::Button_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Color::Button_Background_Active];
        const ImVec4 col_txt_idle = colorTable[Color::Button_Foreground];
        const ImVec4 col_txt_hov = colorTable[Color::Button_Foreground_Hover];
        const ImVec4 col_txt_held = colorTable[Color::Button_Foreground_Active];
        const ImVec4 col_border = colorTable[Color::Button_Border];

        const ImVec4 bg_final = LerpC(LerpC(col_bg_idle,  col_bg_hov,  st.th), col_bg_held,  easedHeld);
        const ImVec4 txt_final = LerpC(LerpC(col_txt_idle, col_txt_hov, st.th), col_txt_held, easedHeld);

        ImDrawList* dl = window->DrawList;

        dl->AddRectFilled(
            { bb.Min.x + 1.f, bb.Min.y + 2.f },
            { bb.Max.x + 1.f, bb.Max.y + 2.f },
            ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.14f * alpha * (1.f - easedHeld) }), rnd
        );

        dl->AddRectFilled(bb.Min, bb.Max, ToU32(bg_final, alpha), rnd);

        if (col_border.w > 0.f)
            dl->AddRect(bb.Min, bb.Max, ToU32(col_border, alpha),
                        rnd, 0, style.FrameBorderSize > 0.f ? style.FrameBorderSize : 1.f);

        const ImVec2 text_pos = {
            bb.Min.x + (total_sz.x - label_sz.x) * 0.5f,
            bb.Min.y + (total_sz.y - label_sz.y) * 0.5f
        };
        dl->AddText(text_pos, ToU32(txt_final, alpha), label, lend);

        return pressed;
    }

    bool Checkbox(const char* label, bool* v)
    {
        struct ToggleState { float t, th, tHeld; };
        static std::unordered_map<ImGuiID, ToggleState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io = g.IO;
        const float alpha = style.Alpha;

        const float H = ImGui::GetFrameHeight();
        const float W = H * 1.8f;
        const float KR = H * 0.5f - 2.5f;

        const char* lend = ImGui::FindRenderedTextEnd(label);
        const ImVec2 label_sz = ImGui::CalcTextSize(label, lend, true);
        const ImVec2 total_sz = { W + style.ItemInnerSpacing.x + label_sz.x, H };

        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb = { pos, pos + total_sz };
        const ImRect pill_bb = { pos, { pos.x + W, pos.y + H } };
        ImGui::ItemSize(total_sz, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, ImGui::GetID(label))) return false;

        const ImGuiID id = ImGui::GetID(label);
        auto& st = s_states.emplace(id, ToggleState{ *v ? 1.f : 0.f, 0.f, 0.f }).first->second;

        bool hovered, held;
        const bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        bool changed = false;
        if (pressed) { *v = !*v; changed = true; ImGui::MarkItemEdited(id); }

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt; else ImGui::MarkItemEdited(id);
        };
        Lerp(st.t, *v ? 1.f : 0.f, 14.f);
        Lerp(st.th, (hovered || held) ? 1.f : 0.f,  9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f, 18.f);

        const ImVec4 col_bg_off = colorTable[Color::Checkbox_Background_Off];
        const ImVec4 col_bg_off_hov = colorTable[Color::Checkbox_Background_Off_Hover];
        const ImVec4 col_bg_off_held = colorTable[Color::Checkbox_Background_Off_Active];
        const ImVec4 col_bg_on = colorTable[Color::Checkbox_Background_On];
        const ImVec4 col_bg_on_hov = colorTable[Color::Checkbox_Background_On_Hover];
        const ImVec4 col_bg_on_held = colorTable[Color::Checkbox_Background_On_Active];
        const ImVec4 col_knob_off = colorTable[Color::Checkbox_Knob_Off];
        const ImVec4 col_knob_off_hov = colorTable[Color::Checkbox_Knob_Off_Hover];
        const ImVec4 col_knob_off_held = colorTable[Color::Checkbox_Knob_Off_Active];
        const ImVec4 col_knob_on = colorTable[Color::Checkbox_Knob_On];
        const ImVec4 col_knob_on_hov = colorTable[Color::Checkbox_Knob_On_Hover];
        const ImVec4 col_knob_on_held = colorTable[Color::Checkbox_Knob_On_Active];
        const ImVec4 col_border = colorTable[Color::Checkbox_Border];

        const ImVec4 bg_base = LerpC(col_bg_off, col_bg_on, st.t);
        const ImVec4 bg_hov = LerpC(col_bg_off_hov, col_bg_on_hov, st.t);
        const ImVec4 bg_held = LerpC(col_bg_off_held, col_bg_on_held, st.t);
        const ImVec4 bg_final = LerpC(LerpC(bg_base, bg_hov, st.th), bg_held, st.tHeld);

        const ImVec4 kn_base = LerpC(col_knob_off, col_knob_on, st.t);
        const ImVec4 kn_hov = LerpC(col_knob_off_hov, col_knob_on_hov, st.t);
        const ImVec4 kn_held = LerpC(col_knob_off_held, col_knob_on_held, st.t);
        const ImVec4 kn_final = LerpC(LerpC(kn_base, kn_hov, st.th), kn_held, st.tHeld);

        ImDrawList* dl  = window->DrawList;
        const float rnd = H * 0.5f;

        dl->AddRectFilled(pill_bb.Min, pill_bb.Max, ToU32(bg_final, alpha), rnd);

        if (col_border.w > 0.f)
            dl->AddRect(pill_bb.Min, pill_bb.Max, ToU32(col_border, alpha),
                        rnd, 0, style.FrameBorderSize > 0.f ? style.FrameBorderSize : 1.f);

        const float kr_final = KR - st.tHeld * 1.2f;
        const float kx = pos.x + KR + 2.5f + (W - H) * st.t;
        const float ky = pos.y + H * 0.5f;

        dl->AddCircleFilled({ kx, ky + 0.8f }, kr_final + 1.f,
                            ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.18f * alpha }), 32);
        dl->AddCircleFilled({ kx, ky }, kr_final, ToU32(kn_final, alpha), 32);
        dl->AddCircle      ({ kx, ky }, kr_final,
                            ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.08f * alpha }), 32, 1.f);

        if (lend > label)
            dl->AddText({ pos.x + W + style.ItemInnerSpacing.x, pos.y + style.FramePadding.y },
                        ImGui::GetColorU32(ImGuiCol_Text), label, lend);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return changed;
    }

    bool RadioButton(const char* label, bool active)
    {
        struct RadioState { float t, th, tHeld; };
        static std::unordered_map<ImGuiID, RadioState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g     = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO&    io    = g.IO;
        const float alpha = style.Alpha;

        const float H  = ImGui::GetFrameHeight();
        const float OR = H * 0.5f;
        const float BW = 2.f;
        const float IR = OR * 0.50f;

        const float SR = OR * 1.35f;

        const char* lend      = ImGui::FindRenderedTextEnd(label);
        const ImVec2 label_sz = ImGui::CalcTextSize(label, lend, true);
        const ImVec2 total_sz = { H + style.ItemInnerSpacing.x + label_sz.x, H };
        const ImVec2 pos      = window->DC.CursorPos;
        const ImRect total_bb = { pos, pos + total_sz };
        const ImRect radio_bb = { pos, { pos.x + H, pos.y + H } };
        const ImVec2 center   = { pos.x + OR, pos.y + OR };

        ImGui::ItemSize(total_sz, style.FramePadding.y);
        const ImGuiID id = ImGui::GetID(label);
        if (!ImGui::ItemAdd(total_bb, id)) return false;

        auto& st = s_states.emplace(id, RadioState{ active ? 1.f : 0.f, 0.f, 0.f })
                            .first->second;

        bool hovered, held;
        const bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt;
            else ImGui::MarkItemEdited(id);
        };

        Lerp(st.t,     active ? 1.f : 0.f,             14.f);
        Lerp(st.th,    (hovered || held) ? 1.f : 0.f,   9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f,               18.f);

        const ImVec4 col_ring_off      = colorTable[Color::RadioButton_Ring_Off];
        const ImVec4 col_ring_off_hov  = colorTable[Color::RadioButton_Ring_Off_Hover];
        const ImVec4 col_ring_off_held = colorTable[Color::RadioButton_Ring_Off_Active];
        const ImVec4 col_ring_on       = colorTable[Color::RadioButton_Ring_On];
        const ImVec4 col_ring_on_hov   = colorTable[Color::RadioButton_Ring_On_Hover];
        const ImVec4 col_ring_on_held  = colorTable[Color::RadioButton_Ring_On_Active];

        const ImVec4 col_dot_off       = colorTable[Color::RadioButton_Dot_Off];
        const ImVec4 col_dot_on        = colorTable[Color::RadioButton_Dot_On];
        const ImVec4 col_dot_on_hov    = colorTable[Color::RadioButton_Dot_On_Hover];
        const ImVec4 col_dot_on_held   = colorTable[Color::RadioButton_Dot_On_Active];

        const ImVec4 col_layer_off_hov  = colorTable[Color::RadioButton_Layer_Off_Hover];
        const ImVec4 col_layer_off_held = colorTable[Color::RadioButton_Layer_Off_Active];
        const ImVec4 col_layer_on_hov   = colorTable[Color::RadioButton_Layer_On_Hover];
        const ImVec4 col_layer_on_held  = colorTable[Color::RadioButton_Layer_On_Active];

        const ImVec4 ring_base  = LerpC(col_ring_off,     col_ring_on,     st.t);
        const ImVec4 ring_hov   = LerpC(col_ring_off_hov,  col_ring_on_hov,  st.t);
        const ImVec4 ring_held  = LerpC(col_ring_off_held, col_ring_on_held, st.t);
        const ImVec4 ring_final = LerpC(LerpC(ring_base, ring_hov, st.th), ring_held, st.tHeld);

        const ImVec4 dot_base  = LerpC(col_dot_off, col_dot_on,     st.t);
        const ImVec4 dot_hov   = LerpC(col_dot_off, col_dot_on_hov,  st.t);
        const ImVec4 dot_held  = LerpC(col_dot_off, col_dot_on_held, st.t);
        const ImVec4 dot_final = LerpC(LerpC(dot_base, dot_hov, st.th), dot_held, st.tHeld);

        const ImVec4 lay_hov  = LerpC(col_layer_off_hov,  col_layer_on_hov,  st.t);
        const ImVec4 lay_held = LerpC(col_layer_off_held, col_layer_on_held, st.t);

        ImVec4 layer_final = LerpC(lay_hov, lay_held, st.tHeld);
        layer_final.w *= (st.th + st.tHeld * 0.5f);

        ImDrawList* dl = window->DrawList;

        if (layer_final.w > 0.002f)
            dl->AddCircleFilled(center, SR, ToU32(layer_final, alpha), 48);

        const float ring_w = BW + st.t * 0.4f;
        dl->AddCircle(center, OR - ring_w * 0.5f,
                    ToU32(ring_final, alpha), 48, ring_w);

        const float dot_r = IR * st.t * (1.f - st.tHeld * 0.12f);
        if (dot_r > 0.5f)
        {
            dl->AddCircleFilled({ center.x, center.y + 0.8f }, dot_r + 1.f,
                                ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.15f * alpha * st.t }),
                                32);
            dl->AddCircleFilled(center, dot_r, ToU32(dot_final, alpha), 48);
            dl->AddCircle(center, dot_r,
                        ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.06f * alpha }), 48, 1.f);
        }

        if (lend > label)
            dl->AddText({ pos.x + H + style.ItemInnerSpacing.x, pos.y + style.FramePadding.y },
                        ImGui::GetColorU32(ImGuiCol_Text), label, lend);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return pressed;
    }

    void Tooltip(const char* text, bool hovered)
    {
        struct TooltipState { float t, delay; ImVec2 frozen_pos; };
        static std::unordered_map<std::string, TooltipState> s_states;

        ImGuiContext& g = *GImGui;
        const ImGuiIO& io = g.IO;
        const float alpha = g.Style.Alpha;

        constexpr float SPD_IN = 22.f;
        constexpr float SPD_OUT = 14.f;
        constexpr float DELAY_SEC = 0.45f;

        auto& st = s_states.emplace(text, TooltipState{ 0.f, 0.f, {0.f, 0.f} }).first->second;

        if (hovered)
        {
            st.delay += io.DeltaTime;
            if (st.delay < DELAY_SEC)
            {
                if (st.t > 0.f) {
                    st.t -= st.t * ImMin(1.f, io.DeltaTime * SPD_OUT);
                    if (st.t < 0.003f) st.t = 0.f;
                }
                ImGui::GetForegroundDrawList();
                return;
            }
            st.t += (1.f - st.t) * ImMin(1.f, io.DeltaTime * SPD_IN);
            if (1.f - st.t < 0.003f) st.t = 1.f;
        }
        else
        {
            st.delay = 0.f;
            st.t -= st.t * ImMin(1.f, io.DeltaTime * SPD_OUT);
            if (st.t < 0.003f) st.t = 0.f;
        }

        if (st.t > 0.f && st.t < 1.f) ImGui::GetForegroundDrawList();
        if (st.t <= 0.001f) return;

        const ImGuiStyle& style = g.Style;

        const ImVec4 col_bg = colorTable[Color::Tooltip_Background];
        const ImVec4 col_border = colorTable[Color::Tooltip_Border];
        const ImVec4 col_text = colorTable[Color::Tooltip_Foreground];

        const char* lend = ImGui::FindRenderedTextEnd(text);
        const ImVec2 text_sz = ImGui::CalcTextSize(text, lend, false);
        const ImVec2 pad = style.WindowPadding;
        const ImVec2 box_sz = { text_sz.x + pad.x * 2.f, text_sz.y + pad.y * 2.f };
        const ImVec2 disp = io.DisplaySize;
        const ImVec2 off = { 16.f, 12.f };

        if (hovered)
        {
            constexpr float M = 6.f;
            ImVec2 p = { io.MousePos.x + off.x, io.MousePos.y + off.y };
            if (p.x + box_sz.x > disp.x - M) p.x = io.MousePos.x - box_sz.x - off.x;
            if (p.y + box_sz.y > disp.y - M) p.y = io.MousePos.y - box_sz.y - off.y;
            if (p.x < M) p.x = M;
            if (p.y < M) p.y = M;
            st.frozen_pos = p;
        }

        const float a = st.t * alpha;
        const ImRect bb = { st.frozen_pos, st.frozen_pos + box_sz };
        const ImVec2 slide_off = { 0.f, (1.f - st.t) * 6.f };
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const float rnd = style.WindowRounding;

        dl->AddRectFilled(bb.Min + ImVec2(2.f, 3.f) + slide_off,
            bb.Max + ImVec2(2.f, 3.f) + slide_off,
            ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.18f * a }), rnd + 1.f
        );

        dl->AddRectFilled(bb.Min + slide_off, bb.Max + slide_off, ToU32A(col_bg, 1.f, a), rnd);
        dl->AddRect(bb.Min + slide_off, bb.Max + slide_off, ToU32A(col_border, 1.f, a), rnd, 0, 1.f);
        dl->AddText(bb.Min + pad + slide_off, ToU32A(col_text, 1.f, a), text, lend);
    }

    bool DragFloat(const char* label, float* v, float speed, float v_min, float v_max, const char* fmt)
    {
        struct DragState { float th, tHeld; };
        static std::unordered_map<ImGuiID, DragState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io = g.IO;

        const ImGuiID id = window->GetID(label);
        DragState& st = s_states.emplace(id, DragState{ 0.f, 0.f }).first->second;
        
        const ImVec2 pos = window->DC.CursorPos;
        const float  drag_w = ImGui::CalcItemWidth();
        const ImRect drag_bb = { pos, { pos.x + drag_w, pos.y + ImGui::GetFrameHeight() } };

        bool wheel_changed = false;
        if (ImGui::IsMouseHoveringRect(drag_bb.Min, drag_bb.Max) && io.MouseWheel != 0.f)
        {
            *v += io.MouseWheel > 0.f ? speed : -speed;
            if (v_min != v_max) *v = ImClamp(*v, v_min, v_max);
            wheel_changed = true;
            g.IO.MouseWheel = 0.f;
        }

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt;
        };
        Lerp(st.th, st.th, 9.f);
        Lerp(st.tHeld, st.tHeld, 18.f);

        const ImVec4 col_bg_idle = colorTable[Color::Drag_Background];
        const ImVec4 col_bg_hov = colorTable[Color::Drag_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Color::Drag_Background_Active];
        const ImVec4 col_fill_idle = colorTable[Color::Drag_Middleground];
        const ImVec4 col_fill_hov = colorTable[Color::Drag_Middleground_Hover];
        const ImVec4 col_fill_held = colorTable[Color::Drag_Middleground_Active];
        const ImVec4 col_txt_idle = colorTable[Color::Drag_Foreground];
        const ImVec4 col_txt_hov = colorTable[Color::Drag_Foreground_Hover];
        const ImVec4 col_txt_held = colorTable[Color::Drag_Foreground_Active];

        const ImVec4 bg_now = LerpC(LerpC(col_bg_idle, col_bg_hov, st.th), col_bg_held, st.tHeld);
        const ImVec4 fill_now = LerpC(LerpC(col_fill_idle, col_fill_hov, st.th), col_fill_held, st.tHeld);
        const ImVec4 txt_now = LerpC(LerpC(col_txt_idle, col_txt_hov, st.th), col_txt_held, st.tHeld);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_now);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, bg_now);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, bg_now);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, fill_now);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, fill_now);
        ImGui::PushStyleColor(ImGuiCol_Text, txt_now);

        ImGui::PushItemWidth(drag_w);
        const bool native_changed = ImGui::DragFloat(
            fmt::format("##DragWidget_{}", label).c_str(),
            v, speed, v_min, v_max,
            fmt
        );
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(6);

        const char* label_display = label;
        if (label_display[0] == '#' && label_display[1] == '#')
            label_display = "";

        if (label_display[0] != '\0')
        {
            const ImVec2 label_pos = {
                drag_bb.Max.x + style.ItemInnerSpacing.x,
                drag_bb.Min.y + style.FramePadding.y
            };
            ImGui::RenderText(label_pos, label_display);
        }

        const bool is_hov = ImGui::IsItemHovered();
        const bool is_active = ImGui::IsItemActive();
        const bool is_typing = ImGui::TempInputIsActive(id);
        const bool held = is_active && !is_typing;

        Lerp(st.th, (is_hov || is_active) ? 1.f : 0.f, 9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f, 18.f);

        if (st.th != (is_hov || is_active ? 1.f : 0.f) ||
            st.tHeld != (held ? 1.f : 0.f))
            ImGui::MarkItemEdited(id);

        return native_changed || wheel_changed;
    }

    bool DragInt(const char* label, int* v, float speed, int v_min, int v_max, const char* fmt)
    {
        struct DragState { float th, tHeld; };
        static std::unordered_map<ImGuiID, DragState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io = g.IO;
        const ImGuiID id = window->GetID(label);
        DragState& st = s_states.emplace(id, DragState{ 0.f, 0.f }).first->second;

        const ImVec2 pos = window->DC.CursorPos;
        const float  drag_w = ImGui::CalcItemWidth();
        const ImRect drag_bb = { pos, { pos.x + drag_w, pos.y + ImGui::GetFrameHeight() } };

        bool wheel_changed = false;
        if (ImGui::IsMouseHoveringRect(drag_bb.Min, drag_bb.Max) && io.MouseWheel != 0.f)
        {
            *v += io.MouseWheel > 0.f ? static_cast<int>(speed) : -static_cast<int>(speed);
            if (v_min != v_max) *v = ImClamp(*v, v_min, v_max);
            wheel_changed = true;
            g.IO.MouseWheel = 0.f;
        }

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt;
        };

        const ImVec4 col_bg_idle = colorTable[Color::Drag_Background];
        const ImVec4 col_bg_hov = colorTable[Color::Drag_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Color::Drag_Background_Active];
        const ImVec4 col_fill_idle = colorTable[Color::Drag_Middleground];
        const ImVec4 col_fill_hov = colorTable[Color::Drag_Middleground_Hover];
        const ImVec4 col_fill_held = colorTable[Color::Drag_Middleground_Active];
        const ImVec4 col_txt_idle = colorTable[Color::Drag_Foreground];
        const ImVec4 col_txt_hov  = colorTable[Color::Drag_Foreground_Hover];
        const ImVec4 col_txt_held = colorTable[Color::Drag_Foreground_Active];

        const ImVec4 bg_now = LerpC(LerpC(col_bg_idle,   col_bg_hov,   st.th), col_bg_held,   st.tHeld);
        const ImVec4 fill_now = LerpC(LerpC(col_fill_idle, col_fill_hov, st.th), col_fill_held, st.tHeld);
        const ImVec4 txt_now = LerpC(LerpC(col_txt_idle,  col_txt_hov,  st.th), col_txt_held,  st.tHeld);

        ImGui::PushStyleColor(ImGuiCol_FrameBg,bg_now);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, bg_now);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, bg_now);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, fill_now);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, fill_now);
        ImGui::PushStyleColor(ImGuiCol_Text, txt_now);

        ImGui::PushItemWidth(drag_w);
        const bool native_changed = ImGui::DragInt(
            fmt::format("##DragWidget_{}", label).c_str(),
            v, speed, v_min, v_max,
            fmt ? fmt : "%d"
        );
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(6);

        const char* label_display = label;
        if (label_display[0] == '#' && label_display[1] == '#')
            label_display = "";
        if (label_display[0] != '\0')
        {
            const ImVec2 label_pos = {
                drag_bb.Max.x + style.ItemInnerSpacing.x,
                drag_bb.Min.y + style.FramePadding.y
            };
            ImGui::RenderText(label_pos, label_display);
        }

        const bool is_hov  = ImGui::IsItemHovered();
        const bool is_active = ImGui::IsItemActive();
        const bool is_typing = ImGui::TempInputIsActive(id);
        const bool held = is_active && !is_typing;

        Lerp(st.th, (is_hov || is_active) ? 1.f : 0.f, 9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f, 18.f);

        if (st.th != (is_hov || is_active ? 1.f : 0.f) ||
            st.tHeld != (held ? 1.f : 0.f))
            ImGui::MarkItemEdited(id);

        return native_changed || wheel_changed;
    }

    bool ArrowButton(const char* str_id, ImGuiDir dir)
    {
        struct ArrowState { float th, tHeld; };
        static std::unordered_map<ImGuiID, ArrowState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io = g.IO;
        const float alpha = style.Alpha;

        const ImGuiID id = window->GetID(str_id);
        ArrowState& st = s_states.emplace(id, ArrowState{0.f, 0.f}).first->second;

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt; else ImGui::MarkItemEdited(id);
        };

        const ImVec4 col_bg_idle = colorTable[Color::Button_Background];
        const ImVec4 col_bg_hov = colorTable[Color::Button_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Color::Button_Background_Active];

        const ImVec4 col_arr_idle = colorTable[Color::Button_Foreground];
        const ImVec4 col_arr_hov = colorTable[Color::Button_Foreground_Hover];
        const ImVec4 col_arr_held = colorTable[Color::Button_Foreground_Active];

        const ImVec4 bg_now = LerpC(LerpC(col_bg_idle, col_bg_hov, st.th), col_bg_held,  st.tHeld);
        const ImVec4 arr_now = LerpC(LerpC(col_arr_idle, col_arr_hov, st.th), col_arr_held, st.tHeld);

        ImGui::PushStyleColor(ImGuiCol_Button, bg_now);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg_now);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg_now);
        ImGui::PushStyleColor(ImGuiCol_Text, arr_now);

        const bool pressed = ImGui::ArrowButton(str_id, dir);

        ImGui::PopStyleColor(4);

        const bool is_hov = ImGui::IsItemHovered();
        const bool is_active = ImGui::IsItemActive();

        Lerp(st.th, (is_hov || is_active) ? 1.f : 0.f,  9.f);
        Lerp(st.tHeld, is_active ? 1.f : 0.f, 18.f);

        return pressed;
    }

    void AddRadialGradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out)
    {
        if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
            return;

        draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
        const int count = draw_list->_Path.Size - 1;

        unsigned int vtx_base = draw_list->_VtxCurrentIdx;
        draw_list->PrimReserve(count * 3, count + 1);

        const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
        draw_list->PrimWriteVtx(center, uv, col_in);
        for (int n = 0; n < count; n++)
            draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);

        for (int n = 0; n < count; n++)
        {
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
            draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
        }
        draw_list->_Path.Size = 0;
    }

    void GlowWindow(ImU32 col_in, ImU32 col_out) {
        ImVec2 pos  = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        ImVec4 oldClipRect = dl->_ClipRectStack.back(); 

        dl->PushClipRectFullScreen();
        float radius = ImMax(size.x, size.y) * 1.5f;
        ImGuiH::AddRadialGradient(dl, center, radius, col_in, col_out);
        dl->PopClipRect();
    }
    
    struct SS { float y=0, t=0, h=0, wh=0, a=0; };
    static std::unordered_map<ImGuiID, SS> m;
    static std::vector<ImGuiID> st;

    bool BeginSmoothScroll(const char* name, bool* p_open, ImGuiWindowFlags f)
    {
        bool v = ImGui::Begin(name, p_open, f | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGuiID id = ImGui::GetCurrentWindow()->ID;
        st.push_back(id);
        auto& s = m[id];
        if (!v) return false;

        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        bool hov = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

        if (hov && io.MouseWheel != 0.f) {
            s.t -= io.MouseWheel * 40.f;
            io.MouseWheel = 0.f;
        }

        float maxY = ImMax(0.f, s.h - s.wh);
        s.t = ImClamp(s.t, 0.f, maxY);

        float k = ImMin(1.f, dt * 8.f);
        s.y += (s.t - s.y) * k;

        if (ImAbs(s.y - s.t) < 0.2f) s.y = s.t;
        s.y = ImClamp(s.y, 0.f, maxY);

        ImGui::SetScrollY(s.y);

        float tgtA = (maxY > 0 && (hov || ImAbs(s.y - s.t) > 0.5f)) ? 1.f : 0.f;
        s.a += (tgtA - s.a) * ImMin(1.f, dt * 10.f);
        if (s.a < 0.01f) s.a = 0.f;

        return true;
    }

    void EndSmoothScroll()
    {
        if (st.empty()) { ImGui::End(); return; }

        ImGuiID id = st.back();
        st.pop_back();
        auto& s = m[id];

        ImGuiWindow* w = ImGui::GetCurrentWindow();
        s.wh = w->InnerRect.GetHeight();
        s.h = s.wh + w->ScrollMax.y;

        if (s.a > 0.f)
        {
            ImGuiStyle& style = ImGui::GetStyle();

            const float scrollbar_size = style.ScrollbarSize;
            const float rounding = style.ScrollbarRounding;

            const float padding = style.WindowPadding.x * 0.35f;
            const float width = scrollbar_size * 0.55f;

            float maxY = ImMax(1.f, s.h - s.wh);
            float frac = ImClamp(s.y / maxY, 0.f, 1.f);

            float barH = ImMax(
                scrollbar_size,
                (s.wh / ImMax(s.h, s.wh)) * (s.wh - padding * 2.f)
            );

            float track = s.wh - barH - padding * 2.f;

            float x = w->InnerRect.Max.x - width - padding;
            float y = w->InnerRect.Min.y + padding + frac * track;

            ImVec2 a = { x, y };
            ImVec2 b = { x + width, y + barH };

            ImVec4 scrollbar = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
            scrollbar.w *= ImGui::GetStyle().Alpha * s.a;
            
            ImGui::GetWindowDrawList()->AddRectFilled(a, b, ImGui::GetColorU32(scrollbar), rounding);
        }
        ImGui::End();
    }

    static std::vector<PopupMessage> s_popups;

    void AddPopup(const std::string& caption)
    {
        s_popups.insert(s_popups.begin(), {
            caption,
            static_cast<float>(ImGui::GetTime()) + 5.0f,
            0.0f,
            0.0f,
            false
        });
    }

    void RenderPopups()
    {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImVec2 dis = ImGui::GetIO().DisplaySize;
        const float dt = ImGui::GetIO().DeltaTime;
        const float now = static_cast<float>(ImGui::GetTime());
        
        auto& layout = GDH::Layout::Manager::get();

        for (auto& m : s_popups) 
        {
            if (!m.dying && now >= m.expiry_time) m.dying = true;
            if (!m.dying) {
                m.slide_t  += (1.0f - m.slide_t)  * std::min(1.0f, dt * 10.0f);
                m.height_t += (1.0f - m.height_t) * std::min(1.0f, dt * 10.0f);
            } else {
                m.slide_t  += (0.0f - m.slide_t)  * std::min(1.0f, dt * 10.0f);
                m.height_t += (0.0f - m.height_t) * std::min(1.0f, dt * 10.0f);
            }
        }

        s_popups.erase(
            std::remove_if(s_popups.begin(), s_popups.end(),
                [](const PopupMessage& m) { return m.dying && m.slide_t <= 0.01f && m.height_t <= 0.01f; }),
            s_popups.end()
        );

        float curY = dis.y - layout.multipleScale(18.0f);

        const ImVec4 col_bg = colorTable[Color::Tooltip_Background];
        const ImVec4 col_border = colorTable[Color::Tooltip_Border];
        const ImVec4 col_text = colorTable[Color::Tooltip_Foreground];
        
        const float rnd = layout.multipleScale(GImGui->Style.WindowRounding);
        const ImVec2 pad = { layout.multipleScale(GImGui->Style.WindowPadding.x), layout.multipleScale(GImGui->Style.WindowPadding.y) };

        for (const auto& m : s_popups) 
        {
            float sCardW = layout.multipleScale(272.0f);
            
            float textMaxW = sCardW - pad.x * 2.0f;
            ImVec2 textSz = ImGui::CalcTextSize(m.caption.c_str(), nullptr, false, textMaxW);
            
            float totalCardH = pad.y * 2.0f + textSz.y;
            float slotH = totalCardH * m.height_t;

            float xOff = (sCardW + layout.multipleScale(18.0f)) * (1.0f - m.slide_t);
            float x = dis.x - sCardW - layout.multipleScale(18.0f) + xOff;
            
            float y = curY - slotH;

            ImVec2 pMin = { x, y };
            ImVec2 pMax = { x + sCardW, y + slotH };

            ImU32 u_bg = ImGui::ColorConvertFloat4ToU32({ col_bg.x, col_bg.y, col_bg.z, col_bg.w });
            ImU32 u_border = ImGui::ColorConvertFloat4ToU32({ col_border.x, col_border.y, col_border.z, col_border.w });
            ImU32 u_text = ImGui::ColorConvertFloat4ToU32({ col_text.x, col_text.y, col_text.z, col_text.w });
            ImU32 u_shadow = ImGui::ColorConvertFloat4ToU32({ 0.0f, 0.0f, 0.0f, 0.18f });

            dl->AddRectFilled({ pMin.x + 2.0f, pMin.y + 3.0f }, { pMax.x + 2.0f, pMax.y + 3.0f }, u_shadow, rnd + 1.0f);
            dl->AddRectFilled(pMin, pMax, u_bg, rnd);
            dl->AddRect(pMin, pMax, u_border, rnd, 0, 1.0f);
            
            dl->PushClipRect(pMin, pMax, true);
            dl->AddText(ImGui::GetFont(), layout.multipleScale(ImGui::GetFontSize()), { pMin.x + pad.x, pMin.y + pad.y }, u_text, m.caption.c_str(), nullptr, textMaxW);
            dl->PopClipRect();

            curY -= slotH + layout.multipleScale(8.0f);
        }
    }
} // namespace ImGuiH