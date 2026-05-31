#include <Geode/Geode.hpp>
#include <Geode/binding/UILayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/checkpointData.hpp"
#include "../../core/replayEngine.hpp"

GUI_HACK_CREATE("Level", "Practice Fix", "The right way to respawn a player", false);

static std::array<bool, 6> m_clickReleased = {true, true, true, true, true, true};

class $modify(PracticeFixPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<CheckpointObject*, std::pair<checkpoint_data, checkpoint_data>> m_checkpoints;

        ~Fields() {
            m_clickReleased.fill(true);
        }
    };

    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Practice Fix");        
        
        hack.addHookPtr(self.getHook("PlayLayer::createCheckpoint").unwrap());
        hack.addHookPtr(self.getHook("PlayLayer::removeCheckpoint").unwrap());
        hack.addHookPtr(self.getHook("PlayLayer::loadFromCheckpoint").unwrap());
        hack.addHookPtr(self.getHook("PlayLayer::resetLevel").unwrap());
    }

    CheckpointObject* createCheckpoint() {
        CheckpointObject* checkpoint = PlayLayer::createCheckpoint();
        if (!checkpoint) return checkpoint;

        auto fields = m_fields.self();
        auto& data = fields->m_checkpoints[checkpoint];

        auto& releaseP1 = *reinterpret_cast<std::array<bool, 3>*>(&m_clickReleased[0]);
        auto& releaseP2 = *reinterpret_cast<std::array<bool, 3>*>(&m_clickReleased[3]);

        auto& checkpoint_p1 = data.first;   
        checkpoint_p1 = GDH::PracticeFix::store(m_player1, releaseP1);
        
        if (m_gameState.m_isDualMode) {
            auto& checkpoint_p2 = data.second;
            checkpoint_p2 = GDH::PracticeFix::store(m_player2, releaseP2);
        }
        
        return checkpoint;
    }

    void removeCheckpoint(bool first) {
        CheckpointObject* checkpoint = nullptr;
        if (m_checkpointArray->count()) {
            if (first) checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0));
            else checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->lastObject());
        }

        auto fields = m_fields.self();
        if (checkpoint && fields->m_checkpoints.contains(checkpoint)) {
            fields->m_checkpoints.erase(checkpoint);
        }

        PlayLayer::removeCheckpoint(first);
    }

    void loadFromCheckpoint(CheckpointObject* checkpoint) {
        auto fields = m_fields.self();
        if (fields->m_checkpoints.contains(checkpoint)) {
            PlayLayer::loadFromCheckpoint(checkpoint);
            
            auto& data = fields->m_checkpoints[checkpoint];
            auto& checkpoint_p1 = data.first;
            
            auto& releaseP1 = *reinterpret_cast<std::array<bool, 3>*>(&m_clickReleased[0]);
            auto& releaseP2 = *reinterpret_cast<std::array<bool, 3>*>(&m_clickReleased[3]);

            GDH::PracticeFix::apply(m_player1, checkpoint_p1, releaseP1);
            
            if (m_gameState.m_isDualMode) {
                auto& checkpoint_p2 = data.second;
                GDH::PracticeFix::apply(m_player2, checkpoint_p2, releaseP2);
            }
            else {
                releaseP2.fill(true);
            }
        
            return;
        }
        PlayLayer::loadFromCheckpoint(checkpoint);
    }

    void resetLevel() {
        if (!m_checkpointArray->count()) {
            auto fields = m_fields.self();
            fields->m_checkpoints.clear();
        }
        
        PlayLayer::resetLevel();

        auto& engine = GDH::ReplayEngine::get();
        if (engine.mode != state::disable) return;

        if (auto gjbgl = GJBaseGameLayer::get()) {
            gjbgl->m_queuedButtons.clear();

            for (int i = 0; i < 3; ++i) {
                if (!m_clickReleased[i]) {
                    gjbgl->queueButton(i + 1, false, false, 0.0);
                }
                if (!m_clickReleased[i + 3]) {
                    gjbgl->queueButton(i + 1, false, true, 0.0);
                }
            }

            auto isJumpButtonPressedForPlayer = [](UILayer* self, bool checkPlayer2) -> bool {
                return checkPlayer2 ? (self->m_p2TouchId != -1 || self->m_p2Jumping) : (self->m_p1TouchId != -1 || self->m_p1Jumping);
            };

            if (isJumpButtonPressedForPlayer(gjbgl->m_uiLayer, false)) {
                gjbgl->queueButton(1, true, false, 0.0);
            }
            
            if (isJumpButtonPressedForPlayer(gjbgl->m_uiLayer, true)) {
                gjbgl->queueButton(1, true, true, 0.0);
            }
        }
    }
};

class $modify(PracticeFixGJBaseGameLayer, GJBaseGameLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto& hack = gui.getWindow("Level").findHackByName("Practice Fix");        
        
        hack.addHookPtr(self.getHook("GJBaseGameLayer::handleButton").unwrap());
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        bool swapControls = GameManager::get()->getGameVariable("0010");

        if (swapControls) {
            isPlayer1 = !isPlayer1;
        }

        int index = button - 1 + (isPlayer1 ? 0 : 3);
        m_clickReleased[index] = !down;
    }
};