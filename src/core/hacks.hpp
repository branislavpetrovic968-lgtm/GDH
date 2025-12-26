#pragma once
#include <string>
#include <functional>
#include <unordered_set>
#include <vector>

#include "config.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace GDH {
    class Hack {
    public:
        Hack() = default;

        Hack(std::string id, std::string name, std::string desc, bool cheating)
            : m_ID(std::move(id)),
            m_name(std::move(name)),
            m_desc(std::move(desc)),
            m_cheating(cheating) {}

        void setID(const std::string& id) { m_ID = id; }
        void setName(const std::string& n) { m_name = n; }
        void setDesc(const std::string& d) { m_desc = d; }
        void setCheating(bool value) { m_cheating = value; }
        void setHandler(std::function<void(bool)> func) { m_handlerFunc = std::move(func); }
        void addHookPtr(geode::Hook* ptr);
        void setKeybind(int key) { m_keybind = key; }

        const std::string& getID() const { return m_ID; }
        const std::string& getName() const { return m_name; }
        const std::string& getDesc() const { return m_desc; }
        bool isCheating() const { return m_cheating; }
        int getKeybind() const { return m_keybind; }

        void callHandler(bool state);
        void enableHooks(bool state);

    private:
        std::string m_ID;
        std::string m_name;
        std::string m_desc;
        bool m_cheating = false;

        std::function<void(bool)> m_handlerFunc = nullptr;
        std::unordered_set<geode::Hook*> m_hooksPtr;
        int m_keybind = 0;
    };
} // namespace GDH