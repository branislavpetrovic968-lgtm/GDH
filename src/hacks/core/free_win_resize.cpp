#ifdef GEODE_IS_WINDOWS

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <imgui-cocos.hpp>
#include "../../core/gui.hpp"
#include "../../core/windowsUtils.hpp"

GUI_HACK_CREATE("Core", "core.free_win_resize", "Free Window Resize", "Allows free window resizing", false);

$execute {
    auto& config = Config::get();
    auto& gui = GDH::Gui::get();
    auto* hack = gui.getWindow("Core").findHack("core.free_win_resize");   
    hack->setHandler([](bool enabled) {
        static auto result1 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xD93CA), {0x90, 0x90, 0x90, 0x90, 0x90});
        static auto patch1 = result1.isErr() ? nullptr : result1.unwrap();

        static auto result2 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xD7589), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90});
        static auto patch2 = result1.isErr() ? nullptr : result2.unwrap();

        static auto result3 = geode::Mod::get()->patch((void*)(geode::base::getCocos() + 0xD8A67), {0x48, 0xe9});
        static auto patch3 = result1.isErr() ? nullptr : result3.unwrap();

        if (enabled) {
            if (patch1) (void) patch1->enable();
            if (patch2) (void) patch1->enable();
            if (patch3) (void) patch1->enable();
        } else {
            if (patch1) (void) patch1->disable();
            if (patch2) (void) patch1->disable();
            if (patch3) (void) patch1->disable();
        }        
    });

    hack->setCustomWindowImGui([&config]{
        bool maximaze_window = config.get<bool>("core.free_win_resize::maximaze_window", true);
        if (ImGui::Checkbox("Maximize on Startup", &maximaze_window))
            config.set<bool>("core.free_win_resize::maximaze_window", maximaze_window);
    });

    geode::queueInMainThread([&config] {
        if (config.get<bool>("core.free_win_resize::maximaze_window", true))
            ShowWindow(GDH::WindowUtils::FindMainHWND(), SW_MAXIMIZE);
    });
}

#endif