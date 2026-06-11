#include "PlayLevelMenuPopup.hpp"
#include "Geode/binding/PlayLayer.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <hooks/PlayLayer.hpp>
#include <util/filesystem.hpp>
#include <util/platform.hpp>

using namespace geode::prelude;
using namespace util::platform;

// ─── construction ────────────────────────────────────────────────────────────

PlayLevelMenuPopup::PlayLevelMenuPopup(bool i_slotExists[PS_SAVE_SLOT_COUNT]) {
    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        m_slotExists[i] = i_slotExists[i];
    }
}

PlayLevelMenuPopup* PlayLevelMenuPopup::create(bool i_slotExists[PS_SAVE_SLOT_COUNT]) {
    PlayLevelMenuPopup* i_this = new PlayLevelMenuPopup(i_slotExists);
    if (i_this && i_this->init()) {
        i_this->autorelease();
    } else {
        CC_SAFE_DELETE(i_this);
    }
    return i_this;
}

// ─── init / setup ────────────────────────────────────────────────────────────

bool PlayLevelMenuPopup::init() {
    if (!FLAlertLayer::init(150)) return false;
    setup();
    return true;
}

void PlayLevelMenuPopup::setup() {
#if defined(GEODE_IS_MAC)
    PlatformToolbox::showCursor();
#endif
    CCSize l_win = CCDirector::sharedDirector()->getWinSize();

    CCSize l_panelSize = CCSize(l_win.width * 0.52f, l_win.height * 0.62f);
    m_background = CCScale9Sprite::create("GJ_square01.png");
    m_background->setContentSize(l_panelSize);
    m_background->setPosition(l_win / 2.0f);
    m_mainLayer->addChild(m_background, -2);

    CCMenu* l_menu = CCMenu::create();
    l_menu->ignoreAnchorPointForPosition(false);
    l_menu->setContentSize(l_panelSize);
    l_menu->setPosition(l_win / 2.0f);
    m_mainLayer->addChild(l_menu, 10);

    CCLabelBMFont* l_title = CCLabelBMFont::create("Select Save Slot", "goldFont.fnt");
    l_title->setScale(0.7f);
    l_title->setPosition({l_panelSize.width / 2.0f, l_panelSize.height - 20.0f});
    l_menu->addChild(l_title);

    float l_rowH = (l_panelSize.height - 60.0f) / (PS_SAVE_SLOT_COUNT + 1);
    float l_topY = l_panelSize.height - 48.0f;

    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        float l_y = l_topY - i * l_rowH;

        CCLabelBMFont* l_label = CCLabelBMFont::create(
            fmt::format("Slot {}", i + 1).c_str(), "bigFont.fnt");
        l_label->setScale(0.45f);
        l_label->setPosition({l_panelSize.width * 0.14f, l_y});
        l_menu->addChild(l_label);

        const char* l_btnText = m_slotExists[i] ? "Continue" : "Empty";
        const char* l_btnBg   = m_slotExists[i] ? "GJ_button_01.png" : "GJ_button_04.png";
        ButtonSprite* l_contSprite = ButtonSprite::create(
            l_btnText, static_cast<int>(l_panelSize.width * 0.33f),
            true, "goldFont.fnt", l_btnBg, 0.0f, 0.8f);
        m_continueBtnSprite[i] = l_contSprite;

        CCMenuItemSpriteExtra* l_contBtn = CCMenuItemSpriteExtra::create(
            l_contSprite, this,
            menu_selector(PlayLevelMenuPopup::onContinueSlot));
        l_contBtn->setTag(i);
        l_contBtn->setID(fmt::format("continue-slot-{}"_spr, i));
        l_contBtn->setPosition({l_panelSize.width * 0.44f, l_y});
        if (!m_slotExists[i]) l_contBtn->m_bEnabled = false;
        l_menu->addChild(l_contBtn);
        m_continueButton[i] = l_contBtn;

        ButtonSprite* l_delSprite = ButtonSprite::create(
            "Delete", static_cast<int>(l_panelSize.width * 0.22f),
            true, "goldFont.fnt", "GJ_button_06.png", 0.0f, 0.8f);
        CCMenuItemSpriteExtra* l_delBtn = CCMenuItemSpriteExtra::create(
            l_delSprite, this,
            menu_selector(PlayLevelMenuPopup::onDeleteSlot));
        l_delBtn->setTag(i);
        l_delBtn->setID(fmt::format("delete-slot-{}"_spr, i));
        l_delBtn->setPosition({l_panelSize.width * 0.79f, l_y});
        if (!m_slotExists[i]) {
            l_delBtn->setVisible(false);
            l_delBtn->m_bEnabled = false;
        }
        l_menu->addChild(l_delBtn);
        m_deleteButton[i] = l_delBtn;
    }

    float l_newY = l_topY - PS_SAVE_SLOT_COUNT * l_rowH;
    ButtonSprite* l_newSprite = ButtonSprite::create(
        "New Game", static_cast<int>(l_panelSize.width * 0.46f),
        true, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
    m_newGameButton = CCMenuItemSpriteExtra::create(
        l_newSprite, this,
        menu_selector(PlayLevelMenuPopup::onNewGame));
    m_newGameButton->setID("new-game-button"_spr);
    m_newGameButton->setPosition({l_panelSize.width / 2.0f, l_newY});

    bool l_hasEmpty = false;
    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        if (!m_slotExists[i]) { l_hasEmpty = true; break; }
    }
    if (!l_hasEmpty) {
        l_newSprite->m_label->setColor({127, 127, 127});
        l_newSprite->m_BGSprite->setColor({127, 127, 127});
        m_newGameButton->m_bEnabled = false;
    }
    l_menu->addChild(m_newGameButton);

    CCSprite* l_closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    l_closeSprite->setScale(0.8f);
    CCMenuItemSpriteExtra* l_closeBtn = CCMenuItemSpriteExtra::create(
        l_closeSprite, this,
        menu_selector(PlayLevelMenuPopup::onClose));
    l_closeBtn->setID("close-button"_spr);
    float l_off = l_closeBtn->getContentWidth() / 5.0f;
    l_closeBtn->setPosition({l_off, l_panelSize.height - l_off});
    l_menu->addChild(l_closeBtn);
}

// ─── callbacks ───────────────────────────────────────────────────────────────

void PlayLevelMenuPopup::onContinueSlot(CCObject* i_sender) {
    int l_slot = static_cast<CCNode*>(i_sender)->getTag();
    PSPlayLayer* l_pl = static_cast<PSPlayLayer*>(PlayLayer::get());
    if (l_pl && l_pl->m_fields->m_loadingState == LoadingState::WaitingForPlayLevelMenuPopup) {
        l_pl->m_fields->m_saveSlot = l_slot;
    }
    onRemove(nullptr);
}

void PlayLevelMenuPopup::onNewGame(CCObject*) {
    PSPlayLayer* l_pl = static_cast<PSPlayLayer*>(PlayLayer::get());
    if (!l_pl) { onRemove(nullptr); return; }

    bool l_anyExist = false;
    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        if (m_slotExists[i]) { l_anyExist = true; break; }
    }

    auto startNew = [this, l_pl]() {
        int l_slot = 0;
        for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
            if (!m_slotExists[i]) { l_slot = i; break; }
        }
        if (l_pl->m_fields->m_loadingState == LoadingState::WaitingForPlayLevelMenuPopup) {
            l_pl->m_fields->m_saveSlot   = -3;
            l_pl->m_fields->m_newGameSlot = l_slot;
        }
        onRemove(nullptr);
    };

    if (l_anyExist) {
        createQuickPopup("Start New Game",
            "Start a new game? Your saves in other slots are <cy>kept</c>.",
            "Cancel", "Ok",
            [this, startNew](FLAlertLayer*, bool i_btn2) {
                if (i_btn2) startNew();
            });
    } else {
        startNew();
    }
}

void PlayLevelMenuPopup::onDeleteSlot(CCObject* i_sender) {
    int l_slot = static_cast<CCNode*>(i_sender)->getTag();
    PSPlayLayer* l_pl = static_cast<PSPlayLayer*>(PlayLayer::get());
    if (!l_pl) return;

    createQuickPopup("Delete Save",
        fmt::format("Delete save in <cr>Slot {}</c>?", l_slot + 1).c_str(),
        "Cancel", "Delete",
        [this, l_pl, l_slot](FLAlertLayer*, bool i_btn2) {
            if (!i_btn2) return;
            l_pl->removeSaveFile(l_slot);
            m_slotExists[l_slot] = false;
            refreshSlotRow(l_slot);

            // Re-enable "New Game" if a slot opened up
            if (m_newGameButton && !m_newGameButton->m_bEnabled) {
                m_newGameButton->m_bEnabled = true;
                if (auto* l_s = dynamic_cast<ButtonSprite*>(m_newGameButton->getNormalImage())) {
                    l_s->m_label->setColor({255, 255, 255});
                    l_s->m_BGSprite->setColor({255, 255, 255});
                }
            }
        });
}

void PlayLevelMenuPopup::refreshSlotRow(int i_slot) {
    if (m_continueButton[i_slot]) {
        m_continueButton[i_slot]->m_bEnabled = false;
        if (m_continueBtnSprite[i_slot]) {
            m_continueBtnSprite[i_slot]->m_label->setColor({127, 127, 127});
            m_continueBtnSprite[i_slot]->m_BGSprite->setColor({127, 127, 127});
        }
    }
    if (m_deleteButton[i_slot]) {
        m_deleteButton[i_slot]->setVisible(false);
        m_deleteButton[i_slot]->m_bEnabled = false;
    }
}

void PlayLevelMenuPopup::keyBackClicked() {
    PSPlayLayer* l_pl = static_cast<PSPlayLayer*>(PlayLayer::get());
    if (l_pl && l_pl->m_fields->m_loadingState == LoadingState::WaitingForPlayLevelMenuPopup) {
        l_pl->m_fields->m_saveSlot = -2; // cancel
    }
    // Defer by one frame so the Escape key event fully clears before we remove
    // ourselves — prevents PlayLayer from seeing an orphaned key-down and
    // requiring two presses to open the pause menu.
    setKeyboardEnabled(false);
    scheduleOnce(schedule_selector(PlayLevelMenuPopup::deferredRemove), 0.0f);
}

void PlayLevelMenuPopup::deferredRemove(float) {
    removeFromParentAndCleanup(true);
}

void PlayLevelMenuPopup::onClose(CCObject*) { keyBackClicked(); }

void PlayLevelMenuPopup::onRemove(CCObject*) {
    removeFromParentAndCleanup(true);
}
