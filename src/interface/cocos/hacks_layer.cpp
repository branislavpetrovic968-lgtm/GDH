#include "hacks_layer.hpp"
#include "../../core/config.hpp"
#include "../../core/gui.hpp"
#include "../../core/labels.hpp"

HacksLayer* HacksLayer::instance = nullptr;

HacksLayer::~HacksLayer() {
    instance = nullptr;
}

bool HacksLayer::init() {
    if (!Popup::init(460.f, 260.f, "GDH_square.png"_spr))
        return false;

    auto& gui = GDH::Gui::get();
    auto& config = Config::get();

    m_index = config.get<int>("gui_mobile.index", 0);
    m_lastIndexScroll = config.get<int>("gui_mobile.lastIndexScroll", 20.f);

    auto windows = gui.getWindows();

    m_closeBtn->setVisible(false);
    
    auto version = CCLabelBMFont::create(geode::Mod::get()->getVersion().toVString().c_str(), "GoogleSans.fnt"_spr);
    version->setAnchorPoint({1.f, 0.f});
    version->setScale(0.5f);
    version->setPosition({CCDirector::get()->getScreenRight() - 6.f, 10.f});
    version->setOpacity(0);
    version->runAction(CCFadeTo::create(0.15f, 175));
    addChild(version);
    
    auto logo = cocos2d::CCSprite::create("GDH_logo.png"_spr);
    logo->setAnchorPoint({1.f, 0.f});
    logo->setScale(0.3f);
    logo->setPosition({CCDirector::get()->getScreenRight() - 6.f - version->getScaledContentWidth(), 6.f});
    logo->setOpacity(0);
    logo->runAction(CCFadeTo::create(0.15f, 200));
    addChild(logo);

    auto panel = geode::NineSlice::create("GDH_roundBG.png"_spr);
    panel->setAnchorPoint({0, 0});
    panel->setColor({50, 50, 107});
    panel->setPosition({5.f, 5.f});
    panel->setContentSize({100.f, 250.f});
    m_mainLayer->addChild(panel);

    int i = 0;
    float tabPositionY = 235.f;
    for (auto& win : windows) {
        auto winName = win.getName();
        if (winName == "Framerate" || winName == "Invisible")
            continue;

        auto button = ButtonSprite::create(winName.c_str(), 90, true, "GoogleSans.fnt"_spr, (i == m_index) ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr, 30.f, 0.7f);
        button->m_label->setColor((i == m_index) ? ccColor3B({33, 33, 78}) : ccColor3B({255, 255, 255}));
        button->setScale(0.8f);

        auto buttonClick = CCMenuItemExt::createSpriteExtra(button, [this, i](CCMenuItemSpriteExtra* sender) {
            switchTab(i);
        });
        buttonClick->setPosition({55.f, tabPositionY});
        m_buttonTabs.push_back(buttonClick);
        m_buttonMenu->addChild(buttonClick);

        auto tab = HacksTab::create();
        tab->setVisible(i == m_index);
        tab->setID(fmt::format("{}"_spr, winName));
        m_mainLayer->addChild(tab);
        
        for (auto& hack : win.getHacks()) {
            tab->addToggle(hack);          
        }

        if (win.avaibleCustomWindowCocos()) win.callCustomWindowCocos(tab);
        else tab->addPadding(2.5f);
        
        tab->m_scrollLayer->m_contentLayer->updateLayout();
        if (i == m_index && m_lastIndexScroll != 20.f)
            tab->m_scrollLayer->m_contentLayer->setPositionY(m_lastIndexScroll);
        else
            tab->m_scrollLayer->moveToTop();

        m_tabs.push_back(tab);

        tabPositionY -= 29;
        i++;
    }

    return true;
}

void HacksLayer::switchTab(int newIndex) {
    if (newIndex < 0 || newIndex >= m_tabs.size()) return;

    auto& config = Config::get();
    m_index = newIndex;
    config.set<int>("gui_mobile.index", m_index);

    for (int i = 0; i < m_tabs.size(); i++) {
        m_tabs[i]->setVisible(i == m_index);
    }

    for (int i = 0; i < m_buttonTabs.size(); i++) {
        auto* button = m_buttonTabs[i];
        if (button) {
            auto* btnSprite = static_cast<ButtonSprite*>(button->getChildren()->objectAtIndex(0));
            if (btnSprite) {
                btnSprite->updateBGImage(i == m_index ? "GDH_button_02.png"_spr : "GDH_button_01.png"_spr);
                btnSprite->m_label->setColor((i == m_index) ? ccColor3B({33, 33, 78}) : ccColor3B({255, 255, 255}));
            }
        }
    }
}

HacksLayer* HacksLayer::create() {
    auto* ret = new HacksLayer();
    if (ret->init()) { ret->autorelease(); return ret; }
    delete ret;
    return nullptr;
}

HacksLayer* HacksLayer::get() {
    if (!instance) instance = create();
    return instance;
}

bool HacksLayer::isOpened() {
    return instance != nullptr;
}

void HacksLayer::onClose(CCObject* object) {
    auto& config = Config::get();
    config.set<int>("gui_mobile.lastIndexScroll", m_tabs[m_index]->m_scrollLayer->m_contentLayer->getPositionY());
    config.save(fileDataPath);
    GDH::Labels::Manager::get().save();

    Popup::onClose(object);
    instance = nullptr;
}