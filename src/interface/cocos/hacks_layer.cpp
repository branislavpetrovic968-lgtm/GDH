#include "hacks_layer.hpp"
#include "../../core/config.hpp"
#include "../../core/gui.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"

HacksLayer* HacksLayer::instance = nullptr;

HacksLayer::~HacksLayer() {
    instance = nullptr;
}

bool HacksLayer::init() {
    if (!Popup::init(460.f, 260.f, "GDH_square.png"_spr))
        return false;

    auto& gui = GDH::Gui::get();
    auto& config = Config::get();

    auto windows = gui.getWindows();

    m_closeBtn->setVisible(false);

    auto logo = cocos2d::CCSprite::create("GDH_logo.png"_spr);
    logo->setScale(0.6f);
    logo->setPosition({45.f, 230.f});
    m_mainLayer->addChild(logo);

    int i = 0;
    float tabPositionY = 195.f;
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
        tab->setVisible(i == 0);
        tab->setID(fmt::format("{}"_spr, winName));
        m_mainLayer->addChild(tab);

        for (auto& hack : win.getHacks()) {
            tab->addToggle(hack);          
        }
        tab->addPadding();

        tab->m_scrollLayer->m_contentLayer->updateLayout();
        tab->m_scrollLayer->moveToTop();

        m_tabs.push_back(tab);

        tabPositionY -= 29;
        i++;
    }

    return true;
}

void HacksLayer::switchTab(int newIndex) {
    if (newIndex < 0 || newIndex >= m_tabs.size()) return;

    m_index = newIndex;

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
    Popup::onClose(object);
    instance = nullptr;
}

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HacksTab::addToggle(GDH::Hack& hck) {
    const std::string ID = hck.getID();
    auto& gui = GDH::Gui::get();

    auto toggle = CCMenuItemExt::createTogglerWithFilename("GDH_togglerOn.png"_spr, "GDH_togglerOff.png"_spr, 0.75f, [this, &gui, ID](CCMenuItemToggler* sender) {
        auto* hack = gui.findHackByIDGlobal(ID);
        hack->toggle();
    });

    toggle->setPosition({ 25, 10 });
    toggle->toggle(hck.getEnabled());

    auto label = CCLabelBMFont::create(hck.getName().c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.75f);
    label->setPosition({ toggle->getPositionX() + 20.f, 10 });

    auto hack = CCMenu::create();
    hack->setContentSize({325, 23});
    hack->addChild(toggle);
    hack->addChild(label);

    std::string desc = hck.getDesc();
    if (!desc.empty()) {
        auto descSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        descSprite->setScale(0.4f);
        auto descClick = CCMenuItemExt::createSpriteExtra(descSprite, [this, desc](CCMenuItemSpriteExtra* sender) {
            FLAlertLayer::create("Description", desc.c_str(), "OK")->show();
        });
        descClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 5.f, label->getScaledContentHeight());
        hack->addChild(descClick);
    }

    if (hck.avaibleCustomWindowCocos()) {
        auto moreSettingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        moreSettingsSprite->setScale(0.5f);
        auto moreSettingsSpriteClick = CCMenuItemExt::createSpriteExtra(moreSettingsSprite, [this, &gui, ID](CCMenuItemSpriteExtra* sender) {
            auto* hack = gui.findHackByIDGlobal(ID);
            hack->callCustomWindowCocos();
        });
        moreSettingsSpriteClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 25.f, label->getPositionY());
        hack->addChild(moreSettingsSpriteClick);
    }

    m_scrollLayer->m_contentLayer->addChild(hack);
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    m_scrollLayer = ScrollLayer::create({325.f, 240.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(false)
            ->setGap(0.f)
    );
    m_scrollLayer->setPosition({105.f, 10.f});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;
    
    addChild(m_scrollLayer);
    
    return true;
}

void HacksTab::addPadding() {
    auto node = cocos2d::CCNode::create();
    node->setContentHeight(5.f);

    m_scrollLayer->m_contentLayer->addChild(node);
}