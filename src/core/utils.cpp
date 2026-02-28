#include "utils.hpp"
#include <algorithm>
#include <cctype>

#include <fmt/core.h>
#include <Geode/Geode.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

std::string GDH::Utils::String::toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string GDH::Utils::String::toUpperCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string GDH::Utils::String::replaceChar(const std::string& input, char from, char to) {
    std::string result = input;
    std::replace(result.begin(), result.end(), from, to);
    return result;
}

std::string GDH::Utils::formatTime(double sec, bool ms = false) {
    int h = sec / 3600, m = (int(sec) % 3600) / 60, s = int(sec) % 60;
    int cs = (sec - int(sec)) * 100;
    
    auto base = h > 0 ? fmt::format("{:02d}:{:02d}:{:02d}", h, m, s)
                      : fmt::format("{:02d}:{:02d}", m, s);
    return ms ? fmt::format("{}.{:02d}", base, cs) : base;
}

void GDH::Utils::updateCursorState(bool show) {
    bool canShowInLevel = true;
    if (auto* playLayer = PlayLayer::get()) {
        canShowInLevel = playLayer->m_hasCompletedLevel ||
                         playLayer->m_isPaused ||
                         GameManager::sharedState()->getGameVariable("0024");
    }
    if (show || canShowInLevel)
        PlatformToolbox::showCursor();
    else
        PlatformToolbox::hideCursor();
}

void GDH::Utils::UncompleteLevel() {
    auto pl = PlayLayer::get();
	
	if (!pl) {
		FLAlertLayer::create("Uncomplete Level", "Enter playlayer to uncomplete the level", "OK")->show();
		return;
	}
	
    auto gsm = GameStatsManager::get();
    auto glm = GameLevelManager::get();
    auto level = pl->m_level;
    int level_id = level->m_levelID.value();

    if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
        gsm->m_completedLevels->removeObjectForKey(fmt::format("c_{}", level_id));
        gsm->m_completedLevels->removeObjectForKey(fmt::format("star_{}", level_id)); 
        gsm->m_completedLevels->removeObjectForKey(fmt::format("gstar_{}", level_id));       
        gsm->m_completedLevels->removeObjectForKey(fmt::format("dstar_{}", level_id));     
        gsm->m_completedLevels->removeObjectForKey(fmt::format("gdemon_{}", level_id));   
        gsm->m_completedLevels->removeObjectForKey(fmt::format("demon_{}", level_id));    
        gsm->m_completedLevels->removeObjectForKey(fmt::format("ddemon_{}", level_id));

        // removing completed levels stat
        if (level_id >= 1 && level_id <= 22)
            gsm->setStat("4", gsm->getStat("4") - 1);
        else
            gsm->setStat("3", gsm->getStat("3") - 1);

        if (level->m_stars > 0) {
            // removing starts
            if (level->isPlatformer())
                gsm->setStat("28", gsm->getStat("28") - level->m_stars);
            else
                gsm->setStat("6", gsm->getStat("6") - level->m_stars);
        }

        // removing demons stat
        if (level->m_demon > 0) 
            gsm->setStat("5", gsm->getStat("5") - 1);
            
        // removing coins stat
        if (level_id >= 1 && level_id <= 22)
            gsm->setStat("8", gsm->getStat("8") - level->m_coinsVerified);
        else
            gsm->setStat("12", gsm->getStat("12") - level->m_coinsVerified);
            
    
        // to do: remove more stat
        // total jumps: 1
        // total attempts: 2
        // collected stars: 6 (done)
        // collected diamons 13
        // total orbs collected 22
        // completed levels 3 (done)
        // completed online levels 4 (done)
        // completed insane levels 42
        // completed demon levels 5 (done)
        // daily levels 15
        // collected secret coins 8 (done)
        // collected user coins 12 (done)
        // completed map packs 7
        // completed gauntlents 40
        // liked/disliked levels 10
        // rated levels 11
        // collected list rewards 41
        // players destroyed 9
    }  


    level->m_normalPercent = 0;
    level->m_practicePercent = 0;
    level->m_attempts = 0;
    level->m_jumps = 0;
    level->m_newNormalPercent2 = 0;
    level->m_clicks = 0;
    level->m_orbCompletion = 0;
    level->m_bestTime = 0;
    level->m_bestPoints = 0;
    level->m_isCompletionLegitimate = true;
    // level->m_k111 = 0;

    for (auto i = 0; i < level->m_coins; i++) {
        auto key = level->getCoinKey(i + 1);
        gsm->m_verifiedUserCoins->removeObjectForKey(key);
    }
}

float GDH::Utils::easeInOut(float x) {
    return -(cosf(M_PI * x) - 1) / 2;
}
