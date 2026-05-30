#ifdef GEODE_IS_WINDOWS
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui-cocos.hpp>
#include <imgui_stdlib.h>
#include "../../../core/gui.hpp"
#include "../../../core/config.hpp"
#include "../../../core/labels.hpp"
#include "../widgetH.hpp"
#include "../layout.hpp"

GDH::Labels::Corner g_editCorner = GDH::Labels::Corner::Top_Left;
GDH::Labels::Corner g_popupCorner;
int g_popupId = 0;
bool g_openPopup = false;
bool g_editPopup = false;

float g_newLabelSize = 0.35f;
std::array<float, 4> g_newLabelColor = { 1.0f, 1.0f, 1.0f, 0.30f };

const std::vector<std::pair<const char*, const char*>> g_labelTemplates = {
    {"Attempts", "Attempt {attempt}"},
    {"CPS Counter", "{cps}/{cps_high}/{clicks}"},
    {"Custom text", "Edit me!"},
    {"Date", "{date}"},
    {"Death Counter", "{deaths} Deaths"},
    {"FPS", "{fps} FPS"},
    {"Frame", "{frame} Frame"},
    {"Level Info", "{level_name} by {level_creator}"},
    {"Level ID", "{level_id}"},
    {"Level progress", "{progress:2}%"},
    {"Noclip Accuracy (%)", "{noclip_accuracy}%"},
    {"Normal Percent", "{normal_percent}%"},
    {"Practice Percent", "{practice_percent}%"},
    {"Rainbow Text", "Rainbow Text!!"},
    {"Replay Engine State", "{re_state}"},
    {"Session time", "{session_time}"},
    {"Testmode", "{testmode}"},
    {"Time (12h)", "{time_12}"},
    {"Time (24h)", "{time_24}"}
};

void drawLabelCornerControls(GDH::Labels::Corner corner) {
    ImGui::BeginChild("Labels");
    
    ImGui::PushID(static_cast<int>(corner));
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
    auto& currentLabels = GDH::Labels::labels[corner];
    for (auto &label : currentLabels) {
        ImGui::PushID(id++);
        if (ImGuiH::Button("E")) {
            g_editPopup = true;
            g_popupCorner = corner;
            g_popupId = id - 1;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        
        if (!label.enabled) ImGui::BeginDisabled(true);
        if (label.type == GDH::Labels::LabelType::Text) {
            ImGui::InputText("##TextInput", &label.text);
        } else if (label.type == GDH::Labels::LabelType::Spacing) {
            ImGuiH::DragFloat("##Spacing", &label.size, 1.0f, 1.0f, 256.0f, "Spacing: %.1fpx");
        }
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
    auto& layout = GDH::Layout::Manager::get();

    window.setCustomWindowImGui([&config, &gui, &layout] {
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGuiH::DragFloat("##LabelCorPadDrag", &GDH::Labels::cornerPadding, 1.0f, 0.0f, 256.0f, "Corner Padding: %.1fpx");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGuiH::DragFloat("##LabelMidPadDrag", &GDH::Labels::midPadding, 1.0f, 0.0f, 256.0f, "Mid-label Padding: %.1fpx");

        static const char* cornerNames[] = {
            "Top Left", "Top Center", "Top Right",
            "Center Left", "Center Center", "Center Right",
            "Bottom Left", "Bottom Center", "Bottom Right"
        };

        int currentCorner = static_cast<int>(g_editCorner);
        if (ImGui::Combo("Corner", &currentCorner, cornerNames, IM_ARRAYSIZE(cornerNames))) {
            g_editCorner = static_cast<GDH::Labels::Corner>(currentCorner);
        }

        ImGui::Separator();
        drawLabelCornerControls(g_editCorner);
        
        if (g_openPopup) {
            ImGui::OpenPopup("Add a label##Modal");
            g_openPopup = false;
        }

        ImVec4 bgColor = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        bgColor.w = 1.0f;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
        
        if (ImGui::BeginPopupModal("Add a label##Modal", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGuiH::GlowWindow(IM_COL32(71, 71, 131, 100), IM_COL32(100, 100, 255, 0));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::ColorEdit4("##Color", g_newLabelColor.data());
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGuiH::DragFloat("##Size", &g_newLabelSize, 0.1f, 0.1f, 4.0f, "Size: %.1f");
            
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            for (const auto &[name, value] : g_labelTemplates) {
                if (ImGuiH::Button(name, {ImGui::GetContentRegionAvail().x, 0})) {
                    std::string_view nameView(name);
                    bool isRainbowButton = (nameView == "Rainbow Text");
                    bool isCPSButton = (nameView == "CPS Counter");
                    bool isNoclip = (nameView == "Death Counter" || nameView == "Noclip Accuracy (%)");

                    if (isRainbowButton) g_newLabelColor[3] = 0.85f;
                    
                    GDH::Labels::labels[g_popupCorner].push_back(
                        GDH::Labels::Label(value, g_newLabelColor, g_newLabelSize, isRainbowButton, isCPSButton, isNoclip)
                    );

                    if (isRainbowButton) g_newLabelColor[3] = 0.30f;
                    ImGui::CloseCurrentPopup();
                }
            }
            
            if (ImGuiH::Button("Spacing", {ImGui::GetContentRegionAvail().x, 0.f})) {
                GDH::Labels::labels[g_popupCorner].push_back(GDH::Labels::Label(g_newLabelSize));
                ImGui::CloseCurrentPopup();
            }

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            if (ImGuiH::Button("Close", {layout.multipleScale(290.f), 0.f})) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();

        if (g_editPopup) {
            ImGui::OpenPopup("Editing the label");
            g_editPopup = false;
        }

        if (ImGui::BeginPopup("Editing the label")) {
            auto& targetLabels = GDH::Labels::labels[g_popupCorner];
            auto& currentLabel = targetLabels[g_popupId];

            ImGuiH::Checkbox("Enabled", &currentLabel.enabled);
            if (!currentLabel.enabled) ImGui::BeginDisabled(true);
            
            const char* typeName = (currentLabel.type == GDH::Labels::LabelType::Text) ? "Text" : "Spacing";
                
            if (ImGui::BeginCombo("Label Type", typeName)) {
                if (ImGui::Selectable("Text", currentLabel.type == GDH::Labels::LabelType::Text)) {
                    currentLabel.type = GDH::Labels::LabelType::Text;
                }
                if (ImGui::Selectable("Spacing", currentLabel.type == GDH::Labels::LabelType::Spacing)) {
                    currentLabel.type = GDH::Labels::LabelType::Spacing;
                }
                ImGui::EndCombo();
            }
            
            if (currentLabel.type == GDH::Labels::LabelType::Text) {
                ImGui::InputText("Label Text", &currentLabel.text);
                ImGui::ColorEdit4("Color", currentLabel.color.data());
                
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
                ImGui::Text("Color Action:");
                ImGuiH::Checkbox("Rainbow", &currentLabel.rainbow); ImGui::SameLine();
                ImGuiH::Checkbox("CPS", &currentLabel.cps); ImGui::SameLine();
                ImGuiH::Checkbox("Noclip", &currentLabel.noclip);
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
                
                ImGuiH::DragFloat("Size", &currentLabel.size, 0.1f, 0.1f, 4.0f);
            } else if (currentLabel.type == GDH::Labels::LabelType::Spacing) {
                ImGuiH::DragFloat("##Spacing", &currentLabel.size, 1.0f, 1.0f, 256.0f, "Spacing: %.1fpx");
            }
            
            if (!currentLabel.enabled) ImGui::EndDisabled();
            
            if (ImGuiH::Button("Delete")) {
                targetLabels.erase(targetLabels.begin() + g_popupId);
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::SameLine();
            ImGui::BeginDisabled(g_popupId == 0);
            if (ImGuiH::Button("Move up")) {
                std::swap(targetLabels[g_popupId - 1], targetLabels[g_popupId]);
                g_popupId--;
            }
            ImGui::EndDisabled();
            
            ImGui::SameLine();
            ImGui::BeginDisabled(g_popupId == static_cast<int>(targetLabels.size()) - 1);
            if (ImGuiH::Button("Move down")) {
                std::swap(targetLabels[g_popupId], targetLabels[g_popupId + 1]);
                g_popupId++;
            }
            ImGui::EndDisabled();
            
            ImGui::EndPopup();
        }
    });
}
#endif