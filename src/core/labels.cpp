
#include "labels.hpp"

#include <string_view>
#include <sstream>

#include <Geode/modify/PlayLayer.hpp>

using namespace GDH::Labels;

float GDH::Labels::cornerPadding = 4.0f;
float GDH::Labels::midPadding = 2.0f;
std::unordered_map<Corner, std::vector<Label>> GDH::Labels::labels;

void renderVariable(std::stringstream &stream, std::string_view var_name) {
    if (var_name == "TIME_24") {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        stream << std::put_time(std::localtime(&nowTime), "%T");
    } else {
        stream << "<nil>";
    }
}

std::string GDH::Labels::Label::render(void) const {
    std::string_view raw = text;
    std::stringstream stream;
    int r = 255, g = 255, b = 255;
    while (raw.size()) {
        if (raw[0] == '$') {
            raw.remove_prefix(1);
            if (raw[0] == '{') {
                std::string_view::size_type ccurly = raw.find("}");
                if (ccurly == std::string_view::npos) {
                    stream << '$'; continue;
                }
                std::string_view var = raw.substr(1, ccurly - 1);
                raw.remove_prefix(ccurly + 1);
                renderVariable(stream, var);
            } else stream << '$';
        } else {
            stream << raw[0];
            raw.remove_prefix(1);
        }
    }
    return stream.str();
}

class $modify(LabelsPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<Corner, std::vector<cocos2d::CCLabelBMFont *>> labelObjs;
    };

    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        m_fields->labelObjs[Corner::Top_Left] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Top_Center] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Top_Right] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Left] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Center] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Right] = std::vector<cocos2d::CCLabelBMFont *>();
        return true;
    }
    
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        
        for (auto &[corner, labelVec] : m_fields->labelObjs) {
            float y = 0.0f; int i = 0;
            for (const auto &labelObj : labels[corner]) {
                if (!labelObj.enabled) continue;
                cocos2d::CCLabelBMFont *label = NULL;
                if (i < labelVec.size()) label = labelVec[i];
                else {
                    label = cocos2d::CCLabelBMFont::create("Swag", "bigFont.fnt");
                    switch (corner) {
                    case Corner::Top_Left: label->setAnchorPoint({0.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft); break;
                    case Corner::Top_Center: label->setAnchorPoint({0.5f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter); break;
                    case Corner::Top_Right: label->setAnchorPoint({1.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight); break;
                    case Corner::Bottom_Left: label->setAnchorPoint({0.0f, 0.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft); break;
                    case Corner::Bottom_Center: label->setAnchorPoint({0.5f, 0.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter); break;
                    case Corner::Bottom_Right: label->setAnchorPoint({1.0f, 0.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight); break;
                    }
                    addChild(label, 999);
                    labelVec.push_back(label);
                }

                i++;
                
                label->setScale(labelObj.size);
                label->setColor(cocos2d::ccColor3B { static_cast<GLubyte>(labelObj.color[0] * 255), static_cast<GLubyte>(labelObj.color[1] * 255), static_cast<GLubyte>(labelObj.color[2] * 255) });
                label->setOpacity(static_cast<GLubyte>(labelObj.color[3] * 255));
                label->setCString(labelObj.render().c_str());
               
                switch (corner) {
                case Corner::Top_Left: label->setPosition({cornerPadding, windowSize.height - cornerPadding - y}); break;
                case Corner::Top_Center: label->setPosition({windowSize.width / 2.0f, windowSize.height - cornerPadding - y}); break;
                case Corner::Top_Right: label->setPosition({windowSize.width - cornerPadding, windowSize.height - cornerPadding - y}); break;
                case Corner::Bottom_Left: label->setPosition({cornerPadding, cornerPadding + y}); break;
                case Corner::Bottom_Center: label->setPosition({windowSize.width / 2.0f, cornerPadding + y}); break;
                case Corner::Bottom_Right: label->setPosition({windowSize.width - cornerPadding, cornerPadding + y}); break;
                }

                y += label->getContentHeight() * labelObj.size + midPadding;
            }
            for (; i < labelVec.size(); i++) {
                labelVec[i]->setOpacity(0);
            }
        }
    }
};
