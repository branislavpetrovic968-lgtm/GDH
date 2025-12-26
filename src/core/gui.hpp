#pragma once
#include "hacks.hpp"
#include "config.hpp"

namespace GDH {
    class Window {
    public:
        explicit Window(std::string name)
            : m_name(std::move(name)) {}

        const std::string& getName() const { return m_name; }

        Hack& createHack(const std::string& config, const std::string& name,
            const std::string& desc, bool cheating);

        Hack* findHack(const std::string& config);

        std::vector<Hack>& getHacks() { return m_hacks; }
    private:
        std::string m_name;
        std::vector<Hack> m_hacks;
    };

    class Gui {
    public:
        static Gui& get() {
            static Gui instance;
            return instance;
        }

        Gui& operator=(const Gui&) = delete;
        Gui(const Gui&) = delete;

        Window& getWindow(const std::string& name);
        std::vector<Window>& getWindows() { return m_windows; };
    private:
        Gui() = default;

        std::vector<Window> m_windows;
    };
} // namespace GDH

#define GUI_HACK_CREATE(window_name, config, name, desc, cheating) \
$execute \
{ \
    auto& win = GDH::Gui::get().getWindow(window_name); \
    win.createHack(config, name, desc, cheating); \
}