#pragma once
#include <Geode/Geode.hpp>
#if defined(GEODE_IS_MAC)
#include <platform/mac_cursor.hpp>
#endif

using namespace geode::prelude;

namespace util::platform {
    // Limit is 7 cause we use a 3 bit bitfield
    enum class PSPlatform {
        Win = 0,
        Android64 = 1,
        Android32 = 2,
        MacArm = 3,
        MacIntel = 4,
        Ios = 5,
        PlatformInvalid = 7,
    };

    inline void toggleLockCursor(bool i_state) {
        #if defined(GEODE_IS_WINDOWS)
            CCEGLView::get()->toggleLockCursor(i_state);
        #elif defined(GEODE_IS_MAC)
            // TODO: implement someday
        #endif
    }

    // Block or unblock cursor hiding at the OS level (no-op on non-desktop platforms).
    inline void setCursorHideBlocked(bool blocked) {
        #if defined(GEODE_IS_MAC)
        setCursorHideBlockedMac(blocked);
        #endif
        // Windows: hideCursor is inlined so we can't block it; showCursor every frame
        // in keepCursorVisible is the best we can do there.
    }

    inline void hideAndLockCursor(bool i_hide) {
        #if defined(GEODE_IS_WINDOWS)
        if (i_hide) {
            PlatformToolbox::hideCursor();
        } else {
            PlatformToolbox::showCursor();
        }
        bool l_lockCursor = GameManager::get()->getGameVariable("0128");
        if (l_lockCursor) {
            toggleLockCursor(i_hide);
        }
        #elif defined(GEODE_IS_MAC)
        if (!i_hide) {
            forceShowCursorMac();
        }
        #endif
    }
}