#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "../../core/config.hpp"
#include "../../core/replayEngine.hpp"

float left_over = 0.f;

class $modify(ReplayCCScheduler, cocos2d::CCScheduler) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCScheduler::update", geode::Priority::Last); 
    }

    void update(float dt) {
        auto &engine = GDH::ReplayEngine::get();
        if (engine.mode == state::disable) return CCScheduler::update(dt);

        auto &config = Config::get();

        float tps_value = config.get<float>("tps_value", 240.f);
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
    void update(float dt) {
        auto &config = Config::get();
        auto &engine = GDH::ReplayEngine::get();

        GJBaseGameLayer::update(dt);
        engine.handle_update(this);
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        GDH::ReplayEngine::get().handle_button(down, button, isPlayer1);
    }

    void processQueuedButtons(float dt, bool clearInputQueue) {
        GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
        GDH::ReplayEngine::get().handle_commands(this);
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

    void playPlatformerEndAnimationToPos(cocos2d::CCPoint pos, bool idk) {
        PlayLayer::playPlatformerEndAnimationToPos(pos, idk);
        geode::queueInMainThread([]() {
            auto& engine = GDH::ReplayEngine::get();
            if (engine.mode == state::record)
                engine.mode = state::disable;
        });
    }
    
};