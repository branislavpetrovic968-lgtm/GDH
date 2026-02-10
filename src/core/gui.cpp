#include "gui.hpp"
#include "utils.hpp"

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
    const std::string& name,
    const std::string& desc,
    bool cheating
) {
    std::string config = fmt::format("{}.{}",
        Utils::String::replaceChar(
            Utils::String::toLowerCase(this->m_name),
            ' ', '_'
        ),
        Utils::String::replaceChar(
            Utils::String::toLowerCase(name),
            ' ', '_'
        )
    );

    geode::log::debug("Initalized - {}", config);

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

Hack* Window::findHackByName(const std::string& name) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&name](const Hack& h) {
            return h.getName() == name;
        }
    );
    
    return it != m_hacks.end() ? &(*it) : nullptr;
}

Hack* Window::findHackByID(const std::string& ID) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&ID](const Hack& h) {
            return h.getID() == ID;
        }
    );
    
    return it != m_hacks.end() ? &(*it) : nullptr;
}