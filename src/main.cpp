#include <Geode/Geode.hpp>
#include <Geode/modify/PlatformToolbox.hpp>
#include <sabe.persistenceapi/include/PersistenceAPI.hpp>

using namespace geode::prelude;
using namespace persistenceAPI;

#if defined(GEODE_IS_MAC)
// On Windows hideCursor is inlined so it can't be hooked
class $modify(PSPlatformToolbox, PlatformToolbox) {
public:
    $override
    static void hideCursor() {
        // Don't let GD hide the cursor while our save slot popup is open
        auto* scene = CCScene::get();
        if (scene && scene->getChildByID("play-level-menu-popup"_spr)) return;
        PlatformToolbox::hideCursor();
    }
};
#endif
