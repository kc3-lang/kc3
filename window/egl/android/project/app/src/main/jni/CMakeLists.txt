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

# Build libffi with autotools
set(LIBFFI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libffi)
set(LIBFFI_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/libffi-build)

# Configure and build libffi using autotools
add_custom_command(
  OUTPUT ${LIBFFI_BUILD_DIR}/.libs/libffi.a
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBFFI_BUILD_DIR}
  COMMAND cd ${LIBFFI_DIR} && ./autogen.sh
  COMMAND cd ${LIBFFI_BUILD_DIR} && ${LIBFFI_DIR}/configure --host=arm-linux-androideabi --prefix=${LIBFFI_BUILD_DIR}/install --enable-static --disable-shared CC=${CMAKE_C_COMPILER} CFLAGS="${CMAKE_C_FLAGS} --target=armv7-none-linux-androideabi${ANDROID_PLATFORM}" LDFLAGS="${CMAKE_SHARED_LINKER_FLAGS}" AR=${CMAKE_AR} RANLIB=${CMAKE_RANLIB}
  COMMAND cd ${LIBFFI_BUILD_DIR} && make
  WORKING_DIRECTORY ${LIBFFI_BUILD_DIR}
  COMMENT "Building libffi with autotools"
)

add_custom_target(libffi_build DEPENDS ${LIBFFI_BUILD_DIR}/.libs/libffi.a)

# Import the built library
add_library(ffi STATIC IMPORTED)
set_target_properties(ffi PROPERTIES
  IMPORTED_LOCATION ${LIBFFI_BUILD_DIR}/.libs/libffi.a
)
add_dependencies(ffi libffi_build)

# Include directories
include_directories(
    ${KC3_ROOT}
    ${KC3_ROOT}/libkc3
    ${KC3_ROOT}/window/egl
    ${KC3_ROOT}/window/egl/android
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${LIBFFI_BUILD_DIR}/install/include
)

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
    ${LIBFFI_SOURCES}
    ${APP_GLUE_DIR}/android_native_app_glue.c
)

# Link libraries
target_link_libraries(kc3_egl_android_demo
    ffi
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
    m  # math library
)

# Make sure libffi is built before the main library
add_dependencies(kc3_egl_android_demo libffi_build)

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