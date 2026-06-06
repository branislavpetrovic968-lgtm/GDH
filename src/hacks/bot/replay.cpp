#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "../../core/config.hpp"
#include "../../core/replayEngine.hpp"
#include "../../core/gui.hpp"

float left_over = 0.f;

class $modify(ReplayCCScheduler, cocos2d::CCScheduler) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCScheduler::update", geode::Priority::Last); 

        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Invisible").findHackByName("TPS");        
        hack.addHookPtr(self.getHook("cocos2d::CCScheduler::update").unwrap());
    }

    void update(float dt) {
        auto &engine = GDH::ReplayEngine::get();
        auto &config = Config::get();

        float tps_value = config.get<float>("invisible.tps::value", 240.f);
        float new_dt = 1.f / tps_value;

        unsigned times = static_cast<int>((dt + left_over) / new_dt);  
        if (dt <= 0.f)
            return CCScheduler::update(new_dt);

        auto start = std::chrono::high_resolution_clock::now();
        using namespace std::literals;

        for (unsigned i = 0; i < times; ++i) {
            CCScheduler::update(new_dt);

            if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {         
                times = i + 1;
                break;
            }
        }

        left_over = std::max(0.f, left_over + dt - new_dt * times);
    }
};

class $modify(ReplayGJBaseGameLayer, GJBaseGameLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Invisible").findHackByName("TPS");        
        hack.addHookPtr(self.getHook("GJBaseGameLayer::getModifiedDelta").unwrap());
    }

    // void update(float dt) {
    //     auto &config = Config::get();
    //     auto &engine = GDH::ReplayEngine::get();

    //     GJBaseGameLayer::update(dt);
    //     engine.handle_update(this);
    // }

    double getModifiedDelta(float dt) {
        if (m_resumeTimer > 0)
        {
            --m_resumeTimer;
            dt = 0.0;
        }
        
        auto& config = Config::get();
        auto fixed_dt = 1.0 / static_cast<double>(config.get<float>("invisible.tps::value", 240.f));

        auto timestep = std::min(static_cast<double>(m_gameState.m_timeWarp), 1.0) * fixed_dt;
        auto total_dt = dt + m_extraDelta;
        auto steps = std::round(total_dt / timestep);
        auto new_dt = steps * timestep;
        m_extraDelta = total_dt - new_dt;

        return static_cast<double>(new_dt);
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        GDH::ReplayEngine::get().handle_button(down, button, isPlayer1);
    }
    
    // void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        //     GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        // }
        
    void processQueuedButtons(float dt, bool clearInputQueue) {
        GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
        
        auto &engine = GDH::ReplayEngine::get();
        engine.handle_commands(this);
        engine.handle_update(this);
    }
};

class $modify(ReplayPlayLayer, PlayLayer) {
    void resetLevel() {
        auto &config = Config::get();
        auto &engine = GDH::ReplayEngine::get();

        left_over = 0;
        PlayLayer::resetLevel();

        if (engine.mode != disable) m_resumeTimer = 15;
        engine.handle_reset();  
    }

    void playEndAnimationToPos(cocos2d::CCPoint pos) {
        PlayLayer::playEndAnimationToPos(pos);
        geode::queueInMainThread([]() {
            auto& engine = GDH::ReplayEngine::get();
            if (engine.mode == state::record)
                engine.mode = state::disable;
        });
    }

    void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool instant) {
        PlayLayer::playPlatformerEndAnimationToPos(pos, instant);
        geode::queueInMainThread([]() {
            auto& engine = GDH::ReplayEngine::get();
            if (engine.mode == state::record)
                engine.mode = state::disable;
        });
    }
    
};