#include "hacks_tab.hpp"
#include "../../core/gui.hpp"

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void ShowMessage(const std::string& title, const std::string& desc) {
    auto* falert = FLAlertLayer::create(title.c_str(), desc.c_str(), "OK");

    if (auto* sprite9 = falert->m_mainLayer->getChildByType<cocos2d::extension::CCScale9Sprite>(0)) {
        auto* frame = cocos2d::CCSpriteFrame::create("GDH_square.png"_spr, {0, 0, 0, 0});
        auto size = sprite9->getContentSize();
        sprite9->setSpriteFrame(frame);
        sprite9->setContentSize(size);
    }

    if (auto* label = falert->m_mainLayer->getChildByType<cocos2d::CCLabelBMFont>(0)) {
        label->setFntFile("GoogleSans.fnt"_spr);
        label->setColor({255, 188, 29});
    }

    if (falert->m_button1 != nullptr) {
        falert->m_button1->updateBGImage("GDH_button_01.png"_spr);
        falert->m_button1->m_label->setColor(ccColor3B({255, 255, 255}));
        falert->m_button1->m_label->setFntFile("GoogleSans.fnt"_spr);
    }
    falert->show();
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

    std::string name = hck.getName();
    auto label = CCLabelBMFont::create(name.c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.75f);
    label->setPosition({ toggle->getPositionX() + 20.f, 10 });

    auto hack = CCMenu::create();
    hack->setContentSize({325, 23});
    hack->addChild(toggle);
    hack->addChild(label);

    std::string desc = hck.getDesc();
    if (!desc.empty()) {
        auto descSprite = CCSprite::create("GDH_infoIcon.png"_spr);
        descSprite->setScale(0.4f);
        auto descClick = CCMenuItemExt::createSpriteExtra(descSprite, [this, name, desc](CCMenuItemSpriteExtra* sender) {
            ShowMessage(name, desc);
        });
        descClick->setPosition(label->getPositionX() + label->getScaledContentWidth() + 8.f, label->getScaledContentHeight() - 3.f);
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

    m_scrollLayer = ScrollLayer::create({325.f, 250.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(false)
            ->setGap(0.f)
    );
    m_scrollLayer->setPosition({105.f, 5.f});
    m_scrollLayer->m_peekLimitTop = 15;
    m_scrollLayer->m_peekLimitBottom = 15;
    
    addChild(m_scrollLayer);
    
    return true;
}

void HacksTab::addPadding(float height) {
    auto node = cocos2d::CCNode::create();
    node->setContentHeight(height);

    m_scrollLayer->m_contentLayer->addChild(node);
}