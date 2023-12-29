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
#import <Foundation/Foundation.h>
#import <cairo/cairo-quartz.h>
#import <xkbcommon/xkbcommon.h>
#import "quartz_to_xkbcommon.h"
#import "window_cairo_quartz_view.h"

@implementation WindowCairoQuartzView

- (instancetype) initWithWindowCairo:(s_window_cairo *) window_cairo {
  NSRect frame = NSMakeRect(window_cairo->x, window_cairo->y, window_cairo->w, window_cairo->h);
  self = [super initWithFrame:frame];
  if (self) {
    self.window_cairo = window_cairo;
  }
  return self;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void) drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    struct CGContext *cg_context = [[NSGraphicsContext currentContext]
                                     CGContext];
    CGContextTranslateCTM(cg_context, 0.0, self.bounds.size.height);
    CGContextScaleCTM(cg_context, 1.0, -1.0);
    cairo_surface_t *surface =
      cairo_quartz_surface_create_for_cg_context(cg_context,
                                                 self.bounds.size.width,
                                                 self.bounds.size.height);
    cairo_t *cr = cairo_create(surface);
    if (! self.window_cairo->render(self.window_cairo)) {
      [self.window close];
      [self.window release];
      [[NSApplication sharedApplication] stop:nil];
    }
    cairo_surface_flush(surface);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

- (void)keyDown:(NSEvent *)event {
  NSString *characters = [event characters];
  unichar character = [characters characterAtIndex:0];
  u32 keysym = quartz_to_xkbcommon(character);
  if (! self.window_cairo->key(self.window_cairo, keysym)) {
    [self.window close];
    [self.window release];
    [[NSApplication sharedApplication] stop:nil];
  }
}

- (void)mouseDown:(NSEvent *)event {
  NSPoint p = [event locationInWindow];
  if (! self.window_cairo->button(self.window_cairo, 0, p.x, p.y)) {
    [self.window close];
    [self.window release];
    [[NSApplication sharedApplication] stop:nil];
  }
}

- (void)mouseMoved:(NSEvent *)event {
  NSPoint p = [event locationInWindow];
  p.y = self.window_cairo->h - p.y;
  if (! self.window_cairo->motion(self.window_cairo, p.x, p.y)) {
    [self.window close];
    [self.window release];
    [[NSApplication sharedApplication] stop:nil];
  }
}

- (void)rightMouseDown:(NSEvent *)event {
  NSPoint p = [event locationInWindow];
  if (! self.window_cairo->button(self.window_cairo, 2, p.x, p.y)) {
    [self.window close];
    [self.window release];
    [[NSApplication sharedApplication] stop:nil];
  }
}

@end

