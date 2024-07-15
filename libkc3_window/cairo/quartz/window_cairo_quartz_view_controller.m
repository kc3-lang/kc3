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
#import "window_cairo_quartz_view_controller.h"

@implementation WindowCairoQuartzViewController

- (instancetype) initWithCairoView:(WindowCairoQuartzView *)cairo_view {
  self = [super init];
  if (self) {
    self.cairo_view = cairo_view;
  }
  return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view addSubview:self.cairo_view];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [self.view.window makeFirstResponder:self.cairo_view];
}

@end
