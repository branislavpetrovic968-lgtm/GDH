
#include "labels.hpp"
#include "utils.hpp"
#include "config.hpp"

#include <string_view>
#include <json.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace GDH::Labels;

float GDH::Labels::cornerPadding = 4.0f;
float GDH::Labels::midPadding = 2.0f;
std::unordered_map<Corner, std::vector<Label>> GDH::Labels::labels;

std::chrono::steady_clock::time_point g_sessionStart;
float g_rainbowHue = 0.0f;

$execute {
    load();
}

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

void GDH::Labels::save(void) {
    nlohmann::json obj;
    obj["cornerPadding"] = cornerPadding;
    obj["midPadding"] = midPadding;
    for (const auto &[corner, labelsCorner] : labels) {
        std::string cornerIndice = corner == Corner::Top_Left ? "labelsTL" :
            corner == Corner::Top_Center ? "labelsTC" :
            corner == Corner::Top_Right ? "labelsTR" :
            corner == Corner::Center_Left ? "labelsCL" :
            corner == Corner::Center_Center ? "labelsCC" :
            corner == Corner::Center_Right ? "labelsCR" :
            corner == Corner::Bottom_Left ? "labelsBL" :
            corner == Corner::Bottom_Center ? "labelsBC" : "labelsBR";
        for (const auto &label : labelsCorner) {
            nlohmann::json labelObj;
            labelObj["enabled"] = label.enabled;
            labelObj["type"] = label.type;
            switch (label.type) {
                case LabelType::Text: {
                    labelObj["text"] = label.text;
                    labelObj["color"] = { label.color[0], label.color[1], label.color[2], label.color[3] };
                    labelObj["rainbow"] = label.rainbow;
                    labelObj["size"] = label.size;
                } break;
                case LabelType::Spacing: {
                    labelObj["spacing"] = label.size;
                } break;
            }
            obj[cornerIndice].push_back(labelObj);
        }
    }
    std::ofstream outFile(labelsDataPath);
    if (outFile.is_open()) {
        outFile << obj.dump(4);
        outFile.close();
    }
}

void GDH::Labels::load(void) {
    std::ifstream ifs(labelsDataPath);
    if (!ifs.is_open()) return;

    labels[Corner::Top_Left] = std::vector<Label> {};
    labels[Corner::Top_Center] = std::vector<Label> {};
    labels[Corner::Top_Right] = std::vector<Label> {};
    labels[Corner::Center_Left] = std::vector<Label> {};
    labels[Corner::Center_Center] = std::vector<Label> {};
    labels[Corner::Center_Right] = std::vector<Label> {};
    labels[Corner::Bottom_Left] = std::vector<Label> {};
    labels[Corner::Bottom_Center] = std::vector<Label> {};
    labels[Corner::Bottom_Right] = std::vector<Label> {};
    
    nlohmann::json obj = nlohmann::json::parse(ifs);
    ifs.close();

    cornerPadding = obj["cornerPadding"];
    midPadding = obj["midPadding"];

    for (auto &[corner, labelsCorner] : labels) {
        std::string cornerIndice = corner == Corner::Top_Left ? "labelsTL" :
            corner == Corner::Top_Center ? "labelsTC" :
            corner == Corner::Top_Right ? "labelsTR" :
            corner == Corner::Center_Left ? "labelsCL" :
            corner == Corner::Center_Center ? "labelsCC" :
            corner == Corner::Center_Right ? "labelsCR" :
            corner == Corner::Bottom_Left ? "labelsBL" :
            corner == Corner::Bottom_Center ? "labelsBC" : "labelsBR";

        for (const auto &i : obj[cornerIndice]) {
            switch (i["type"].get<LabelType>()) {
            case LabelType::Text: {
                float color[4] = { i["color"][0], i["color"][1], i["color"][2], i["color"][3] };
                labelsCorner.push_back(Label(i["text"], color, i["size"], i["rainbow"]));
            } break;
            case LabelType::Spacing: {
                labelsCorner.push_back(Label(i["spacing"]));
            } break;
            }
        }
    }
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
