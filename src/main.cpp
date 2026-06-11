#include <Geode/Geode.hpp>
#include <Geode/modify/PlatformToolbox.hpp>
#include <sabe.persistenceapi/include/PersistenceAPI.hpp>
#if defined(GEODE_IS_MAC)
#include <platform/mac_cursor.hpp>
#endif

using namespace geode::prelude;
using namespace persistenceAPI;

$on_mod(Loaded) {
#if defined(GEODE_IS_MAC)
    // Swizzle [NSCursor hide] so we can block it while the save slot popup is open.
    // This intercepts cursor hiding regardless of which code path GD uses.
    installCursorHookMac();
#endif
}

#if defined(GEODE_IS_MAC)
// Belt-and-suspenders: also block via PlatformToolbox in case it adds extra hides.
class $modify(PSPlatformToolbox, PlatformToolbox) {
public:
    $override
    static void hideCursor() {
        auto* scene = CCScene::get();
        if (scene && scene->getChildByID("play-level-menu-popup"_spr)) return;
        PlatformToolbox::hideCursor();
    }
};
#endif
