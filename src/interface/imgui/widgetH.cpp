#define IMGUI_DEFINE_MATH_OPERATORS
#include "widgetH.hpp"
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#define COLOR_F32(x) ImVec4 { (((x) >> 24) & 0xFF) / 255.0f, (((x) >> 16) & 0xFF) / 255.0f, (((x) >> 8) & 0xFF) / 255.0f, ((x) & 0xFF) / 255.0f }

namespace ImGuiH {
    std::map<Colors, ImVec4> colorTable = {
        {Colors::Button_Background,              COLOR_F32(0xD5C4FFFF)},
        {Colors::Button_Background_Hover,        COLOR_F32(0xE4D7FFFF)},
        {Colors::Button_Background_Active,       COLOR_F32(0xB9A5F0FF)},
        {Colors::Button_Foreground,              COLOR_F32(0x1A1A49FF)},
        {Colors::Button_Foreground_Hover,        COLOR_F32(0x24245AFF)},
        {Colors::Button_Foreground_Active,       COLOR_F32(0x121237FF)},
        {Colors::Button_Border,                  COLOR_F32(0x00000000)},
        {Colors::Checkbox_Background_Off,        COLOR_F32(0x474783FF)},
        {Colors::Checkbox_Background_Off_Hover,  COLOR_F32(0x6868BFFF)},
        {Colors::Checkbox_Background_Off_Active, COLOR_F32(0x37376EFF)},
        {Colors::Checkbox_Background_On,         COLOR_F32(0xD5C4FFFF)},
        {Colors::Checkbox_Background_On_Hover,   COLOR_F32(0xE4D7FFFF)},
        {Colors::Checkbox_Background_On_Active,  COLOR_F32(0xB9A5F0FF)},
        {Colors::Checkbox_Knob_Off,              COLOR_F32(0xC7D4FAFF)},
        {Colors::Checkbox_Knob_Off_Hover,        COLOR_F32(0xE0EBFFFF)},
        {Colors::Checkbox_Knob_Off_Active,       COLOR_F32(0x949ED2FF)},
        {Colors::Checkbox_Knob_On,               COLOR_F32(0x21214EFF)},
        {Colors::Checkbox_Knob_On_Hover,         COLOR_F32(0x2E2E63FF)},
        {Colors::Checkbox_Knob_On_Active,        COLOR_F32(0x16163AFF)},
        {Colors::Checkbox_Border,                COLOR_F32(0x00000000)},
        {Colors::Tooltip_Background,             COLOR_F32(0x21214EFF)},
        {Colors::Tooltip_Foreground,             COLOR_F32(0xD5C4FFFF)},
        {Colors::Tooltip_Border,                 COLOR_F32(0xD5C4FFFF)},
        {Colors::Drag_Background,                COLOR_F32(0x474783FF)},
        {Colors::Drag_Background_Hover,          COLOR_F32(0x6868BFFF)},
        {Colors::Drag_Background_Active,         COLOR_F32(0xD5C4FFFF)},
        {Colors::Drag_Middleground,              COLOR_F32(0x635AB4FF)},
        {Colors::Drag_Middleground_Hover,        COLOR_F32(0x8578DCFF)},
        {Colors::Drag_Middleground_Active,       COLOR_F32(0xB9A5F0FF)},
        {Colors::Drag_Foreground,                COLOR_F32(0xC7D4FAFF)},
        {Colors::Drag_Foreground_Hover,          COLOR_F32(0xE0EBFFFF)},
        {Colors::Drag_Foreground_Active,         COLOR_F32(0x1A1A49FF)},
    };
    
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

        const char*  lend = ImGui::FindRenderedTextEnd(label);
        const ImVec2 label_sz = ImGui::CalcTextSize(label, lend, false);
        const ImVec2 pad  = style.FramePadding;
        const ImVec2 total_sz = ImGui::CalcItemSize(size_arg, label_sz.x + pad.x * 2.f, label_sz.y + pad.y * 2.f);

        const ImVec2 pos = window->DC.CursorPos;
        const ImRect bb  = { pos, pos + total_sz };
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
        Lerp(st.th,    (hovered || held) ? 1.f : 0.f,  9.f);
        Lerp(st.tHeld, held ? 1.f : 0.f,              18.f);

        const ImVec4 col_bg_idle = colorTable[Colors::Button_Background];
        const ImVec4 col_bg_hov = colorTable[Colors::Button_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Colors::Button_Background_Active];
        const ImVec4 col_txt_idle = colorTable[Colors::Button_Foreground];
        const ImVec4 col_txt_hov = colorTable[Colors::Button_Foreground_Hover];
        const ImVec4 col_txt_held = colorTable[Colors::Button_Foreground_Active];
        const ImVec4 col_border = colorTable[Colors::Button_Border];

        const ImVec4 bg_final = LerpC(LerpC(col_bg_idle,  col_bg_hov,  st.th), col_bg_held,  st.tHeld);
        const ImVec4 txt_final = LerpC(LerpC(col_txt_idle, col_txt_hov, st.th), col_txt_held, st.tHeld);

        ImDrawList* dl  = window->DrawList;
        const float rnd = style.FrameRounding;

        dl->AddRectFilled(
            { bb.Min.x + 1.f, bb.Min.y + 2.f },
            { bb.Max.x + 1.f, bb.Max.y + 2.f },
            ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 0.14f * alpha * (1.f - st.tHeld) }), rnd
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

        const ImVec4 col_bg_off = colorTable[Colors::Checkbox_Background_Off];
        const ImVec4 col_bg_off_hov = colorTable[Colors::Checkbox_Background_Off_Hover];
        const ImVec4 col_bg_off_held = colorTable[Colors::Checkbox_Background_Off_Active];
        const ImVec4 col_bg_on = colorTable[Colors::Checkbox_Background_On];
        const ImVec4 col_bg_on_hov = colorTable[Colors::Checkbox_Background_On_Hover];
        const ImVec4 col_bg_on_held = colorTable[Colors::Checkbox_Background_On_Active];
        const ImVec4 col_knob_off = colorTable[Colors::Checkbox_Knob_Off];
        const ImVec4 col_knob_off_hov = colorTable[Colors::Checkbox_Knob_Off_Hover];
        const ImVec4 col_knob_off_held = colorTable[Colors::Checkbox_Knob_Off_Active];
        const ImVec4 col_knob_on = colorTable[Colors::Checkbox_Knob_On];
        const ImVec4 col_knob_on_hov = colorTable[Colors::Checkbox_Knob_On_Hover];
        const ImVec4 col_knob_on_held = colorTable[Colors::Checkbox_Knob_On_Active];
        const ImVec4 col_border = colorTable[Colors::Checkbox_Border];

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

        const ImVec4 col_bg = colorTable[Colors::Tooltip_Background];
        const ImVec4 col_border = colorTable[Colors::Tooltip_Border];
        const ImVec4 col_text = colorTable[Colors::Tooltip_Foreground];

        const char*  lend = ImGui::FindRenderedTextEnd(text);
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

        const ImVec4 col_bg_idle = colorTable[Colors::Drag_Background];
        const ImVec4 col_bg_hov = colorTable[Colors::Drag_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Colors::Drag_Background_Active];
        const ImVec4 col_fill_idle = colorTable[Colors::Drag_Middleground];
        const ImVec4 col_fill_hov = colorTable[Colors::Drag_Middleground_Hover];
        const ImVec4 col_fill_held = colorTable[Colors::Drag_Middleground_Active];
        const ImVec4 col_txt_idle = colorTable[Colors::Drag_Foreground];
        const ImVec4 col_txt_hov = colorTable[Colors::Drag_Foreground_Hover];
        const ImVec4 col_txt_held = colorTable[Colors::Drag_Foreground_Active];

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
        float fv = static_cast<float>(*v);
        const bool changed = DragFloat(label, &fv, speed, static_cast<float>(v_min), static_cast<float>(v_max), fmt);
        if (changed) *v = static_cast<int>(fv);
        return changed;
    }

    bool ArrowButton(const char* str_id, ImGuiDir dir)
    {
        struct ArrowState { float th, tHeld; };
        static std::unordered_map<ImGuiID, ArrowState> s_states;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiIO& io    = g.IO;
        const float alpha = style.Alpha;

        const ImGuiID id = window->GetID(str_id);
        ArrowState& st = s_states.emplace(id, ArrowState{ 0.f, 0.f }).first->second;

        auto Lerp = [&](float& val, float tgt, float spd) {
            val += (tgt - val) * ImMin(1.f, io.DeltaTime * spd);
            if (ImAbs(val - tgt) < 0.004f) val = tgt; else ImGui::MarkItemEdited(id);
        };

        const ImVec4 col_bg_idle = colorTable[Colors::Button_Background];
        const ImVec4 col_bg_hov = colorTable[Colors::Button_Background_Hover];
        const ImVec4 col_bg_held = colorTable[Colors::Button_Background_Active];

        const ImVec4 col_arr_idle = colorTable[Colors::Button_Foreground];
        const ImVec4 col_arr_hov = colorTable[Colors::Button_Foreground_Hover];
        const ImVec4 col_arr_held = colorTable[Colors::Button_Foreground_Active];

        const ImVec4 bg_now  = LerpC(LerpC(col_bg_idle, col_bg_hov,  st.th), col_bg_held,  st.tHeld);
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

} // namespace ImGuiH
