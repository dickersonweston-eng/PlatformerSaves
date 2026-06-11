#pragma once
#include <Geode/Geode.hpp>
#include <util/filesystem.hpp>

class PlayLevelMenuPopup : public FLAlertLayer {
public:
    static PlayLevelMenuPopup* create(bool i_slotExists[PS_SAVE_SLOT_COUNT]);

    bool init() override;
    void keyBackClicked() override;

private:
    bool m_slotExists[PS_SAVE_SLOT_COUNT] = {};

    CCMenuItemSpriteExtra* m_continueButton[PS_SAVE_SLOT_COUNT]    = {};
    CCMenuItemSpriteExtra* m_deleteButton[PS_SAVE_SLOT_COUNT]      = {};
    ButtonSprite*          m_continueBtnSprite[PS_SAVE_SLOT_COUNT] = {};
    cocos2d::CCDrawNode*   m_rowBg[PS_SAVE_SLOT_COUNT]            = {};
    cocos2d::CCDrawNode*   m_slotBadge[PS_SAVE_SLOT_COUNT]        = {};

    CCMenuItemSpriteExtra* m_newGameButton = nullptr;
    cocos2d::extension::CCScale9Sprite* m_background = nullptr;

    // Cached layout values needed by refreshSlotRow
    cocos2d::CCSize m_panelSize = {};
    float           m_rowH      = 0.0f;

    PlayLevelMenuPopup(bool i_slotExists[PS_SAVE_SLOT_COUNT]);

    void setup();
    void refreshSlotRow(int i_slot);
    void onRemove(cocos2d::CCObject* i_sender);
    void deferredRemove(float);
    void keepCursorVisible(float);
    void onClose(cocos2d::CCObject* i_sender);
    void onNewGame(cocos2d::CCObject* i_sender);
    void onContinueSlot(cocos2d::CCObject* i_sender);
    void onDeleteSlot(cocos2d::CCObject* i_sender);
};
