
#include "labels.hpp"
#include "utils.hpp"

#include <string_view>
#include <sstream>

#include <Geode/modify/PlayLayer.hpp>

using namespace GDH::Labels;

float GDH::Labels::cornerPadding = 4.0f;
float GDH::Labels::midPadding = 2.0f;
std::unordered_map<Corner, std::vector<Label>> GDH::Labels::labels;

std::chrono::steady_clock::time_point g_sessionStart;
float g_rainbowHue = 0.0f;

void renderVariable(std::stringstream &stream, std::string_view var_name) {
    PlayLayer *pl = PlayLayer::get();
    cocos2d::CCDirector *dr = cocos2d::CCDirector::get();
    if (var_name.starts_with("TIME:")) {
        var_name.remove_prefix(5);
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        stream << std::put_time(std::localtime(&nowTime), std::string(var_name).c_str());
    } else if (var_name == "SESSION_TIME") {
        auto dur = std::chrono::steady_clock::now() - g_sessionStart;
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur); secs %= 60;
        auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs); mins %= 60;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
        stream << std::format("{:02}:{:02}:{:02}", hours.count(), mins.count(), secs.count());
    } else if (var_name == "PROGRESS" || var_name.starts_with("PROGRESS:")) {
        var_name.remove_prefix(8);
        int decimal_points = 0;
        if (var_name.size()) {
            var_name.remove_prefix(1);
            std::from_chars(var_name.data(), var_name.data() + var_name.size(), decimal_points);
        }
        stream << std::fixed << std::setprecision(decimal_points) << (pl ? pl->getCurrentPercent() : 0.0f);
    } else if (var_name == "FPS") {
        stream << std::fixed << std::setprecision(0) << (dr ? 1.0f / dr->getDeltaTime() : 0.0f);
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
        m_fields->labelObjs[Corner::Center_Left] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Center_Center] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Center_Right] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Left] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Center] = std::vector<cocos2d::CCLabelBMFont *>();
        m_fields->labelObjs[Corner::Bottom_Right] = std::vector<cocos2d::CCLabelBMFont *>();
        g_sessionStart = std::chrono::steady_clock::now();
        return true;
    }
    
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        g_rainbowHue += 0.125f * dt;
        if (g_rainbowHue > 1.0f) g_rainbowHue -= 1.0f;
        
        for (auto &[corner, labelVec] : m_fields->labelObjs) {
            float y = 0.0f, labelsHeight = 0.0f; int i = 0;
            for (const auto &labelObj : labels[corner]) {
                if (!labelObj.enabled) continue;
                if (labelObj.type == LabelType::Text) {
                    cocos2d::CCLabelBMFont *label = NULL;
                    if (i < labelVec.size()) label = labelVec[i];
                    else {
                        label = cocos2d::CCLabelBMFont::create("Swag", "bigFont.fnt");
                        switch (corner) {
                        case Corner::Top_Left: label->setAnchorPoint({0.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft); break;
                        case Corner::Top_Center: label->setAnchorPoint({0.5f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter); break;
                        case Corner::Top_Right: label->setAnchorPoint({1.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight); break;
                        case Corner::Center_Left: label->setAnchorPoint({0.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft); break;
                        case Corner::Center_Center: label->setAnchorPoint({0.5f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter); break;
                        case Corner::Center_Right: label->setAnchorPoint({1.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight); break;
                        case Corner::Bottom_Left: label->setAnchorPoint({0.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft); break;
                        case Corner::Bottom_Center: label->setAnchorPoint({0.5f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentCenter); break;
                        case Corner::Bottom_Right: label->setAnchorPoint({1.0f, 1.0f}); label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight); break;
                        }
                        addChild(label, 999);
                        labelVec.push_back(label);
                    }

                    i++;
                
                    label->setScale(labelObj.size);
                    if (labelObj.rainbow) {
                        float r, g, b; GDH::Utils::hsvToRgb(g_rainbowHue, 1.0f, 1.0f, r, g, b);
                        label->setColor(cocos2d::ccColor3B { static_cast<GLubyte>(r * 255), static_cast<GLubyte>(g * 255), static_cast<GLubyte>(b * 255) });
                    } else label->setColor(cocos2d::ccColor3B { static_cast<GLubyte>(labelObj.color[0] * 255), static_cast<GLubyte>(labelObj.color[1] * 255), static_cast<GLubyte>(labelObj.color[2] * 255) });
                    label->setOpacity(static_cast<GLubyte>(labelObj.color[3] * 255));
                    label->setCString(labelObj.render().c_str());

                    y += label->getContentHeight() * labelObj.size + midPadding;
                } else if (labelObj.type == LabelType::Spacing) {
                    y += labelObj.size + midPadding;
                }
            }
            for (; i < labelVec.size(); i++) {
                labelVec[i]->setOpacity(0);
            }
            labelsHeight = y - midPadding; y = 0.0f; i = 0;
            for (const auto &labelObj : labels[corner]) {
                if (!labelObj.enabled) continue;
                if (labelObj.type == LabelType::Text) {
                    cocos2d::CCLabelBMFont *label = labelVec[i++];
                    switch (corner) {
                    case Corner::Top_Left: label->setPosition({cornerPadding, windowSize.height - cornerPadding - y}); break;
                    case Corner::Top_Center: label->setPosition({windowSize.width / 2.0f, windowSize.height - cornerPadding - y}); break;
                    case Corner::Top_Right: label->setPosition({windowSize.width - cornerPadding, windowSize.height - cornerPadding - y}); break;
                    case Corner::Center_Left: label->setPosition({cornerPadding, windowSize.height / 2.0f + labelsHeight / 2.0f - y}); break;
                    case Corner::Center_Center: label->setPosition({windowSize.width / 2.0f, windowSize.height / 2.0f + labelsHeight / 2.0f - y}); break;
                    case Corner::Center_Right: label->setPosition({windowSize.width - cornerPadding, windowSize.height / 2.0f + labelsHeight / 2.0f - y}); break;
                    case Corner::Bottom_Left: label->setPosition({cornerPadding, cornerPadding + labelsHeight - y}); break;
                    case Corner::Bottom_Center: label->setPosition({windowSize.width / 2.0f, cornerPadding + labelsHeight - y}); break;
                    case Corner::Bottom_Right: label->setPosition({windowSize.width - cornerPadding, cornerPadding + labelsHeight - y}); break;
                    }
                    y += label->getContentHeight() * labelObj.size + midPadding;
                } else if (labelObj.type == LabelType::Spacing) {
                    y += labelObj.size + midPadding;
                }
            }
        }
    }
};
