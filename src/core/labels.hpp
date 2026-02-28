#pragma once

#include <Geode/Geode.hpp>

namespace GDH {
    namespace Labels {
        enum Corner {
            Top_Left,
            Top_Center,
            Top_Right,
            Bottom_Left,
            Bottom_Center,
            Bottom_Right,
        };

        class Label {
        public:
            std::string text;
            float color[4];
            float size;
            bool enabled;
            bool rainbow;

            std::string render(void) const;
        };

        extern float cornerPadding, midPadding;
        extern std::unordered_map<Corner, std::vector<Label>> labels;
    }
} // namespace GDH
