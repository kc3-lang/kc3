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
#ifndef LIBC3_WINDOW_CAIRO_QUARTZ_VIEW_CONTROLLER_H
#define LIBC3_WINDOW_CAIRO_QUARTZ_VIEW_CONTROLLER_H

#import <Cocoa/Cocoa.h>
#import "window_cairo_quartz_view.h"

@interface WindowCairoQuartzViewController : NSViewController

@property (strong) WindowCairoQuartzView *cairo_view;

- (instancetype) initWithCairoView:(WindowCairoQuartzView *)cairo_view;

@end

#endif /* LIBC3_WINDOW_CAIRO_QUARTZ_VIEW_H */
