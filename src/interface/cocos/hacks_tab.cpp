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
            ShowMessage(hck.getName(), desc);
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