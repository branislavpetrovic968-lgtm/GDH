#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/labels.hpp"
// #include "../widget_helper.hpp"
#include <imgui-cocos.hpp>
#include <imgui_stdlib.h>

GDH::Labels::Corner g_popupCorner;
int g_popupId = 0;
bool g_openPopup = false;
bool g_editPopup = false;

float g_newLabelSize = 0.3f;
float g_newLabelColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

std::unordered_map<const char *, const char *> g_labelTemplates = {
    {"Time (24h)", "${TIME_24}"},
    {"Custom text", "Edit me!"},
};

void drawLabelCornerControls(GDH::Labels::Corner corner) {
    ImGui::BeginChild("Labels");
    switch (corner) {
    case GDH::Labels::Corner::Top_Left: ImGui::PushID("##LabelsTLCorner"); break;
    case GDH::Labels::Corner::Top_Center: ImGui::PushID("##LabelsTCCorner"); break;
    case GDH::Labels::Corner::Top_Right: ImGui::PushID("##LabelsTRCorner"); break;
    case GDH::Labels::Corner::Bottom_Left: ImGui::PushID("##LabelsBLCorner"); break;
    case GDH::Labels::Corner::Bottom_Center: ImGui::PushID("##LabelsBCCorner"); break;
    case GDH::Labels::Corner::Bottom_Right: ImGui::PushID("##LabelsBRCorner"); break;
    }
    if (ImGui::Button("Add a label")) {
        g_popupCorner = corner;
        g_openPopup = true;
    }
    int id = 0;
    for (auto &label : GDH::Labels::labels[corner]) {
        ImGui::PushID(id++);
        if (ImGui::Button("E")) {
            g_editPopup = true;
            g_popupCorner = corner;
            g_popupId = id - 1;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("##TextInput", &(label.text));
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
        ImGui::DragFloat("##LabelCorPadDrag", &GDH::Labels::cornerPadding, 1.0f, 0.0f, 256.0f, "Corner Padding: %.1fpx");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##LabelMidPadDrag", &GDH::Labels::midPadding, 1.0f, 0.0f, 256.0f, "Mid-label Padding: %.1fpx");
        if (ImGui::BeginTabBar("LabelTabBar")) {
            if (ImGui::BeginTabItem("TL")) {
                drawLabelCornerControls(GDH::Labels::Corner::Top_Left);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("TC")) {
                drawLabelCornerControls(GDH::Labels::Corner::Top_Center);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("TR")) {
                drawLabelCornerControls(GDH::Labels::Corner::Top_Right);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("BL")) {
                drawLabelCornerControls(GDH::Labels::Corner::Bottom_Left);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("BC")) {
                drawLabelCornerControls(GDH::Labels::Corner::Bottom_Center);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("BR")) {
                drawLabelCornerControls(GDH::Labels::Corner::Bottom_Right);
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        if (g_openPopup) {
            ImGui::OpenPopup("Add a label");
            g_openPopup = false;
        }

        ImGui::SetNextWindowSize(ImVec2 { 200, 300 });
        if (ImGui::BeginPopupModal("Add a label")) {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::ColorEdit4("##Color", g_newLabelColor);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##Size", &g_newLabelSize, 0.1f, 0.1f, 4.0f, "Size: %.1f");
            ImGui::Separator();
            for (const auto &[name, value] : g_labelTemplates) {
                if (ImGui::Button(name, ImVec2 { ImGui::GetContentRegionAvail().x, 0 })) {
                    GDH::Labels::labels[g_popupCorner].push_back(GDH::Labels::Label { value, g_newLabelColor[0], g_newLabelColor[1], g_newLabelColor[2], g_newLabelColor[3], g_newLabelSize, true, false });
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

        if (g_editPopup) {
            ImGui::OpenPopup("Editing the label");
            g_editPopup = false;
        }

        ImGui::SetNextWindowSize(ImVec2 { 300, 150 });
        if (ImGui::BeginPopup("Editing the label")) {
            ImGui::Checkbox("Enabled", &(GDH::Labels::labels[g_popupCorner][g_popupId].enabled));
            if (!GDH::Labels::labels[g_popupCorner][g_popupId].enabled) ImGui::BeginDisabled(true);
            ImGui::InputText("Label Text", &(GDH::Labels::labels[g_popupCorner][g_popupId].text));
            ImGui::ColorEdit4("Color", GDH::Labels::labels[g_popupCorner][g_popupId].color);
            ImGui::Checkbox("Rainbow", &(GDH::Labels::labels[g_popupCorner][g_popupId].rainbow));
            ImGui::DragFloat("Size", &(GDH::Labels::labels[g_popupCorner][g_popupId].size), 0.1f, 0.1f, 4.0f);
            if (!GDH::Labels::labels[g_popupCorner][g_popupId].enabled) ImGui::EndDisabled();
            if (ImGui::Button("Delete")) {
                GDH::Labels::labels[g_popupCorner].erase(std::next(GDH::Labels::labels[g_popupCorner].begin(), g_popupId));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    });
}
#endif
