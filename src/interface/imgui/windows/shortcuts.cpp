#ifdef GEODE_IS_WINDOWS
#include "imgui.h"
#include <Geode/Geode.hpp>
#include "../../../core/gui.hpp"
#include "../../../core/utils.hpp"
#include "../widgetH.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& window = gui.getWindow("Shortcuts");

    window.setCustomWindowImGui([] {
        float width = ImGui::GetContentRegionAvail().x;
        
        if (ImGuiH::Button("Options", {width, 0.f})) {
            auto options_layer = OptionsLayer::create();
            auto scene = cocos2d::CCScene::get();

            if (options_layer && scene) {
                auto zOrder = scene->getHighestChildZ();
                scene->addChild(options_layer, zOrder + 1);
                options_layer->showLayer(false);
            }
        }

        if (ImGuiH::Button("Reset Level", {width, 0.f})) {
            auto pl = PlayLayer::get();
            if (pl) pl->resetLevel();
        }

        if (ImGuiH::Button("Practice Mode", {width, 0.f})) {
            auto pl = PlayLayer::get();
            if (pl) pl->togglePracticeMode(!pl->m_isPracticeMode);
        }

        if (ImGuiH::Button("Reset Volume", {width, 0.f})) {
            auto fmod_engine = FMODAudioEngine::sharedEngine();
            fmod_engine->setBackgroundMusicVolume(0.5f);
            fmod_engine->setEffectsVolume(0.5f);
        }

        if (ImGuiH::Button("Uncomplete Level", {width, 0.f})) {
            GDH::Utils::UncompleteLevel();
        }

        if (ImGuiH::Button("Resources", {width/2, 0.f})) {
            std::string path = cocos2d::CCFileUtils::get()->getWritablePath2();
            geode::utils::file::openFolder(path);
        }
        ImGui::SameLine();
        if (ImGuiH::Button("AppData", {ImGui::GetContentRegionAvail().x, 0.f})) {
            auto path = geode::dirs::getSaveDir();
            geode::utils::file::openFolder(path);
        }

        if (ImGuiH::Button("GDH AppData", {width, 0.f})) {
            geode::utils::file::openFolder(geode::Mod::get()->getSaveDir());
        }

        if (ImGuiH::Button("Crash Game", {width, 0.f})) {
            ImGui::OpenPopup("Crash the Game");
        }

        if (ImGui::BeginPopupModal("Crash the Game")) {
            ImGuiH::GlowWindow(IM_COL32(71, 71, 131, 100), IM_COL32(255, 128, 128, 0));
            ImGui::Text("Are you sure you want to crash Geometry Dash?");
            ImGui::Text("Unsaved data will be lost");

            if (ImGuiH::Button("Cancel", {ImGui::GetContentRegionAvail().x, 0})) {
                ImGui::CloseCurrentPopup();
            }

            if (ImGuiH::Button("OK", {ImGui::GetContentRegionAvail().x, 0})) {
                volatile int* ptr = nullptr;
                *ptr = 0;
            }

            ImGui::EndPopup();
        }

    });
}
#endif
