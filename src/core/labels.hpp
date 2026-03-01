#pragma once

#include <Geode/Geode.hpp>

namespace GDH {
    namespace Labels {
        enum Corner {
            Top_Left,
            Top_Center,
            Top_Right,
            Center_Left,
            Center_Center,
            Center_Right,
            Bottom_Left,
            Bottom_Center,
            Bottom_Right,
        };

        enum LabelType {
            Text,
            Spacing,
            // Cheat indicator maybe?
        };

        class Label {
        public:
            bool enabled;
            LabelType type;
            std::string text;
            float color[4];
            float size;
            bool rainbow;

            Label(std::string text, float *color, float size = 0.3f, bool rainbow = false) {
                this->enabled = true;
                this->type = LabelType::Text;
                this->text = text;
                std::memcpy(this->color, color, sizeof(float) * 4);
                this->size = size;
                this->rainbow = rainbow;
            }
            
            Label(float spacing) {
                this->enabled = true;
                this->type = LabelType::Spacing;
                this->text = "";
                this->color[0] = 1.0f;
                this->color[1] = 1.0f;
                this->color[2] = 1.0f;
                this->color[3] = 1.0f;
                this->size = spacing;
                this->rainbow = false;
            }
            
            std::string render(void) const;
        };

        extern float cornerPadding, midPadding;
        extern std::unordered_map<Corner, std::vector<Label>> labels;
    }
} // namespace GDH
