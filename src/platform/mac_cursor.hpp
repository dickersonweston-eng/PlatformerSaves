#pragma once

#if defined(GEODE_IS_MAC)
// Swizzle [NSCursor hide] at startup so we can block it while the popup is open.
void installCursorHookMac();
// Block or unblock all future [NSCursor hide] calls.
void setCursorHideBlockedMac(bool blocked);
// Forcibly drain the NSCursor hide counter so the cursor becomes visible.
void forceShowCursorMac();
#endif
