
#include "labels.hpp"
#include "utils.hpp"

#include <Geode/modify/PlayLayer.hpp>

using namespace GDH::Labels;

float GDH::Labels::cornerPadding = 4.0f;
float GDH::Labels::midPadding = 2.0f;
std::unordered_map<Corner, std::vector<Label>> GDH::Labels::labels;

std::chrono::steady_clock::time_point g_sessionStart;
float g_rainbowHue = 0.0f;

void renderVariable(geode::utils::StringBuffer<> &stream, std::string_view var_name) {
    PlayLayer *pl = PlayLayer::get();
    if (var_name == "TIME_24") {
        stream.append(fmt::format("{:%T}", geode::localtime(std::time(nullptr)), var_name));
    } else if (var_name == "TIME_12") {
        stream.append(fmt::format("{:%I:%M:%S %p}", geode::localtime(std::time(nullptr)), var_name));
    } else if (var_name == "DATE") {
        stream.append(fmt::format("{:%F}", geode::localtime(std::time(nullptr)), var_name));
    } else if (var_name == "SESSION_TIME") {
        auto dur = std::chrono::steady_clock::now() - g_sessionStart;
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur); secs %= 60;
        auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs); mins %= 60;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
        stream.append(fmt::format("{:02}:{:02}:{:02}", hours.count(), mins.count(), secs.count()));
    } else if (var_name == "PROGRESS" || var_name.starts_with("PROGRESS:")) {
        var_name.remove_prefix(8);
        int decimal_points = 0;
        if (var_name.size()) {
            var_name.remove_prefix(1);
            auto parsed = geode::utils::numFromString<int>(var_name, 10);
            if (parsed.ok()) decimal_points = *parsed;
        }
        stream.append(fmt::format("{:.{}f}", (pl ? pl->getCurrentPercent() : 0.0f), decimal_points));
    } else if (var_name == "FPS" || var_name.starts_with("FPS:")) {
        var_name.remove_prefix(3);
        int decimal_points = 0;
        if (var_name.size()) {
            var_name.remove_prefix(1);
            auto parsed = geode::utils::numFromString<int>(var_name, 10);
            if (parsed.ok()) decimal_points = *parsed;
        }
        stream.append(fmt::format("{:.{}f}", GDH::Utils::getFps(), decimal_points));
    } else {
        stream.append("<nil>");
    }
}

std::string GDH::Labels::Label::render() const {
    geode::utils::StringBuffer<> stream;
    std::string_view raw = text;
    int r = 255, g = 255, b = 255;
    while (raw.size()) {
        if (raw[0] == '$') {
            raw.remove_prefix(1);
            if (raw[0] == '{') {
                std::string_view::size_type ccurly = raw.find("}");
                if (ccurly == std::string_view::npos) {
                    stream.append('$'); continue;
                }
                std::string_view var = raw.substr(1, ccurly - 1);
                raw.remove_prefix(ccurly + 1);
                renderVariable(stream, var);
            } else stream.append('$');
        } else {
            stream.append(raw[0]);
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
