#!/bin/sh
# Wrapper script to run kc3_window_sdl2_demo with ANGLE (OpenGL ES via Metal)

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Set environment variables for SDL2 to use ANGLE
export SDL_VIDEO_EGL_DRIVER="${SCRIPT_DIR}/../Frameworks/libEGL.dylib"
export SDL_VIDEO_GL_DRIVER="${SCRIPT_DIR}/../Frameworks/libGLESv2.dylib"

# Run the demo
exec "${SCRIPT_DIR}/kc3_window_sdl2_demo_asan" "$@"
