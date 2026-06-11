#include "CCKeyboardDispatcher.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h"
#include <hooks/PlayLayer.hpp>

using namespace geode::prelude;
using namespace persistenceAPI;

static bool isPopupShowing() {
    auto* scene = CCScene::get();
    return scene && scene->getChildByID("play-level-menu-popup"_spr) != nullptr;
}

// overrides

bool PSCCKeyboardDispatcher::dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool arr, double unk) {
    bool popupShowing = isPopupShowing();
    if (popupShowing) {
        geode::log::info("[PS kbd] dispatchKeyboardMSG key={} down={} popupShowing=true", (int)key, down);
    }
    bool result = CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr, unk);
    if (down && popupShowing) {
        geode::log::info("[PS kbd] synthesizing key-up for key={}", (int)key);
        CCKeyboardDispatcher::dispatchKeyboardMSG(key, false, false, unk);
    }
    return result;
}

void PSCCKeyboardDispatcher::removeDelegate(cocos2d::CCKeyboardDelegate* i_delegate) {
    if (s_levelInfoLayerDelegate1 == reinterpret_cast<size_t>(i_delegate)
        || s_levelInfoLayerDelegate2 == reinterpret_cast<size_t>(i_delegate)
        || s_editLevelLayerDelegate1 == reinterpret_cast<size_t>(i_delegate)
        || s_editLevelLayerDelegate2 == reinterpret_cast<size_t>(i_delegate)
    ) {
        return;
    }
    CCKeyboardDispatcher::removeDelegate(i_delegate);
}