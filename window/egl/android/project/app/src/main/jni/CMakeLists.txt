cmake_minimum_required(VERSION 3.10.2)
project(kc3_egl_android_demo)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)
find_library(ffi-lib ffi)

# KC3 source path (relative to this CMakeLists.txt)
set(KC3_ROOT "../../../../../../../..")

# Find libffi
find_path(FFI_INCLUDE_DIR ffi.h
    PATHS
    /usr/include
    /usr/local/include
    ${ANDROID_NDK}/sysroot/usr/include
)

# Include directories
include_directories(
    ${KC3_ROOT}
    ${KC3_ROOT}/libkc3
    ${KC3_ROOT}/window/egl
    ${KC3_ROOT}/window/egl/android
    ${ANDROID_NDK}/sources/android/native_app_glue
)

# Add FFI include if found
if(FFI_INCLUDE_DIR)
    include_directories(${FFI_INCLUDE_DIR})
endif()

# Add all KC3 source files (simplified - you may need to adjust paths)
file(GLOB_RECURSE KC3_SOURCES
    "${KC3_ROOT}/libkc3/*.c"
    "${KC3_ROOT}/window/*.c"
    "${KC3_ROOT}/window/egl/*.c"
    "${KC3_ROOT}/window/egl/android/*.c"
    "${KC3_ROOT}/gl/*.c"
)

# Add the demo source that contains android_main
list(APPEND KC3_SOURCES "${KC3_ROOT}/window/egl/android/demo/window_egl_android_demo.c")

# Remove conflicting files if any (using older CMake syntax)
foreach(source ${KC3_SOURCES})
    if(source MATCHES ".*test.*" OR source MATCHES ".*_test\\.c$")
        list(REMOVE_ITEM KC3_SOURCES ${source})
    endif()
endforeach()

# Add native app glue
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})

# Create the library
add_library(kc3_egl_android_demo SHARED
    ${KC3_SOURCES}
    ${APP_GLUE_DIR}/android_native_app_glue.c
)

# Link libraries
target_link_libraries(kc3_egl_android_demo
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
    ${ffi-lib}
    m  # math library
)

# Compiler flags
target_compile_definitions(kc3_egl_android_demo PRIVATE
    HAVE_ANDROID=1
    GL_GLEXT_PROTOTYPES
)

target_compile_options(kc3_egl_android_demo PRIVATE
    -Wall
    -Wextra
    -fPIC
)