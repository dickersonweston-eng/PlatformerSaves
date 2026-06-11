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
    bool result = CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr, unk);
    // While our popup is showing, any key-down that reaches PlayLayer leaves
    // a "key held" state that survives into the level. Immediately synthesize
    // a key-up so delegates see a clean press-and-release rather than a hold.
    if (down && isPopupShowing()) {
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