/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#import "../window_cairo.h"
#import "window_cairo_quartz_view.h"

@interface WindowCairoQuartzAppDelegate :
NSObject <NSApplicationDelegate, NSWindowDelegate>

@property (strong, nonatomic) NSView *view;
@property (strong, nonatomic) NSWindow *window;
@property (nonatomic, assign) s_window_cairo *window_cairo;

- (instancetype) initWithWindowCairo:(s_window_cairo *) window_cairo;

@end
