#import <AppKit/AppKit.h>
#import <objc/runtime.h>

static bool g_blockCursorHide = false;
static IMP original_nscursor_hide = nil;

static void swizzled_hide(id self, SEL _cmd) {
    if (!g_blockCursorHide && original_nscursor_hide) {
        ((void(*)(id, SEL))original_nscursor_hide)(self, _cmd);
    }
}

void installCursorHookMac() {
    Method m = class_getClassMethod([NSCursor class], @selector(hide));
    if (m) {
        original_nscursor_hide = method_getImplementation(m);
        method_setImplementation(m, (IMP)swizzled_hide);
    }
}

void setCursorHideBlockedMac(bool blocked) {
    g_blockCursorHide = blocked;
    if (!blocked) return;
    // Drain any outstanding hides so cursor becomes visible immediately
    while ([NSCursor isHidden]) {
        [NSCursor unhide];
    }
    // One extra unhide in case isHidden is stale
    [NSCursor unhide];
    // Explicitly set the arrow cursor in case a transparent cursor was active
    [[NSCursor arrowCursor] set];
}

void forceShowCursorMac() {
    while ([NSCursor isHidden]) {
        [NSCursor unhide];
    }
    [NSCursor unhide];
    [[NSCursor arrowCursor] set];
}
