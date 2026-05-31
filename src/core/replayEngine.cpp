#include "replayEngine.hpp"
#include "config.hpp"
#include <Geode/binding/GJBaseGameLayer.hpp>

using namespace GDH;

uint64_t ReplayEngine::get_frame() {
    auto gjbgl = GJBaseGameLayer::get();
    if (gjbgl)
        return static_cast<uint64_t>(gjbgl->m_gameState.m_levelTime * 240.f);
    return 0;
}

void ReplayEngine::remove_actions(uint64_t currentFrame) {
    // clearing physic frames
    std::erase_if(m_physicFrames, [currentFrame](const physic_data &action) {
        return action.frame >= currentFrame;
    });

    // clearing inputs and auto-releasing it
    std::erase_if(m_inputFrames, [currentFrame](const input_data &action) {
        return action.frame >= currentFrame;
    });

    std::array<bool, 6> released = {true, true, true, true, true, true};
    std::array<bool, 6> checked = {false, false, false, false, false, false};

    for (auto it = m_inputFrames.rbegin(); it != m_inputFrames.rend(); ++it) {
        int button_index = it->button - 1 + (it->isPlayer2 ? 3 : 0);
        if (!checked[button_index]) {
            checked[button_index] = true;
            released[button_index] = !it->down;
            
            if (std::all_of(checked.begin(), checked.end(), [](bool v) { return v; }))
                break;
        }
    }

    if (auto gjbgl = GJBaseGameLayer::get()) {
        gjbgl->m_queuedButtons.clear();
        
        bool swapControls = GameManager::get()->getGameVariable("0010");
        
        for (int i = 0; i < 3; ++i) {
            if (!released[i]) {
                gjbgl->queueButton(i + 1, false, swapControls, 0.0);
            }
            if (!released[i + 3]) {
                gjbgl->queueButton(i + 1, false, !swapControls, 0.0);
            }
        }
        
        auto isJumpButtonPressedForPlayer = [](UILayer* ui, bool player1) -> bool {
            if (player1) {
                return (ui->m_p1TouchId != -1 || ui->m_p1Jumping);
            } else {
                return (ui->m_p2TouchId != -1 || ui->m_p2Jumping);
            }
        };

        if (isJumpButtonPressedForPlayer(gjbgl->m_uiLayer, true)) {
            gjbgl->queueButton(1, true, false, 0.0);
        }

        if (isJumpButtonPressedForPlayer(gjbgl->m_uiLayer, false)) {
            gjbgl->queueButton(1, true, true, 0.0);
        }
    }
}

void ReplayEngine::handle_update(GJBaseGameLayer* self) {  
    auto& config = Config::get();
    if (config.get<bool>("engine::accuracy_fix", true)) return;

    auto frame = get_frame();

    if (mode == state::record) {
        bool frameExist = !m_physicFrames.empty() && m_physicFrames.back().frame == frame;
        if (!frameExist)
            m_physicFrames.push_back({
                frame, 
                self->m_player1->m_position.x,
                self->m_player1->m_position.y,
                self->m_player1->m_yVelocity,
                false //first player
            });

        if (!self->m_gameState.m_isDualMode)
            return;
        
        if (!frameExist)
            m_physicFrames.push_back({
                frame, 
                self->m_player2->m_position.x,
                self->m_player2->m_position.y,
                self->m_player2->m_yVelocity,
                true //second player
            });
    }
    else if (mode == state::play) {
        while (m_physicIndex < m_physicFrames.size() && frame >= m_physicFrames[m_physicIndex].frame)
        {
            const auto& physicFrame = m_physicFrames[m_physicIndex];
            auto* player = physicFrame.isPlayer2 ? self->m_player2 : self->m_player1;

            player->m_position.x = physicFrame.x;
            player->m_position.y = physicFrame.y;
            player->m_yVelocity = physicFrame.y_accel;

            m_physicIndex++;
        }
    }
}

void ReplayEngine::handle_commands(GJBaseGameLayer* self) {
    if (mode != state::play) return;
    
    auto frame = get_frame();
    
    while (m_inputIndex < m_inputFrames.size() && frame >= m_inputFrames[m_inputIndex].frame)
    {
        bool isPlayer2 = m_inputFrames[m_inputIndex].isPlayer2;
        bool isPlayer1 = !isPlayer2;

        bool swapControls = GameManager::get()->getGameVariable("0010");

        if (swapControls) {
            isPlayer1 = !isPlayer1;
        }

        self->handleButton(m_inputFrames[m_inputIndex].down, m_inputFrames[m_inputIndex].button, isPlayer1);
        m_inputIndex++;
    }
}

void ReplayEngine::handle_reset() {
    if (mode == state::record) {
        int lastCheckpointFrame = get_frame();
        remove_actions(lastCheckpointFrame);
    }
    else if (mode == state::play) {
        m_physicIndex = 0;
        m_inputIndex = 0;
    }
}

void ReplayEngine::handle_button(bool down, int button, bool isPlayer1) {
    if (mode != state::record)
        return;

    bool swapControls = GameManager::get()->getGameVariable("0010");

    if (swapControls) {
        isPlayer1 = !isPlayer1;
    }

    auto frame = get_frame();
    bool isPlayer2 = !isPlayer1;

    m_inputFrames.push_back({frame, down, button, isPlayer2});
}

size_t ReplayEngine::get_actions_size() {
    return m_inputFrames.size();
}

size_t ReplayEngine::get_current_index() {
    return m_inputIndex;
}

std::string ReplayEngine::save(const std::string& replay_name) {
    if (m_inputFrames.empty())
        return "Replay doesn't have actions";

    std::ofstream out(folderMacroPath / std::string(replay_name + ".re4"), std::ios::binary);
    if (!out.is_open()) return "Failed to save Replay";

    out.write("RE4", 3);
    float fps = 240.f;
    out.write(reinterpret_cast<const char*>(&fps), sizeof(fps));

    uint64_t physicSize = m_physicFrames.size();
    out.write(reinterpret_cast<const char*>(&physicSize), sizeof(physicSize));
    
    for (const auto& p : m_physicFrames) {
        out.write(reinterpret_cast<const char*>(&p.frame), sizeof(p.frame));
        out.write(reinterpret_cast<const char*>(&p.x), sizeof(p.x));
        out.write(reinterpret_cast<const char*>(&p.y), sizeof(p.y));
        out.write(reinterpret_cast<const char*>(&p.y_accel), sizeof(p.y_accel));
        
        uint8_t p2 = p.isPlayer2 ? 1 : 0;
        out.write(reinterpret_cast<const char*>(&p2), 1);
    }

    uint64_t inputSize = m_inputFrames.size();
    out.write(reinterpret_cast<const char*>(&inputSize), sizeof(inputSize));

    for (const auto& i : m_inputFrames) {
        out.write(reinterpret_cast<const char*>(&i.frame), sizeof(i.frame));
        
        uint8_t down = i.down ? 1 : 0;
        out.write(reinterpret_cast<const char*>(&down), 1);
        
        int32_t btn = static_cast<int32_t>(i.button);
        out.write(reinterpret_cast<const char*>(&btn), sizeof(btn));
        
        uint8_t p2 = i.isPlayer2 ? 1 : 0;
        out.write(reinterpret_cast<const char*>(&p2), 1);
    }

    return "Replay Saved";
}

std::string ReplayEngine::load(const std::string& replay_name) {
    std::ifstream in(folderMacroPath / std::string(replay_name + ".re4"), std::ios::binary);
    if (!in.is_open()) return "Failed to open Replay";

    m_physicFrames.clear();
    m_inputFrames.clear();

    char header[3];
    in.read(header, 3);
    if (header[0] != 'R' || header[1] != 'E' || header[2] != '4') {
        return "Invalid Replay format";
    }

    float fps;
    in.read(reinterpret_cast<char*>(&fps), sizeof(fps));

    uint64_t physicSize = 0;
    in.read(reinterpret_cast<char*>(&physicSize), sizeof(physicSize));
    m_physicFrames.resize(physicSize);

    for (auto& p : m_physicFrames) {
        in.read(reinterpret_cast<char*>(&p.frame), sizeof(p.frame));
        in.read(reinterpret_cast<char*>(&p.x), sizeof(p.x));
        in.read(reinterpret_cast<char*>(&p.y), sizeof(p.y));
        in.read(reinterpret_cast<char*>(&p.y_accel), sizeof(p.y_accel));
        
        uint8_t p2 = 0;
        in.read(reinterpret_cast<char*>(&p2), 1);
        p.isPlayer2 = (p2 != 0);
    }

    uint64_t inputSize = 0;
    in.read(reinterpret_cast<char*>(&inputSize), sizeof(inputSize));
    m_inputFrames.resize(inputSize);

    for (auto& i : m_inputFrames) {
        in.read(reinterpret_cast<char*>(&i.frame), sizeof(i.frame));
        
        uint8_t down = 0;
        in.read(reinterpret_cast<char*>(&down), 1);
        i.down = (down != 0);
        
        int32_t btn = 0;
        in.read(reinterpret_cast<char*>(&btn), sizeof(btn));
        i.button = static_cast<int>(btn);
        
        uint8_t p2 = 0;
        in.read(reinterpret_cast<char*>(&p2), 1);
        i.isPlayer2 = (p2 != 0);
    }

    return "Replay Loaded";
}

void ReplayEngine::clear() {
    m_physicFrames.clear();
    m_inputFrames.clear();
}