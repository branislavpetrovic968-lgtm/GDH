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

std::string Window::formatID(const std::string& windowName, const std::string& hackName) {
    std::string config = fmt::format("{}.{}",
        Utils::String::replaceChar(
            Utils::String::toLowerCase(windowName),
            ' ', '_'
        ),
        Utils::String::replaceChar(
            Utils::String::toLowerCase(hackName),
            ' ', '_'
        )
    );

    return config;
}

Hack& Window::createHack(
    const std::string& name,
    const std::string& desc,
    bool cheating
) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&name](const Hack& h) {
            return h.getName() == name;
        }
    );

    if (it != m_hacks.end()) {
        it->setDesc(desc);
        it->setCheating(cheating);

        return *it;
    }
        
    std::string config = formatID(this->m_name, name);
    return m_hacks.emplace_back(config, name, desc, cheating);
}

Hack& Window::findHackByName(const std::string& name) {
    auto it = std::find_if(
        m_hacks.begin(),
        m_hacks.end(),
        [&name](const Hack& h) {
            return h.getName() == name;
        }
    );

    if (it != m_hacks.end())
        return *it;

    std::string config = formatID(this->m_name, name);    
    return m_hacks.emplace_back(config, name, "", false);
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

Hack* Gui::findHackByIDGlobal(const std::string& ID) {
    for (auto& window : m_windows) {
        auto& hacks = window.getHacks();
        auto it = std::find_if(hacks.begin(), hacks.end(), [&ID](const Hack& h) {
            return h.getID() == ID;
        });

        if (it != hacks.end()) {
            return &(*it);
        }
    }
    return nullptr;
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(LateHacksInitMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
		static bool inited = false;
        if (!inited) {
            auto& gui = Gui::get();
            auto& windows = gui.getWindows();

            for (auto& window : windows)
                for (auto& hack : window.getHacks())
                    if (hack.getEnabled() && !hack.getEarlyInit())
                        hack.callHandler(true);
        }

		return true;
    }
};