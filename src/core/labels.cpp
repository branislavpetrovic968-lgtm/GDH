#include "labels.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "replayEngine.hpp"

#include <string_view>
#include <json.hpp>
#include <fstream>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <chrono>
#include <queue>

using namespace GDH::Labels;

float GDH::Labels::cornerPadding = 4.0f;
float GDH::Labels::midPadding = 2.0f;
std::unordered_map<Corner, std::vector<Label>> GDH::Labels::labels;

std::chrono::steady_clock::time_point g_sessionStart;
float g_rainbowHue = 0.0f;

$execute {
    load();
}

class NoclipAccuracy {
public:    
    static NoclipAccuracy& get() {
        static NoclipAccuracy instance;
        return instance;
    }

    NoclipAccuracy& operator=(const NoclipAccuracy&) = delete;
    NoclipAccuracy(const NoclipAccuracy&) = delete;

    int frames = 0, deaths = 0, deaths_full = 0;
    bool wouldDie = false, prevDied = false;

    void handle_update(GJBaseGameLayer* self, float delta) {
        auto pl = PlayLayer::get();
        if (!pl || self->m_player1->m_isDead || pl->m_levelEndAnimationStarted) {
            prevDied = false;
            return;
        }

        frames++;

        if (wouldDie) {
            wouldDie = false;
            deaths++;

            if (!prevDied) {
                deaths_full++;
                prevDied = true;
            }
        } else {
            prevDied = false;
        }
    }

    void handle_reset() {
        frames = 0;
        deaths = 0;
        deaths_full = 0;
        wouldDie = false;
        prevDied = false;
    }

    void handle_death() {
        wouldDie = true;
    }

    float getPercentage() const {
        if (frames == 0) return 100.0f;
        return (1.0f - static_cast<float>(deaths) / frames) * 100.0f;
    }

private:
    NoclipAccuracy() = default;
};

class CpsCounter {
public:
    static CpsCounter& get() {
        static CpsCounter instance;
        return instance;
    }

    CpsCounter(const CpsCounter&) = delete;
    CpsCounter& operator=(const CpsCounter&) = delete;

    std::queue<std::chrono::steady_clock::time_point> clicks;

    int cps = 0;
    int highscore = 0;
    int overall = 0;
    
    void reset() {
        cps = 0;
        highscore = 0;
        overall = 0;
        std::queue<std::chrono::steady_clock::time_point>().swap(clicks);
    }
    
    void click() {
        overall++;
        clicks.push(std::chrono::steady_clock::now());
    }
    
    void update() {
        auto one_second_ago = std::chrono::steady_clock::now() - std::chrono::seconds(1);
        
        while (!clicks.empty() && clicks.front() < one_second_ago) {
            clicks.pop();
        }

        cps = static_cast<int>(clicks.size());

        if (highscore < cps) {
            highscore = cps;
        }
    }
private:
    CpsCounter() = default;
};

bool renderVariable(geode::utils::StringBuffer<> &stream, std::string_view var_name) {
    auto pl = PlayLayer::get();
    if (var_name == "time_24") { stream.append(fmt::format("{:%T}", geode::localtime(std::time(nullptr)), var_name)); }
    else if (var_name == "time_12") { stream.append(fmt::format("{:%I:%M:%S %p}", geode::localtime(std::time(nullptr)), var_name)); }
    else if (var_name == "date") { stream.append(fmt::format("{:%F}", geode::localtime(std::time(nullptr)), var_name)); }
    else if (var_name == "clicks") { stream.append(CpsCounter::get().overall); }
    else if (var_name == "cps") { stream.append(CpsCounter::get().cps); }
    else if (var_name == "cps_high") { stream.append(CpsCounter::get().highscore); }
    else if (var_name == "testmode") { stream.append(pl ? (pl->m_isTestMode ? "Testmode" : "") : ""); }
    else if (var_name == "attempt") { stream.append(pl ? pl->m_attempts : 0); }
    else if (var_name == "level_name") { stream.append(pl ? pl->m_level->m_levelName : ""); }
    else if (var_name == "level_creator") {
        if (pl && pl->m_level) {
            static const std::unordered_set<int> robtop_levels = {
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                5001, 5002, 5003, 5004, 3001
            };
            
            if (robtop_levels.contains(pl->m_level->m_levelID)) { stream.append("RobTop"); } 
            else { stream.append(pl->m_level->m_creatorName); }
        } else {
            stream.append("");
        }
    }
    else if (var_name == "level_id") { stream.append(pl ? pl->m_level->m_levelID : 0); }
    else if (var_name == "normal_percent") { stream.append(pl ? pl->m_level->getNormalPercent() : 0); }
    else if (var_name == "practice_percent") { stream.append(pl ? pl->m_level->m_practicePercent : 0); }
    else if (var_name == "noclip_accuracy") { stream.append(fmt::format("{:.2f}", NoclipAccuracy::get().getPercentage())); }
    else if (var_name == "deaths") { stream.append(NoclipAccuracy::get().deaths_full); }
    else if (var_name == "frame") { stream.append(GDH::ReplayEngine::get().get_frame() ); }
    else if (var_name == "re_state") {
        auto& engine = GDH::ReplayEngine::get();
        stream.append(engine.mode == state::disable ? "Disable" : engine.mode == state::record ? "Record" : "Play");
    }
    else if (var_name == "session_time") {
        auto dur = std::chrono::steady_clock::now() - g_sessionStart;
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur); secs %= 60;
        auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs); mins %= 60;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
        stream.append(fmt::format("{:02}:{:02}:{:02}", hours.count(), mins.count(), secs.count()));
    } else if (var_name == "progress" || var_name.starts_with("progress:")) {
        var_name.remove_prefix(8);
        int decimal_points = 0;
        if (var_name.size()) {
            var_name.remove_prefix(1);
            auto parsed = geode::utils::numFromString<int>(var_name, 10);
            if (parsed.ok()) decimal_points = *parsed;
        }
        stream.append(fmt::format("{:.{}f}", (pl ? pl->getCurrentPercent() : 0.0f), decimal_points));
    } else if (var_name == "fps" || var_name.starts_with("fps:")) {
        var_name.remove_prefix(3);
        int decimal_points = 0;
        if (var_name.size()) {
            var_name.remove_prefix(1);
            auto parsed = geode::utils::numFromString<int>(var_name, 10);
            if (parsed.ok()) decimal_points = *parsed;
        }
        stream.append(fmt::format("{:.{}f}", GDH::Utils::getFps(), decimal_points));
    } else {
        return false;
    }
    return true;
}

std::string GDH::Labels::Label::render() const {
    geode::utils::StringBuffer<> stream;
    std::string_view raw = text;
    
    while (!raw.empty()) {
        if (raw[0] == '{') {
            std::string_view::size_type ccurly = raw.find("}");
            if (ccurly == std::string_view::npos) {
                stream.append('{');
                raw.remove_prefix(1);
                continue;
            }
            
            std::string_view var = raw.substr(1, ccurly - 1);
            
            if (!renderVariable(stream, var)) {
                stream.append('{');
                stream.append(var);
                stream.append('}');
            }
            
            raw.remove_prefix(ccurly + 1);
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
                    labelObj["cps"] = label.cps;
                    labelObj["noclip"] = label.noclip;
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
                std::array<float, 4> color = { i["color"][0], i["color"][1], i["color"][2], i["color"][3] };
                bool rainbow = i.value("rainbow", false);
                bool cps = i.value("cps", false);
                bool noclip = i.value("noclip", false);
                
                Label newLabel(i["text"], color, i["size"], rainbow, cps, noclip);
                
                labelsCorner.push_back(newLabel);
            } break;
            case LabelType::Spacing: {
                labelsCorner.push_back(Label(i["spacing"]));
            } break;
            }
        }
    }
}

static bool m_push = false;

class $modify(LabelsPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<Corner, std::vector<cocos2d::CCLabelBMFont *>> labelObjs;
    };

    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        m_fields->labelObjs[Corner::Top_Left] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Top_Center] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Top_Right] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Center_Left] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Center_Center] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Center_Right] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Bottom_Left] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Bottom_Center] = std::vector<cocos2d::CCLabelBMFont*>();
        m_fields->labelObjs[Corner::Bottom_Right] = std::vector<cocos2d::CCLabelBMFont*>();
        g_sessionStart = std::chrono::steady_clock::now();
        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (m_anticheatSpike && obj == m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, obj);

        PlayLayer::destroyPlayer(player, obj);
        NoclipAccuracy::get().handle_death();
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        CpsCounter::get().reset();
        NoclipAccuracy::get().handle_reset();
    }
    
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        CpsCounter::get().update();

        auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        g_rainbowHue += 0.125f * dt;
        if (g_rainbowHue > 1.0f) g_rainbowHue -= 1.0f;
        
        for (auto &[corner, labelVec] : m_fields->labelObjs) {
            float y = 0.0f, labelsHeight = 0.0f; int i = 0;
            for (const auto &labelObj : labels[corner]) {
                if (!labelObj.enabled) continue;
                if (labelObj.type == LabelType::Text) {
                    std::string renderedText = labelObj.render();
                    if (renderedText.empty()) continue; 

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
                        label->setColor({static_cast<GLubyte>(r * 255), static_cast<GLubyte>(g * 255), static_cast<GLubyte>(b * 255)});
                    } 
                    else if (labelObj.cps) {
                        m_push ? label->setColor({0, 255, 0}) 
                        : label->setColor({static_cast<GLubyte>(labelObj.color[0] * 255),
                            static_cast<GLubyte>(labelObj.color[1] * 255),static_cast<GLubyte>(labelObj.color[2] * 255)});
                    }
                    else if (labelObj.noclip) {
                        NoclipAccuracy::get().prevDied ? label->setColor({255, 0, 0}) 
                        : label->setColor({static_cast<GLubyte>(labelObj.color[0] * 255),
                            static_cast<GLubyte>(labelObj.color[1] * 255),static_cast<GLubyte>(labelObj.color[2] * 255)});
                    }
                    else {
                        label->setColor({static_cast<GLubyte>(labelObj.color[0] * 255),
                            static_cast<GLubyte>(labelObj.color[1] * 255),static_cast<GLubyte>(labelObj.color[2] * 255)});
                    }
                    label->setOpacity(static_cast<GLubyte>(labelObj.color[3] * 255));
                    label->setCString(renderedText.c_str());

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
                    if (labelObj.render().empty()) continue; 

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

class $modify(ReplayGJBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        m_push = down;
        if (down) {
            CpsCounter::get().click();
        }
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {       
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        NoclipAccuracy::get().handle_update(this, dt);
    }
};