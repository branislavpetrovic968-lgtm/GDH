#ifdef GEODE_IS_WINDOWS
#include <Geode/Geode.hpp>
#include "imgui.h"
#include <imgui_stdlib.h>
#include <imgui-cocos.hpp>
#include "../../../core/gui.hpp"
#include "../../../core/replayEngine.hpp"
#include "../../../core/config.hpp"
#include "../../../core/utils.hpp"
#include "../widgetH.hpp"
#include "../layout.hpp"

$execute {
    auto& gui = GDH::Gui::get();
    auto& config = Config::get();
    auto& layout = GDH::Layout::Manager::get();
    auto& window = gui.getWindow("Replay Engine");
    auto& engine = GDH::ReplayEngine::get();

    window.setCustomWindowImGui([&config, &gui, &layout, &engine] {
        using namespace GDH;

        if (ImGuiH::RadioButton("Disable", engine.mode == state::disable)) {
            engine.mode = state::disable;
        }
        ImGui::SameLine();
        
        
        if (ImGuiH::RadioButton("Record", engine.mode == state::record)) {
            if (engine.mode != state::record)
                engine.clear();
            
            engine.mode = state::record;
        }
        ImGui::SameLine();
        
        if (ImGuiH::RadioButton("Play", engine.mode == state::play)) {
            engine.mode = state::play;
        }
        
        ImGui::Separator();
        
        static std::string macro_name;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##replay_name", "Enter the macro name", &macro_name);
        
        if (ImGuiH::Button("Save", {ImGui::GetContentRegionAvail().x / 3, NULL})) {
            ImGuiH::AddPopup(engine.save(macro_name));
        }
        ImGui::SameLine();
        
        if (ImGuiH::Button("Load", {ImGui::GetContentRegionAvail().x / 2, NULL})) {
            ImGuiH::AddPopup(engine.load(macro_name));
        }
        ImGui::SameLine();
        
        if (ImGuiH::Button("Clear", {ImGui::GetContentRegionAvail().x, NULL})) {
            engine.clear();
            ImGuiH::AddPopup("Replay has been cleared");
        }

        ImGui::Text("Replay Size: %zu/%zu", engine.get_current_index(), engine.get_actions_size());
        ImGui::Text("Frame: %llu", engine.get_frame());

        ImGui::Separator();
        ImGui::Selectable("More Settings");
    });
}
#endif
