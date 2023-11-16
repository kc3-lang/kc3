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
#import <Cocoa/Cocoa.h>
#import "window_cairo_quartz.h"
#import "window_cairo_quartz_app_delegate.h"

bool window_cairo_quartz_run (s_window_cairo *window)
{
  @autoreleasepool {
    NSApplication *app = [NSApplication sharedApplication];
    WindowCairoQuartzAppDelegate *app_delegate = [[WindowCairoQuartzAppDelegate alloc] initWithWindowCairo:window];
    [app setDelegate:app_delegate];
    [app run];
  }
  return true;
}

bool window_cairo_run (s_window_cairo *window)
{
  return window_cairo_quartz_run(window);
}
