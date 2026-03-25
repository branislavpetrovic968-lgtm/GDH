#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"
#include "../../interface/imgui/widget_helper.hpp"
#include "imgui.h"
#include <imgui-cocos.hpp>

GUI_HACK_CREATE("Level", "Auto Kill", "Automatically kills the player after a certain percentage of health loss (Classic) or after a set time (Platformer)", false);

class $modify(JumpHackGJBaseGameLayer, GJBaseGameLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Auto Kill");    
        auto& config = Config::get();    
        
        hack.addHookPtr(self.getHook("GJBaseGameLayer::update").unwrap());

        hack.setCustomWindowImGui([&config]() {
            ImGui::SetNextItemWidth(160.f);
            ImGuiWidgetConfig::InputFloat("##auto_kill::percentage", "level.auto_kill::percentage", 50.f);
            ImGui::SameLine();
            ImGuiWidgetConfig::Checkbox("Percentage (%)", "level.auto_kill::percentage_enabled", true);
            
            ImGui::SetNextItemWidth(160.f);
            ImGuiWidgetConfig::InputFloat("##auto_kill::time", "level.auto_kill::time", 90.f);
            ImGui::SameLine();
            ImGuiWidgetConfig::Checkbox("Time (s)", "level.auto_kill::time_enabled", false);
        });
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (auto pl = PlayLayer::get()) {
            auto& config = Config::get();
            bool percentageEnabled = config.get<bool>("level.auto_kill::percentage_enabled", true);
            float percentage = config.get<float>("level.auto_kill::percentage", 50.f);

            bool timeEnabled = config.get<bool>("level.auto_kill::time_enabled", false);
            float time = config.get<float>("level.auto_kill::time", 90.f);

            if ((percentageEnabled && percentage <= pl->getCurrentPercent()) ||
                (timeEnabled && time <= m_gameState.m_levelTime)
            ) {
                bool noclipEnabled = config.get<bool>("core.noclip", false);
                config.set<bool>("core.noclip", false);
                if (m_player1 && !m_player1->m_isDead)
                    pl->destroyPlayer(m_player1, m_player1);
                config.set<bool>("core.noclip", noclipEnabled);
            }
        }
    }
};