/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#import "window_cairo_quartz_app_delegate.h"
#import "window_cairo_quartz_view.h"

@implementation WindowCairoQuartzAppDelegate

- (instancetype) initWithWindowCairo:(s_window_cairo *)window_cairo {
  self = [super init];
  if (self) {
    self.window_cairo = window_cairo;
  }
  return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  NSRect frame = NSMakeRect(self.window_cairo->x, self.window_cairo->y,
                            self.window_cairo->w, self.window_cairo->h);
  self.window = [[NSWindow alloc]
                  initWithContentRect:frame
                            styleMask:(NSWindowStyleMaskTitled |
                                       NSWindowStyleMaskClosable |
                                       NSWindowStyleMaskResizable)
                              backing:NSBackingStoreBuffered
                                defer:NO];
  [self.window makeKeyAndOrderFront:nil];
  [self.window
      setTitle:[NSString
                 stringWithUTF8String:self.window_cairo->title]];

  WindowCairoQuartzView *view =
    [[WindowCairoQuartzView alloc]
      initWithWindowCairo:self.window_cairo];
  [self.window setContentView:view];
}

@end
