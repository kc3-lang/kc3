//
//  custom_cairo_swiftui_view.swift
//  libc3_window_cairo_quartz
//
//  Created by Thomas de Grivel on 14/11/2023.
//

import SwiftUI

struct CustomCairoSwiftUIView : NSViewRepresentable {
    
    func makeNSView(context: Context) -> CustomCairoView {
        return CustomCairoView();
    }
    
    func updateNSView(_ nsView: CustomCairoView, context: Context) {
        puts("updateNSView");
    }
}
