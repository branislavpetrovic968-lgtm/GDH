#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "../../../core/gui.hpp"
#include "../../../core/utils.hpp"
// #include "../popup.hpp"
#include <imgui-cocos.hpp>

$execute {
    auto& gui = GDH::Gui::get();
    auto& window = gui.getWindow("Shortcuts");

    window.setCustomWindowImGui([] {
        float width = 140.f;
        if (ImGui::Button("Options", {width, 0.f})) {
            auto options_layer = OptionsLayer::create();
            auto scene = cocos2d::CCScene::get();

            if (options_layer && scene) {
                auto zOrder = scene->getHighestChildZ();
                scene->addChild(options_layer, zOrder + 1);
                options_layer->showLayer(false);
            }
        }

        if (ImGui::Button("Reset Level", {width, 0.f})) {
            auto pl = PlayLayer::get();
            if (pl) pl->resetLevel();
        }

        if (ImGui::Button("Practice Mode", {width, 0.f})) {
            auto pl = PlayLayer::get();
            if (pl) pl->togglePracticeMode(!pl->m_isPracticeMode);
        }

        if (ImGui::Button("Reset Volume", {width, 0.f})) {
            auto fmod_engine = FMODAudioEngine::sharedEngine();
            fmod_engine->setBackgroundMusicVolume(0.5f);
            fmod_engine->setEffectsVolume(0.5f);
        }

        if (ImGui::Button("Uncomplete Level", {width, 0.f})) {
            GDH::Utils::UncompleteLevel();
        }

        if (ImGui::Button("Resources", {width/2, 0.f})) {
            std::string path = cocos2d::CCFileUtils::get()->getWritablePath2();
            geode::utils::file::openFolder(path);
        }
        ImGui::SameLine();
        if (ImGui::Button("AppData", {ImGui::GetContentRegionAvail().x, 0.f})) {
            auto path = geode::dirs::getSaveDir();
            geode::utils::file::openFolder(path);
        }

        if (ImGui::Button("GDH AppData", {width, 0.f})) {
            geode::utils::file::openFolder(geode::Mod::get()->getSaveDir());
        }
    });
}
#endif
