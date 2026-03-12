#include "imgui.h"
#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/labels.hpp"
#include "../widgetH.hpp"
#include <imgui-cocos.hpp>
#include <imgui_stdlib.h>

GDH::Labels::Corner g_editCorner = GDH::Labels::Corner::Top_Left;

GDH::Labels::Corner g_popupCorner;
int g_popupId = 0;
bool g_openPopup = false;
bool g_editPopup = false;

float g_newLabelSize = 0.3f;
float g_newLabelColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

std::map<const char *, const char *> g_labelTemplates = {
    {"Time (24h)", "${TIME_24}"},
    {"Time (12h)", "${TIME_12}"},
    {"Date", "${DATE}"},
    {"Session time", "${SESSION_TIME}"},
    {"Level progress", "${PROGRESS:2}%"},
    {"FPS", "${FPS} FPS"},
    {"Custom text", "Edit me!"},
};

void drawLabelCornerControls(GDH::Labels::Corner corner) {
    ImGui::BeginChild("Labels");
    switch (corner) {
    case GDH::Labels::Corner::Top_Left: ImGui::PushID("##LabelsTLCorner"); break;
    case GDH::Labels::Corner::Top_Center: ImGui::PushID("##LabelsTCCorner"); break;
    case GDH::Labels::Corner::Top_Right: ImGui::PushID("##LabelsTRCorner"); break;
    case GDH::Labels::Corner::Center_Left: ImGui::PushID("##LabelsCLCorner"); break;
    case GDH::Labels::Corner::Center_Center: ImGui::PushID("##LabelsCCCorner"); break;
    case GDH::Labels::Corner::Center_Right: ImGui::PushID("##LabelsCRCorner"); break;
    case GDH::Labels::Corner::Bottom_Left: ImGui::PushID("##LabelsBLCorner"); break;
    case GDH::Labels::Corner::Bottom_Center: ImGui::PushID("##LabelsBCCorner"); break;
    case GDH::Labels::Corner::Bottom_Right: ImGui::PushID("##LabelsBRCorner"); break;
    }
    ImGui::Spacing();
    if (ImGuiH::Button("Add a label")) {
        g_popupCorner = corner;
        g_openPopup = true;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("?");
    ImGuiH::Tooltip("Labels are shown top-to-bottom in their corners as added here.\n"
                          "Text labels support variables that get substituted to different things like current date, level progress, etc.\n"
                          "Spacing labels add pixel spacing between text labels.\n"
                          "'Add a label' button adds a label with a pre-set text, there can be any number of any type of labels.\n"
                          "Every label has its own color and size.\n"
                          "A single text label may contain any number of variables. Add a few examples and change them around to get the feel for it.", ImGui::IsItemHovered());
    
    int id = 0;
    for (auto &label : GDH::Labels::labels[corner]) {
        ImGui::PushID(id++);
        if (ImGuiH::Button("E")) {
            g_editPopup = true;
            g_popupCorner = corner;
            g_popupId = id - 1;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (!label.enabled) ImGui::BeginDisabled(true);
        if (label.type == GDH::Labels::LabelType::Text) ImGui::InputText("##TextInput", &(label.text));
        else if (label.type == GDH::Labels::LabelType::Spacing) ImGuiH::DragFloat("##Spacing", &(label.size), 1.0f, 1.0f, 256.0f, "Spacing: %.1fpx");
        if (!label.enabled) ImGui::EndDisabled();
       ImGui::PopID();
    }
    ImGui::PopID();
    ImGui::EndChild();
}

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& window = gui.getWindow("Labels");

    window.setCustomWindowImGui([&config, &gui] {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGuiH::DragFloat("##LabelCorPadDrag", &GDH::Labels::cornerPadding, 1.0f, 0.0f, 256.0f, "Corner Padding: %.1fpx");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGuiH::DragFloat("##LabelMidPadDrag", &GDH::Labels::midPadding, 1.0f, 0.0f, 256.0f, "Mid-label Padding: %.1fpx");

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        ImGuiStyle &style = ImGui::GetStyle();
        int alpha = static_cast<int>(style.Alpha * 255);

        float width = (ImGui::GetContentRegionAvail().x - style.ItemSpacing.x*2)/3;
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                GDH::Labels::Corner corner = GDH::Labels::Corner::Top_Left;
                if (y == 0 && x == 0) corner = GDH::Labels::Corner::Top_Left;
                if (y == 0 && x == 1) corner = GDH::Labels::Corner::Top_Center;
                if (y == 0 && x == 2) corner = GDH::Labels::Corner::Top_Right;
                if (y == 1 && x == 0) corner = GDH::Labels::Corner::Center_Left;
                if (y == 1 && x == 1) corner = GDH::Labels::Corner::Center_Center;
                if (y == 1 && x == 2) corner = GDH::Labels::Corner::Center_Right;
                if (y == 2 && x == 0) corner = GDH::Labels::Corner::Bottom_Left;
                if (y == 2 && x == 1) corner = GDH::Labels::Corner::Bottom_Center;
                if (y == 2 && x == 2) corner = GDH::Labels::Corner::Bottom_Right;
                ImVec2 start(pos.x + width*x + style.ItemSpacing.x*x, pos.y + ImGui::GetFrameHeight()*y + style.ItemSpacing.y*y);
                ImVec2 end(pos.x + width*(1+x) + style.ItemSpacing.x*x, pos.y + ImGui::GetFrameHeight()*(1+y) + style.ItemSpacing.y*y);
                ImGui::InvisibleButton(fmt::format("This thing but {}", y * 3 + x).c_str(), ImVec2 { end.x - start.x, end.y - start.y });
                if (x < 2) ImGui::SameLine();
                auto color = ImColor(213, 196, 255, alpha);
                if (ImGui::IsItemClicked() || corner == g_editCorner) {
                    color = ImColor(228, 215, 255, alpha);
                    g_editCorner = corner;
                } else if (ImGui::IsItemHovered()) color = ImColor(185, 165, 240, alpha);
                drawList->AddRectFilled(start, end, color, style.FrameRounding);
                ImVec2 pad = style.ItemInnerSpacing, size = { style.ItemInnerSpacing.x * 3.0f, style.ItemInnerSpacing.y * 0.5f };
                if (y == 0 && x == 0) { start.x += pad.x; start.y += pad.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 0 && x == 1) { start.x += (end.x - start.x) / 2.0f - size.x / 2.0f; start.y += pad.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 0 && x == 2) { start.x += (end.x - start.x) - pad.x - size.x; start.y += pad.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 1 && x == 0) { start.x += pad.x; start.y += (end.y - start.y) / 2.0f - size.y / 2.0f; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 1 && x == 1) { start.x += (end.x - start.x) / 2.0f - size.x / 2.0f; start.y += (end.y - start.y) / 2.0f - size.y / 2.0f; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 1 && x == 2) { start.x += (end.x - start.x) - pad.x - size.x; start.y += (end.y - start.y) / 2.0f - size.y / 2.0f; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 2 && x == 0) { start.x += pad.x; start.y = end.y - pad.y - size.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 2 && x == 1) { start.x += (end.x - start.x) / 2.0f - size.x / 2.0f; start.y = end.y - pad.y - size.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                if (y == 2 && x == 2) { start.x += (end.x - start.x) - pad.x - size.x; start.y = end.y - pad.y - size.y; end.x = start.x + size.x; end.y = start.y + size.y; }
                drawList->AddRectFilled(start, end, ImColor(26, 26, 73, alpha));
            }
        }

        pos.y += ImGui::GetFrameHeight()*3 + style.ItemSpacing.y*3;
        ImGui::SetCursorScreenPos(pos);

        ImGui::Separator();

        drawLabelCornerControls(g_editCorner);
        
        if (g_openPopup) {
            ImGui::OpenPopup("Add a label");
            g_openPopup = false;
        }

        if (ImGui::BeginPopupModal("Add a label")) {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::ColorEdit4("##Color", g_newLabelColor);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGuiH::DragFloat("##Size", &g_newLabelSize, 0.1f, 0.1f, 4.0f, "Size: %.1f");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            for (const auto &[name, value] : g_labelTemplates) {
                if (ImGuiH::Button(name, ImVec2 { ImGui::GetContentRegionAvail().x, 0 })) {
                    GDH::Labels::labels[g_popupCorner].push_back(GDH::Labels::Label(value, g_newLabelColor, g_newLabelSize));
                    ImGui::CloseCurrentPopup();
                }
            }
            if (ImGuiH::Button("Spacing", ImVec2 { ImGui::GetContentRegionAvail().x, 0.f })) {
                GDH::Labels::labels[g_popupCorner].push_back(GDH::Labels::Label(g_newLabelSize));
                ImGui::CloseCurrentPopup();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGuiH::Button("Close", ImVec2 { 240.f, 0.f })) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (g_editPopup) {
            ImGui::OpenPopup("Editing the label");
            g_editPopup = false;
        }

        if (ImGui::BeginPopup("Editing the label")) {
            ImGuiH::Checkbox("Enabled", &(GDH::Labels::labels[g_popupCorner][g_popupId].enabled));
            if (!GDH::Labels::labels[g_popupCorner][g_popupId].enabled) ImGui::BeginDisabled(true);
            std::map<GDH::Labels::LabelType, const char *> types = {
                {GDH::Labels::LabelType::Text, "Text"},
                {GDH::Labels::LabelType::Spacing, "Spacing"},
            };
                
            if (ImGui::BeginCombo("Label Type", types[GDH::Labels::labels[g_popupCorner][g_popupId].type])) {
                for (const auto &[type, name] : types) {
                    if (ImGui::Selectable(name, type == GDH::Labels::labels[g_popupCorner][g_popupId].type)) GDH::Labels::labels[g_popupCorner][g_popupId].type = type;
                }
                
                ImGui::EndCombo();
            }
            if (GDH::Labels::labels[g_popupCorner][g_popupId].type == GDH::Labels::LabelType::Text) {
                ImGui::InputText("Label Text", &(GDH::Labels::labels[g_popupCorner][g_popupId].text));
                ImGui::ColorEdit4("Color", GDH::Labels::labels[g_popupCorner][g_popupId].color);
                ImGuiH::Checkbox("Rainbow", &(GDH::Labels::labels[g_popupCorner][g_popupId].rainbow));
                ImGuiH::DragFloat("Size", &(GDH::Labels::labels[g_popupCorner][g_popupId].size), 0.1f, 0.1f, 4.0f);
            } else if (GDH::Labels::labels[g_popupCorner][g_popupId].type == GDH::Labels::LabelType::Spacing) {
                ImGuiH::DragFloat("##Spacing", &(GDH::Labels::labels[g_popupCorner][g_popupId].size), 1.0f, 1.0f, 256.0f, "Spacing: %.1fpx");
            }
            if (!GDH::Labels::labels[g_popupCorner][g_popupId].enabled) ImGui::EndDisabled();
            if (ImGuiH::Button("Delete")) {
                GDH::Labels::labels[g_popupCorner].erase(std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId));
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(g_popupId == 0);
            if (ImGuiH::Button("Move up")) {
                std::iter_swap(std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId - 1), std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId));
                g_popupId--;
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(g_popupId == GDH::Labels::labels[g_popupCorner].size() - 1);
            if (ImGuiH::Button("Move down")) {
                std::iter_swap(std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId), std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId + 1));
                g_popupId++;
            }
            ImGui::EndDisabled();
            ImGui::EndPopup();
        }
    });
}
#endif
