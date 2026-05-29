#include <Geode/Geode.hpp>
#include "../hacks_tab.hpp"
#include "../../../core/gui.hpp"
#include "../../../core/replayEngine.hpp"

class LabelInfoScheduler : public CCLabelBMFont {
public:
    void customUpdate(float dt) {
        auto& engine = GDH::ReplayEngine::get();
        
        this->setString(fmt::format(
            "Frame: {}\nReplay Size: {}/{}", 
            engine.get_frame(), 
            engine.get_current_index(), 
            engine.get_actions_size()
        ).c_str());
    }
};

$execute {
    auto& gui = GDH::Gui::get();
    auto& window = gui.getWindow("Replay Engine");
    auto& engine = GDH::ReplayEngine::get();

    window.setCustomWindowCocos([&gui, &engine](cocos2d::CCNode* node) {
        auto tab = static_cast<HacksTab*>(node);
        auto engineTab = CCMenu::create();
        engineTab->setContentSize({500.f, 250.f});

        // Info Label
        auto infoLabel = LabelInfoScheduler::create("", "GoogleSans.fnt"_spr);
        infoLabel->setAnchorPoint({0.f, 0.5f});
        infoLabel->setPosition({8, 15});
        infoLabel->setScale(0.5f);
        infoLabel->schedule(schedule_selector(LabelInfoScheduler::customUpdate), 0.0f);
        engineTab->addChild(infoLabel);

        // Record Toggle
        auto recordToggle = CCMenuItemExt::createTogglerWithFilename("GDH_togglerOn.png"_spr, "GDH_togglerOff.png"_spr, 0.8f,
            [&engine, infoLabel, engineTab](CCMenuItemToggler* sender) {
                auto* playToggle = static_cast<CCMenuItemToggler*>(engineTab->getChildByTag(6969));
                if (playToggle && !sender->isOn() && playToggle->isOn()) {
                    playToggle->toggle(false);
                }

                if (!sender->isOn()) {
                    engine.clear();
                    engine.mode = state::record;

                    auto& hack = GDH::Gui::get().getWindow("Level").findHackByName("Practice Fix");   
                    if (!hack.getEnabled()) hack.toggle();
                } else {
                    engine.mode = state::disable;
                }
            });

        recordToggle->setPosition({25.f, 235.f});
        recordToggle->setTag(6767);
        if (engine.mode == state::record) recordToggle->toggle(true);
        engineTab->addChild(recordToggle);
        engineTab->addChild(tab->AddTextToToggle("Record", recordToggle));

        // Play Toggle
        auto playToggle = CCMenuItemExt::createTogglerWithFilename("GDH_togglerOn.png"_spr, "GDH_togglerOff.png"_spr, 0.8f,
            [&engine, infoLabel, engineTab](CCMenuItemToggler* sender) {
                auto* recordToggle = static_cast<CCMenuItemToggler*>(engineTab->getChildByTag(6767));
                if (recordToggle && !sender->isOn() && recordToggle->isOn()) {
                    recordToggle->toggle(false);
                }

                engine.mode = !sender->isOn() ? state::play : state::disable;
            });

        playToggle->setPosition({123.f, 235.f});
        playToggle->setTag(6969);
        if (engine.mode == state::play) playToggle->toggle(true);
        engineTab->addChild(playToggle);
        engineTab->addChild(tab->AddTextToToggle("Play", playToggle));
        
        // Replay Name Input
        auto replayNameInput = geode::TextInput::create(220.f, "Replay Name", "GoogleSans.fnt"_spr);
        if (auto* bg = replayNameInput->getChildByType<geode::NineSlice>(0)) { 
            bg->setColor({71, 71, 131}); 
            bg->setOpacity(255); 
        }
        replayNameInput->setPosition({118.f, 200.f});
        replayNameInput->setString(engine.replay_name);
        replayNameInput->setCallback([&engine](const std::string& text) { engine.replay_name = text; });
        engineTab->addChild(replayNameInput);

        auto saveButton = ButtonSprite::create("Save", 65, true, "GoogleSans.fnt"_spr, "GDH_button_01.png"_spr, 30.f, 0.7f);
        auto saveButtonClick = CCMenuItemExt::createSpriteExtra(saveButton, [&engine, infoLabel](CCMenuItemSpriteExtra* sender) {
            GDH::MaterialLayer(FLAlertLayer::create("Info", engine.save(engine.replay_name).c_str(), "OK"))->show();
        });
        saveButtonClick->setPosition({48, 163});
        engineTab->addChild(saveButtonClick);

        auto loadButton = ButtonSprite::create("Load", 65, true, "GoogleSans.fnt"_spr, "GDH_button_01.png"_spr, 30.f, 0.7f);
        auto loadButtonClick = CCMenuItemExt::createSpriteExtra(loadButton, [&engine, infoLabel](CCMenuItemSpriteExtra* sender) {
            GDH::MaterialLayer(FLAlertLayer::create("Info", engine.load(engine.replay_name).c_str(), "OK"))->show();
        });
        loadButtonClick->setPosition({135, 163});
        engineTab->addChild(loadButtonClick);

        auto cleanButton = ButtonSprite::create("Clear", 65, true, "GoogleSans.fnt"_spr, "GDH_button_01.png"_spr, 30.f, 0.7f);
        auto cleanButtonClick = CCMenuItemExt::createSpriteExtra(cleanButton, [&engine, infoLabel](CCMenuItemSpriteExtra* sender) {
            engine.clear();
            GDH::MaterialLayer(FLAlertLayer::create("Info", "Replay has been cleared", "OK"))->show();
        });
        cleanButtonClick->setPosition({222, 163});
        engineTab->addChild(cleanButtonClick);
        
        tab->m_scrollLayer->m_contentLayer->addChild(engineTab);
    });
}