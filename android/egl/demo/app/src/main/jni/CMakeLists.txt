cmake_minimum_required(VERSION 3.10.2)

project(kc3_egl_android_demo)

# Add precompiled library
add_library(kc3_egl_android_demo SHARED IMPORTED)
set_target_properties(kc3_egl_android_demo PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libkc3_egl_android_demo.so
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Install the library
install(TARGETS kc3_egl_android_demo
    LIBRARY DESTINATION lib/${ANDROID_ABI}
)
