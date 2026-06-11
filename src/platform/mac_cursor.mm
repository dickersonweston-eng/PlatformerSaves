#import <AppKit/AppKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <objc/runtime.h>
#include <Geode/Geode.hpp>

static bool g_blockCursorHide = false;
static IMP  original_nscursor_hide = nil;
static int  g_hideBlockedCount = 0;
static int  g_hidePassedCount  = 0;

static void swizzled_nscursor_hide(id self, SEL _cmd) {
    if (g_blockCursorHide) {
        g_hideBlockedCount++;
        geode::log::info("[PS cursor] BLOCKED hide #{}", g_hideBlockedCount);
        return;
    }
    g_hidePassedCount++;
    geode::log::info("[PS cursor] passed hide #{}", g_hidePassedCount);
    if (original_nscursor_hide)
        ((void(*)(id, SEL))original_nscursor_hide)(self, _cmd);
}

void installCursorHookMac() {
    Method m = class_getClassMethod([NSCursor class], @selector(hide));
    if (m) {
        original_nscursor_hide = method_getImplementation(m);
        method_setImplementation(m, (IMP)swizzled_nscursor_hide);
        geode::log::info("[PS cursor] installed [NSCursor hide] swizzle");
    } else {
        geode::log::warn("[PS cursor] could not find [NSCursor hide] — swizzle failed");
    }
}

void setCursorHideBlockedMac(bool blocked) {
    g_blockCursorHide = blocked;
    geode::log::info("[PS cursor] setCursorHideBlocked({})", blocked);

    if (!blocked) return;

    // [NSCursor isHidden] was removed in macOS 26 — call unhide a fixed number
    // of times to drain whatever reference count GD accumulated.
    for (int i = 0; i < 10; i++) [NSCursor unhide];
    geode::log::info("[PS cursor] unhid 10x");

    CGDisplayShowCursor(kCGDirectMainDisplay);
    geode::log::info("[PS cursor] called CGDisplayShowCursor");

    CGAssociateMouseAndMouseCursorPosition(true);
    [[NSCursor arrowCursor] set];
}

void forceShowCursorMac() {
    for (int i = 0; i < 10; i++) [NSCursor unhide];
    CGDisplayShowCursor(kCGDirectMainDisplay);
    CGAssociateMouseAndMouseCursorPosition(true);
    [[NSCursor arrowCursor] set];
    geode::log::info("[PS cursor] forceShow done");
}
