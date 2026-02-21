#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Level", "Hide Attempts", "Hides the attempt count in-game", false);

$execute {
    auto& gui = GDH::Gui::get();
    auto& hack = gui.getWindow("Level").findHackByName("Hide Attempts");   
    hack.setGameVariableID(GameVar::HideAttemptsNormal);
}