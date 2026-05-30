#pragma once

#include <Geode/Geode.hpp>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>

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
        };

        class Label {
        public:
            bool enabled;
            LabelType type;
            std::string text;
            std::array<float, 4> color;
            float size;
            
            bool rainbow;
            bool cps;
            bool noclip;

            Label(std::string text, std::array<float, 4> color, float size = 0.3f, bool rainbow = false, bool cps = false, bool noclip = false) {
                this->enabled = true;
                this->type = LabelType::Text;
                this->text = text;
                this->color = color;
                this->size = size;
                this->rainbow = rainbow;
                this->cps = cps;
                this->noclip = noclip;
            }
            
            Label(float spacing) {
                this->enabled = true;
                this->type = LabelType::Spacing;
                this->text = "";
                this->color = {1.0f, 1.0f, 1.0f, 1.0f};
                this->size = spacing;
                this->rainbow = false;
                this->cps = false;
                this->noclip = false;
            }

            std::string render() const;
        };

        extern float cornerPadding, midPadding;
        extern std::unordered_map<Corner, std::vector<Label>> labels;

        void save(void);
        void load(void);
    } // namespace Labels
} // namespace GDH