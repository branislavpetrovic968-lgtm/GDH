#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>

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
        // orbs 14
    }  

    // removing orbs
    auto currencyKey = gsm->getCurrencyKey(level);
    int awardedCurrency = gsm->getAwardedCurrencyForLevel(level);
    gsm->setStat("14", gsm->getStat("14") - awardedCurrency);

    cocos2d::CCDictionary* dict = nullptr;
    if (level->m_dailyID > 0) {
        dict = gsm->m_timelyCurrencyScores;
    }
    else if (level->m_levelType == GJLevelType::Main) {
        dict = gsm->m_mainCurrencyScores;
    }
    else if (level->m_gauntletLevel) {
        dict = gsm->m_gauntletCurrencyScores;
    }
    else {
        dict = gsm->m_onlineCurrencyScores;
    }
    dict->removeObjectForKey(currencyKey);


    // removing coins
    for (auto i = 0; i < level->m_coins; i++) {
        auto key = level->getCoinKey(i + 1);
        if (gsm->hasUserCoin(key))
            gsm->m_verifiedUserCoins->removeObjectForKey(key);
        else if (gsm->hasPendingUserCoin(key))
            gsm->m_pendingUserCoins->removeObjectForKey(key);
        else if (gsm->hasSecretCoin(key))
            gsm->m_playerStats->removeObjectForKey(fmt::format("unique_{}", key));
    }

    level->m_normalPercent = 0;
    level->m_newNormalPercent2 = 0;
    level->m_practicePercent = 0;
    level->m_attempts = 0;
    level->m_attemptTime = 0;
    level->m_firstCoinVerified = 0;
    level->m_secondCoinVerified = 0;
    level->m_thirdCoinVerified = 0;
    level->m_isVerified = 0;
    level->m_isVerifiedRaw = 0;
    level->m_bestTime = 0;
    level->m_localBestTimes = "";
    level->m_inputsTime = "";
    level->m_personalBests = "";
    level->m_jumps = 0;
    level->m_clicks = 0;
    level->m_orbCompletion = 0;
    level->m_platformerSeed = 0;
    level->m_bestPoints = 0;
    level->m_isCompletionLegitimate = true;
}

float GDH::Utils::easeInOut(float x) {
    return -(cosf(M_PI * x) - 1) / 2;
}

void GDH::Utils::hsvToRgb(float h, float s, float v, float &r, float &g, float &b) {
    float chroma = v * s;
    h *= 360.0f;
    float h_prime = h / 60.0f;
    float x = chroma * (1.0f - std::fabs(std::fmod(h_prime, 2.0f) - 1.0f));
    float r1, g1, b1;
    if (0 <= h_prime && h_prime < 1) { r1 = chroma; g1 = x; b1 = 0; }
    else if (1 <= h_prime && h_prime < 2) { r1 = x; g1 = chroma; b1 = 0; }
    else if (2 <= h_prime && h_prime < 3) { r1 = 0; g1 = chroma; b1 = x; }
    else if (3 <= h_prime && h_prime < 4) { r1 = 0; g1 = x; b1 = chroma; }
    else if (4 <= h_prime && h_prime < 5) { r1 = x; g1 = 0; b1 = chroma; }
    else if (5 <= h_prime && h_prime < 6) { r1 = chroma; g1 = 0; b1 = x; }
    float m = v - chroma;
    r = r1 + m; g = g1 + m; b = b1 + m;
}

float GDH::Utils::getFps() {
    static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    static float frameTimes[10] = {};
    static float frameTimeSum = 0.0f;
    static int index = 0;
    static int count = 0;

    auto now = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    frameTimeSum -= frameTimes[index];
    frameTimes[index] = deltaTime;
    frameTimeSum += deltaTime;

    index = (index + 1) % 10;
    if (count < 10) ++count;

    return static_cast<float>(count) / frameTimeSum;
}

void GDH::Utils::setPitchShifter(int semitones) {
    static std::vector<FMOD::DSP*> shifters;
    auto* channel = FMODAudioEngine::get()->m_backgroundMusicChannel;
    if (!channel) return;

    semitones = std::clamp(semitones, -72, 72);

    int count = semitones == 0 ? 0 : std::ceil(std::abs(semitones) / 12.0f);

    while (shifters.size() > count) {
        channel->removeDSP(shifters.back());
        shifters.back()->release();
        shifters.pop_back();
    }

    while (shifters.size() < count) {
        FMOD::DSP* dsp = nullptr;
        if (FMODAudioEngine::get()->m_system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp) != FMOD_OK) return;
        channel->addDSP(0, dsp);
        shifters.push_back(dsp);
    }

    float factor = std::pow(2.0f, (static_cast<float>(semitones) / count) / 12.0f);
    for (auto* dsp : shifters) {
        dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 2048);
        dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, factor);
    }
}


void GDH::Utils::setReverb(float decayTime) {
    static FMOD::DSP* reverb_dsp = nullptr;
    auto* channel = FMODAudioEngine::get()->m_backgroundMusicChannel;
    if (!channel) return;

    if (decayTime <= 0.0f) {
        if (!reverb_dsp) return;
        channel->removeDSP(reverb_dsp);
        reverb_dsp->release();
        return void(reverb_dsp = nullptr);
    }

    if (!reverb_dsp) {
        if (FMODAudioEngine::get()->m_system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &reverb_dsp) != FMOD_OK) return;
        channel->addDSP(0, reverb_dsp);
    }

    reverb_dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, std::clamp(decayTime, 100.0f, 20000.0f));
}