#include "PlayLevelMenuPopup.hpp"
#include "Geode/binding/PlayLayer.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <hooks/PlayLayer.hpp>
#include <util/filesystem.hpp>
#include <util/platform.hpp>

using namespace geode::prelude;
using namespace util::platform;

// drawSolidRect doesn't exist in GD's cocos2d 2.x — use drawPolygon instead.
static void drawFilledRect(CCDrawNode* node, float w, float h, ccColor4F color) {
    CCPoint v[4] = {{-w/2, -h/2}, {w/2, -h/2}, {w/2, h/2}, {-w/2, h/2}};
    node->drawPolygon(v, 4, color, 0.0f, {0, 0, 0, 0});
}

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
    // Block cursor hiding and show cursor immediately (Mac swizzle + per-frame show).
    setCursorHideBlocked(true);
    hideAndLockCursor(false);
    schedule(schedule_selector(PlayLevelMenuPopup::keepCursorVisible));

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    m_panelSize = CCSize(winSize.width * 0.56f, winSize.height * 0.72f);

    m_background = CCScale9Sprite::create("GJ_square01.png");
    m_background->setContentSize(m_panelSize);
    m_background->setPosition(winSize / 2.0f);
    m_mainLayer->addChild(m_background, -2);

    CCMenu* menu = CCMenu::create();
    menu->ignoreAnchorPointForPosition(false);
    menu->setContentSize(m_panelSize);
    menu->setPosition(winSize / 2.0f);
    m_mainLayer->addChild(menu, 10);

    // ── Title ──────────────────────────────────────────────────────────────
    auto* title = CCLabelBMFont::create("Select Save Slot", "goldFont.fnt");
    title->setScale(0.85f);
    title->setPosition({m_panelSize.width / 2.0f, m_panelSize.height - 20.0f});
    menu->addChild(title);

    // Separator under title
    auto* sep = CCDrawNode::create();
    drawFilledRect(sep, m_panelSize.width - 24.0f, 3.0f, {0.0f, 0.0f, 0.0f, 0.30f});
    sep->setPosition({m_panelSize.width / 2.0f, m_panelSize.height - 40.0f});
    menu->addChild(sep, -1);

    // ── Slot rows ──────────────────────────────────────────────────────────
    float contentTop    = m_panelSize.height - 52.0f;
    float contentBottom = 50.0f;
    m_rowH = (contentTop - contentBottom) / static_cast<float>(PS_SAVE_SLOT_COUNT);

    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        float rowCenterY = contentTop - (i + 0.5f) * m_rowH;
        bool  filled     = m_slotExists[i];

        // Row background
        float bgW = m_panelSize.width - 20.0f;
        float bgH = m_rowH - 8.0f;
        auto* rowBg = CCDrawNode::create();
        drawFilledRect(rowBg, bgW, bgH,
            filled ? ccColor4F{0.12f, 0.30f, 0.12f, 0.60f}
                   : ccColor4F{0.18f, 0.18f, 0.18f, 0.55f});
        rowBg->setPosition({m_panelSize.width / 2.0f, rowCenterY});
        menu->addChild(rowBg, 0);
        m_rowBg[i] = rowBg;

        // Slot badge (colored square with slot number)
        float badgeSize = m_rowH * 0.58f;
        auto* badge = CCDrawNode::create();
        drawFilledRect(badge, badgeSize, badgeSize,
            filled ? ccColor4F{0.90f, 0.67f, 0.12f, 1.0f}
                   : ccColor4F{0.27f, 0.27f, 0.27f, 1.0f});
        badge->setPosition({m_panelSize.width * 0.09f, rowCenterY});
        menu->addChild(badge, 1);
        m_slotBadge[i] = badge;

        auto* badgeLabel = CCLabelBMFont::create(
            std::to_string(i + 1).c_str(), "bigFont.fnt");
        badgeLabel->setScale(0.36f);
        badgeLabel->setPosition({m_panelSize.width * 0.09f, rowCenterY});
        menu->addChild(badgeLabel, 2);

        // Slot label
        auto* slotLabel = CCLabelBMFont::create(
            fmt::format("Slot {}", i + 1).c_str(), "bigFont.fnt");
        slotLabel->setScale(0.44f);
        slotLabel->setPosition({m_panelSize.width * 0.23f, rowCenterY});
        menu->addChild(slotLabel, 1);

        // Continue / Empty button
        const char* btnText = filled ? "Continue" : "Empty";
        const char* btnBg   = filled ? "GJ_button_01.png" : "GJ_button_04.png";
        auto* contSprite = ButtonSprite::create(
            btnText, static_cast<int>(m_panelSize.width * 0.30f),
            true, "goldFont.fnt", btnBg, 0.0f, 0.8f);
        m_continueBtnSprite[i] = contSprite;

        auto* contBtn = CCMenuItemSpriteExtra::create(
            contSprite, this, menu_selector(PlayLevelMenuPopup::onContinueSlot));
        contBtn->setTag(i);
        contBtn->setID(fmt::format("continue-slot-{}"_spr, i));
        contBtn->setPosition({m_panelSize.width * 0.54f, rowCenterY});
        if (!filled) contBtn->m_bEnabled = false;
        menu->addChild(contBtn, 2);
        m_continueButton[i] = contBtn;

        // Delete button (hidden when slot is empty)
        auto* delSprite = ButtonSprite::create(
            "Delete", static_cast<int>(m_panelSize.width * 0.22f),
            true, "goldFont.fnt", "GJ_button_06.png", 0.0f, 0.8f);
        auto* delBtn = CCMenuItemSpriteExtra::create(
            delSprite, this, menu_selector(PlayLevelMenuPopup::onDeleteSlot));
        delBtn->setTag(i);
        delBtn->setID(fmt::format("delete-slot-{}"_spr, i));
        delBtn->setPosition({m_panelSize.width * 0.83f, rowCenterY});
        if (!filled) {
            delBtn->setVisible(false);
            delBtn->m_bEnabled = false;
        }
        menu->addChild(delBtn, 2);
        m_deleteButton[i] = delBtn;
    }

    // ── New Game button ────────────────────────────────────────────────────
    bool hasEmpty = false;
    for (int i = 0; i < PS_SAVE_SLOT_COUNT; i++) {
        if (!m_slotExists[i]) { hasEmpty = true; break; }
    }

    auto* newSprite = ButtonSprite::create(
        "New Game", static_cast<int>(m_panelSize.width * 0.44f),
        true, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
    m_newGameButton = CCMenuItemSpriteExtra::create(
        newSprite, this, menu_selector(PlayLevelMenuPopup::onNewGame));
    m_newGameButton->setID("new-game-button"_spr);
    m_newGameButton->setPosition({m_panelSize.width / 2.0f, 24.0f});
    if (!hasEmpty) {
        newSprite->m_label->setColor({127, 127, 127});
        newSprite->m_BGSprite->setColor({127, 127, 127});
        m_newGameButton->m_bEnabled = false;
    }
    menu->addChild(m_newGameButton, 2);

    // ── Close button ───────────────────────────────────────────────────────
    auto* closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSprite->setScale(0.8f);
    auto* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, this, menu_selector(PlayLevelMenuPopup::onClose));
    closeBtn->setID("close-button"_spr);
    float off = closeBtn->getContentWidth() / 5.0f;
    closeBtn->setPosition({off, m_panelSize.height - off});
    menu->addChild(closeBtn, 2);
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
    // Update visual indicators to empty state
    if (m_rowBg[i_slot]) {
        float bgW = m_panelSize.width - 20.0f;
        float bgH = m_rowH - 8.0f;
        m_rowBg[i_slot]->clear();
        drawFilledRect(m_rowBg[i_slot], bgW, bgH, {0.18f, 0.18f, 0.18f, 0.55f});
    }
    if (m_slotBadge[i_slot]) {
        float badgeSize = m_rowH * 0.58f;
        m_slotBadge[i_slot]->clear();
        drawFilledRect(m_slotBadge[i_slot], badgeSize, badgeSize, {0.27f, 0.27f, 0.27f, 1.0f});
    }
}

void PlayLevelMenuPopup::keepCursorVisible(float) {
    hideAndLockCursor(false);
}

void PlayLevelMenuPopup::keyBackClicked() {
    PSPlayLayer* l_pl = static_cast<PSPlayLayer*>(PlayLayer::get());
    if (l_pl && l_pl->m_fields->m_loadingState == LoadingState::WaitingForPlayLevelMenuPopup) {
        l_pl->m_fields->m_saveSlot = -2;
    }
    scheduleOnce(schedule_selector(PlayLevelMenuPopup::deferredRemove), 0.0f);
}

void PlayLevelMenuPopup::deferredRemove(float) {
    setCursorHideBlocked(false);
    removeFromParentAndCleanup(true);
}

void PlayLevelMenuPopup::onClose(CCObject*) { keyBackClicked(); }

void PlayLevelMenuPopup::onRemove(CCObject*) {
    setCursorHideBlocked(false);
    removeFromParentAndCleanup(true);
}
