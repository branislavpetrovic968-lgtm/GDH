#pragma once
#include <Geode/Geode.hpp>

enum state { disable, record, play };

struct physic_data
{
    uint64_t frame;
    float x;
    float y;
    double y_accel;
    bool isPlayer2;
};

struct input_data
{
    uint64_t frame;
    bool down;
    int button;
    bool isPlayer2;
};
    
namespace GDH {
    class ReplayEngine {
    public:
        static ReplayEngine& get() {
            static ReplayEngine instance; 
            return instance;
        }

        ReplayEngine& operator=(const ReplayEngine&) = delete; 
        ReplayEngine(const ReplayEngine&) = delete;

        state mode = state::disable;
        std::string replay_name;

        bool accuracy_fix = true;

        uint64_t get_frame();
        void remove_actions(uint64_t currentFrame);

        size_t get_actions_size();
        size_t get_current_index();

        std::string save(const std::string& replay_name);
        std::string load(const std::string& replay_name);
        void clear();

        void handle_update(GJBaseGameLayer* self);
        void handle_commands(GJBaseGameLayer* self);
        void handle_reset();

        void handle_button(bool down, int button, bool isPlayer2);
    private:
        ReplayEngine() = default;

        std::vector<physic_data> m_physicFrames;
        size_t m_physicIndex = 0;

        std::vector<input_data> m_inputFrames;
        size_t m_inputIndex = 0;
    };
}