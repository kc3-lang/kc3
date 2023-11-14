# Compiler settings
CC = clang
SWIFT = swiftc
CFLAGS = -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lcairo
SWIFTCFLAGS = -import-objc-header "libc3_window_cairo_quartz-Bridging-Header.h"

# Source files
OBJC_SOURCES = custom_cairo_view.m
SWIFT_SOURCES = main.swift ContentView.swift

# Object files
OBJC_OBJECTS = $(OBJC_SOURCES:.m=.o)
SWIFT_OBJECTS = $(SWIFT_SOURCES:.swift=.o)

# App binary
APP_BINARY = c3_window_cairo_quartz_demo

all: $(APP_BINARY)

$(APP_BINARY): $(OBJC_OBJECTS) $(SWIFT_OBJECTS)
    $(CC) $(LDFLAGS) -o $@ $^

%.o: %.m
    $(CC) $(CFLAGS) -c $< -o $@

%.o: %.swift
    $(SWIFT) $(SWIFTCFLAGS) -c $< -o $@

clean:
    rm -f $(APP_BINARY) $(OBJC_OBJECTS) $(SWIFT_OBJECTS)
