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

void HacksTab::addToggle(GDH::Hack& hck) {
    auto& gui = GDH::Gui::get();
    const std::string ID = hck.getID();
    
    float columnWidth = 175.f; 

    auto hackNode = CCMenu::create();
    hackNode->setContentSize({columnWidth, 30.f});
    hackNode->setAnchorPoint({0, 0.5f});

    auto toggle = CCMenuItemExt::createTogglerWithFilename("GDH_togglerOn.png"_spr, "GDH_togglerOff.png"_spr, 0.8f, [&gui, ID](CCMenuItemToggler* sender) {
        auto* hack = gui.findHackByIDGlobal(ID);
        hack->toggle();
    });
    toggle->setPosition({25.f, 15.f});
    toggle->toggle(hck.getEnabled());
    hackNode->addChild(toggle);

    auto label = CCLabelBMFont::create(hck.getName().c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.65f); 
    label->setPosition({toggle->getPositionX() + 22.f, 15.f});
    
    float maxLabelWidth = columnWidth - label->getPositionX() - 30.f; 
    if (label->getScaledContentWidth() > maxLabelWidth) {
        label->setScale(label->getScale() * (maxLabelWidth / label->getScaledContentWidth()));
    }
    hackNode->addChild(label);

    std::string desc = hck.getDesc();
    if (!desc.empty()) {
        auto descSprite = CCSprite::create("GDH_infoIcon.png"_spr);
        descSprite->setScale(0.45f);
        auto descClick = CCMenuItemExt::createSpriteExtra(descSprite, [hck, desc](CCMenuItemSpriteExtra* sender) {
            GDH::MaterialLayer(FLAlertLayer::create(hck.getName().c_str(), desc.c_str(), "OK"))->show();
        });
        
        descClick->setPosition({label->getPositionX() + label->getScaledContentWidth() + 10.f, 17.f});
        hackNode->addChild(descClick);
    }

    if (!m_currentRow || m_currentRow->getChildrenCount() >= 2) {
        m_currentRow = CCMenu::create();
        m_currentRow->setContentSize({columnWidth * 2 + 5.f, 32.f});
        m_currentRow->setLayout(
            geode::RowLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(geode::AxisAlignment::Start)
                ->setCrossAxisAlignment(geode::AxisAlignment::Center)
                ->setAutoScale(false)
        );
        m_scrollLayer->m_contentLayer->addChild(m_currentRow);
    }

    m_currentRow->addChild(hackNode);
    m_currentRow->updateLayout();
}

bool HacksTab::init() {
    if (!CCMenu::init())
        return false;

    setPosition({0, 0});

    m_scrollLayer = ScrollLayer::create({500.f, 250.f});
    m_scrollLayer->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setAutoGrowAxis(false)
            ->setGap(0.f)
    );
    m_scrollLayer->setPosition({107.f, 5.f});
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