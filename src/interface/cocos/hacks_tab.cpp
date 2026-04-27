#include "hacks_tab.hpp"
#include "../../core/gui.hpp"
#include "../../core/config.hpp"

HacksTab* HacksTab::create() {
    auto ret = new HacksTab();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void updateRowAlignment(cocos2d::CCMenu* row) {
    if (!row) return;
    if (auto layout = static_cast<geode::RowLayout*>(row->getLayout())) {
        if (row->getChildrenCount() == 1) {
            layout->setAxisAlignment(geode::AxisAlignment::Center);
        } else {
            layout->setAxisAlignment(geode::AxisAlignment::Start);
        }
        row->updateLayout();
    }
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
        prepareNewRow();
    }

    m_currentRow->addChild(hackNode);
    updateRowAlignment(m_currentRow);
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

void HacksTab::addSeparator(float height) {
    auto node = cocos2d::CCLayerColor::create();
    node->setContentSize({335.f, height});
    node->setColor({104, 104, 191});
    node->setOpacity(255);
    m_scrollLayer->m_contentLayer->addChild(node);
}

void HacksTab::prepareNewRow() {
    float columnWidth = 175.f;
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

void HacksTab::addConfigToggle(const std::string& labelText, const std::string& key, bool defaultValue) {
    auto& config = Config::get();
    float columnWidth = 175.f;

    auto node = CCMenu::create();
    node->setContentSize({columnWidth, 30.f});
    node->setAnchorPoint({0, 0.5f});
    
    auto toggle = CCMenuItemExt::createTogglerWithFilename("GDH_togglerOn.png"_spr, "GDH_togglerOff.png"_spr, 0.8f, [key](CCMenuItemToggler* sender) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.findHackByIDGlobal(key);
        if (hack != nullptr)
            hack->toggle();
    });
    toggle->setPosition({25.f, 15.f});
    toggle->toggle(config.get<bool>(key, defaultValue));
    node->addChild(toggle);

    auto label = CCLabelBMFont::create(labelText.c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.65f); 
    label->setPosition({toggle->getPositionX() + 22.f, 15.f});
    node->addChild(label);

    if (!m_currentRow || m_currentRow->getChildrenCount() >= 2) prepareNewRow();
    m_currentRow->addChild(node);
    updateRowAlignment(m_currentRow);
}

void HacksTab::addConfigIntInput(const std::string& labelText, const std::string& key, int defaultValue, std::function<void(int)> callback) {
    auto& config = Config::get();
    float columnWidth = 175.f;
    
    auto node = CCMenu::create();
    node->setContentSize({columnWidth, 30.f});
    
    auto input = geode::TextInput::create(60.f, "Val", "GoogleSans.fnt"_spr);
    if (auto* bg = input->getChildByType<geode::NineSlice>(0)) { bg->setColor({71, 71, 131}); bg->setOpacity(255); }
    input->setScale(0.7f);
    input->setPosition({30.f, 15.f});
    input->setString(std::to_string(config.get<int>(key, defaultValue)));
    input->setFilter("0123456789-");
    input->setCallback([key, callback](const std::string& str) {
        auto value = geode::utils::numFromString<int>(str);
        if (!value.isErr()) {
            int val = value.unwrap();
            Config::get().set<int>(key, val);
            if (callback) callback(val);
        }
    });
    node->addChild(input);
    
    auto label = CCLabelBMFont::create(labelText.c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.55f);
    label->setPosition({60.f, 15.f});
    node->addChild(label);
    
    if (!m_currentRow || m_currentRow->getChildrenCount() >= 2) prepareNewRow();
    m_currentRow->addChild(node);
    updateRowAlignment(m_currentRow);
}

void HacksTab::addConfigFloatInput(const std::string& labelText, const std::string& key, float defaultValue, std::function<void(float)> callback) {
    auto& config = Config::get();
    float columnWidth = 175.f;
    
    auto node = CCMenu::create();
    node->setContentSize({columnWidth, 30.f});
    
    auto input = geode::TextInput::create(60.f, "Val", "GoogleSans.fnt"_spr);
    if (auto* bg = input->getChildByType<geode::NineSlice>(0)) { bg->setColor({71, 71, 131}); bg->setOpacity(255); }
    input->setScale(0.7f);
    input->setPosition({30.f, 15.f});
    input->setString(fmt::format("{:.2f}", config.get<float>(key, defaultValue)));
    input->setFilter("0123456789-.");
    input->setCallback([key, callback](const std::string& str) {
        auto value = geode::utils::numFromString<float>(str);
        if (!value.isErr()) {
            float val = value.unwrap();
            Config::get().set<float>(key, val);
            if (callback) callback(val);
        }
    });
    node->addChild(input);

    auto label = CCLabelBMFont::create(labelText.c_str(), "GoogleSans.fnt"_spr);
    label->setAnchorPoint({0.f, 0.5f});
    label->setScale(0.55f);
    label->setPosition({60.f, 15.f});
    node->addChild(label);

    if (!m_currentRow || m_currentRow->getChildrenCount() >= 2) prepareNewRow();
    m_currentRow->addChild(node);
    updateRowAlignment(m_currentRow);
}

void HacksTab::addText(const std::string& text, float scale) {
    auto label = CCLabelBMFont::create(text.c_str(), "GoogleSans.fnt"_spr);
    label->setScale(scale);
    
    prepareNewRow(); 
    m_currentRow->addChild(label);
    
    if (auto layout = static_cast<geode::RowLayout*>(m_currentRow->getLayout())) {
        layout->setAxisAlignment(geode::AxisAlignment::Center);
        m_currentRow->updateLayout();
    }
}