#include "gui.hpp"

using namespace GDH;

Window& Gui::getWindow(const std::string& name) {
    auto it = std::find_if(
        m_windows.begin(),
        m_windows.end(),
        [&name](const Window& w) {
            return w.getName() == name;
        }
    );

    if (it != m_windows.end())
        return *it;

    return m_windows.emplace_back(name);
}

Hack& Window::createHack(
    const std::string& config,
    const std::string& name,
    const std::string& desc,
    bool cheating
) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&config](const Hack& h) {
            return h.getID() == config;
        }
    );

    if (it != m_hacks.end())
        return *it;
    
    return m_hacks.emplace_back(config, name, desc, cheating);
}

Hack* Window::findHack(const std::string& config) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&config](const Hack& h) {
            return h.getID() == config;
        }
    );
    
    return it != m_hacks.end() ? &(*it) : nullptr;
}