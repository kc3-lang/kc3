//
//  custom_cairo_view.m
//  libc3_window_cairo_quartz
//
//  Created by Thomas de Grivel on 14/11/2023.
//

#import <Foundation/Foundation.h>
#import "custom_cairo_view.h"
#import <cairo/cairo-quartz.h>

@implementation CustomCairoView

- (void) drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    cairo_surface_t *surface =
    cairo_quartz_surface_create_for_cg_context([[NSGraphicsContext currentContext] CGContext], self.bounds.size.width, self.bounds.size.height);
    cairo_t *cr = cairo_create(surface);
    
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

@end

