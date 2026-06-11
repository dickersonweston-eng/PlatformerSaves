#include <Geode/Geode.hpp>
#include <Geode/modify/PlatformToolbox.hpp>
#include <sabe.persistenceapi/include/PersistenceAPI.hpp>
#if defined(GEODE_IS_MACOS)
#include <platform/mac_cursor.hpp>
#endif

using namespace geode::prelude;
using namespace persistenceAPI;

$on_mod(Loaded) {
#if defined(GEODE_IS_MACOS)
    installCursorHookMac();
#endif
}

#if defined(GEODE_IS_MACOS)
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
