#pragma once
#include "hacks.hpp"

namespace GDH {
    class Window {
    public:
        explicit Window(std::string name)
            : m_name(std::move(name)) {}

        const std::string& getName() const { return m_name; }

        Hack& createHack(const std::string& name, const std::string& desc, bool cheating);

        Hack& findHackByName(const std::string& name);
        Hack* findHackByID(const std::string& ID);

        std::vector<Hack>& getHacks() { return m_hacks; }

        void setCustomWindowImGui(std::function<void()> func) { m_handlerImGui = std::move(func); };
        void setCustomWindowCocos(std::function<void()> func) { m_handlerCocos = std::move(func); };

        void callCustomWindowImGui() { if (m_handlerImGui) m_handlerImGui(); };
        void callCustomWindowCocos() { if (m_handlerCocos) m_handlerCocos(); };
        
        bool avaibleCustomWindowImGui() { return m_handlerImGui != nullptr; };
        bool avaibleCustomWindowCocos() { return m_handlerCocos != nullptr;  };

        std::string formatID(const std::string& windowName, const std::string& hackName);
    private:
        std::string m_name;
        std::vector<Hack> m_hacks;

        std::function<void()> m_handlerImGui = nullptr;
        std::function<void()> m_handlerCocos = nullptr;
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

#define GUI_HACK_CREATE(window_name, name, desc, cheating) \
$execute \
{ \
    auto& win = GDH::Gui::get().getWindow(window_name); \
    win.createHack(name, desc, cheating); \
}