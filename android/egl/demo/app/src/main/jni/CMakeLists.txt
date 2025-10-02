cmake_minimum_required(VERSION 3.10.2)
project(kc3_egl_android_demo)

# Set C standard
set(CMAKE_C_STANDARD 11)

###

# Build libffi with autotools
set(LIBFFI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libffi)
set(LIBFFI_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj/libffi)

# Set up proper Android toolchain variables for libffi
set(LIBFFI_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(LIBFFI_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(LIBFFI_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(LIBFFI_CFLAGS "-fPIC")

# Configure and build libffi using autotools
add_custom_command(
  OUTPUT ${LIBFFI_BUILD_DIR}/.libs/libffi.a ${LIBFFI_BUILD_DIR}/install/include/ffi.h
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBFFI_BUILD_DIR}
  COMMAND cd ${LIBFFI_DIR} && ./autogen.sh
  COMMAND cd ${LIBFFI_BUILD_DIR} && ${LIBFFI_DIR}/configure --target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --prefix=${LIBFFI_BUILD_DIR}/install --enable-static --enable-shared --disable-symvers CC="${LIBFFI_CC}" CFLAGS="${LIBFFI_CFLAGS}" AR="${LIBFFI_AR}" RANLIB="${LIBFFI_RANLIB}"
  COMMAND cd ${LIBFFI_BUILD_DIR} && make
  COMMAND cd ${LIBFFI_BUILD_DIR} && make install
  WORKING_DIRECTORY ${LIBFFI_BUILD_DIR}
  COMMENT "Building libffi with autotools"
)

add_custom_target(libffi_build DEPENDS ${LIBFFI_BUILD_DIR}/.libs/libffi.a ${LIBFFI_BUILD_DIR}/install/include/ffi.h)

# Import the built library
add_library(ffi STATIC IMPORTED)
set_target_properties(ffi PROPERTIES
  IMPORTED_LOCATION ${LIBFFI_BUILD_DIR}/.libs/libffi.a
)
add_dependencies(ffi libffi_build)

###

# Build Libpng with autotools
set(LIBPNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
set(LIBPNG_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj/libpng)

# Set up proper Android toolchain variables for libpng
set(LIBPNG_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(LIBPNG_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(LIBPNG_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(LIBPNG_CFLAGS "-fPIC")

# Configure and build libpng using autotools
add_custom_command(
  OUTPUT ${LIBPNG_BUILD_DIR}/.libs/libpng16.a ${LIBPNG_BUILD_DIR}/install/include/png.h
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBPNG_BUILD_DIR}
  COMMAND cd ${LIBPNG_DIR} && ./autogen.sh
  COMMAND cd ${LIBPNG_BUILD_DIR} && ${LIBPNG_DIR}/configure --target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --prefix=${LIBPNG_BUILD_DIR}/install --enable-static --disable-shared --disable-symvers CC="${LIBPNG_CC}" CFLAGS="${LIBPNG_CFLAGS}" AR="${LIBPNG_AR}" RANLIB="${LIBPNG_RANLIB}"
  COMMAND cd ${LIBPNG_BUILD_DIR} && make
  COMMAND cd ${LIBPNG_BUILD_DIR} && make install
  WORKING_DIRECTORY ${LIBPNG_BUILD_DIR}
  COMMENT "Building Libpng with autotools"
)

add_custom_target(libpng_build DEPENDS ${LIBPNG_BUILD_DIR}/.libs/libpng16.a ${LIBPNG_BUILD_DIR}/install/include/png.h)

# Import the built library
add_library(libpng STATIC IMPORTED)
set_target_properties(libpng PROPERTIES
  IMPORTED_LOCATION ${LIBPNG_BUILD_DIR}/.libs/libpng16.a
)
add_dependencies(libpng libpng_build)

###

# Build FreeType with autotools
set(FREETYPE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/freetype)
set(FREETYPE_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj/freetype)

# Set up proper Android toolchain variables for freetype
set(FREETYPE_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(FREETYPE_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(FREETYPE_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(FREETYPE_CFLAGS "-fPIC")

# Configure and build freetype using autotools
add_custom_command(
  OUTPUT ${FREETYPE_BUILD_DIR}/.libs/libfreetype.a ${FREETYPE_BUILD_DIR}/install/include/freetype2/ft2build.h
  COMMAND ${CMAKE_COMMAND} -E make_directory ${FREETYPE_BUILD_DIR}
  COMMAND cd ${FREETYPE_DIR} && ./autogen.sh
  COMMAND cd ${FREETYPE_BUILD_DIR} && ${FREETYPE_DIR}/configure --target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --prefix=${FREETYPE_BUILD_DIR}/install --enable-static --enable-shared --disable-symvers --without-harfbuzz --without-brotli --without-bzip2 --without-png CC="${FREETYPE_CC}" CFLAGS="${FREETYPE_CFLAGS}" AR="${FREETYPE_AR}" RANLIB="${FREETYPE_RANLIB}"
  COMMAND cd ${FREETYPE_BUILD_DIR} && make
  COMMAND cd ${FREETYPE_BUILD_DIR} && make install
  WORKING_DIRECTORY ${FREETYPE_BUILD_DIR}
  COMMENT "Building FreeType with autotools"
)

add_custom_target(freetype_build DEPENDS ${FREETYPE_BUILD_DIR}/.libs/libfreetype.a ${FREETYPE_BUILD_DIR}/install/include/freetype2/ft2build.h)

# Import the built library
add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES
  IMPORTED_LOCATION ${FREETYPE_BUILD_DIR}/.libs/libfreetype.a
)
add_dependencies(freetype freetype_build)

###

# Build libevent with autotools
set(LIBEVENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libevent)
set(LIBEVENT_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj/libevent)

# Set up proper Android toolchain variables for libevent
set(LIBEVENT_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(LIBEVENT_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(LIBEVENT_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(LIBEVENT_CFLAGS "-fPIC")

# Configure and build libevent using autotools
add_custom_command(
  OUTPUT ${LIBEVENT_BUILD_DIR}/.libs/libevent.a ${LIBEVENT_BUILD_DIR}/install/include/event2/event.h
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBEVENT_BUILD_DIR}
  COMMAND cd ${LIBEVENT_DIR} && ./autogen.sh
  COMMAND cd ${LIBEVENT_BUILD_DIR} && ${LIBEVENT_DIR}/configure --target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --prefix=${LIBEVENT_BUILD_DIR}/install --enable-static --enable-shared CC="${LIBEVENT_CC}" CFLAGS="${LIBEVENT_CFLAGS}" AR="${LIBEVENT_AR}" RANLIB="${LIBEVENT_RANLIB}"
  COMMAND cd ${LIBEVENT_BUILD_DIR} && make
  COMMAND cd ${LIBEVENT_BUILD_DIR} && make install
  WORKING_DIRECTORY ${LIBEVENT_BUILD_DIR}
  COMMENT "Building libevent with autotools"
)

add_custom_target(libevent_build DEPENDS ${LIBEVENT_BUILD_DIR}/.libs/libevent.a ${LIBEVENT_BUILD_DIR}/install/include/event2/event.h)

# Import the built library
add_library(event STATIC IMPORTED)
set_target_properties(event PROPERTIES
  IMPORTED_LOCATION ${LIBEVENT_BUILD_DIR}/.libs/libevent.a
)
add_dependencies(event libevent_build)

###

# Build libressl with autotools
set(LIBRESSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libressl)
set(LIBRESSL_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obj/libressl)

# Set up proper Android toolchain variables for libressl
set(LIBRESSL_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(LIBRESSL_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(LIBRESSL_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(LIBRESSL_CFLAGS "-fPIC")

# Configure and build libressl using autotools
add_custom_command(
  OUTPUT ${LIBRESSL_BUILD_DIR}/install/lib/libtls.a ${LIBRESSL_BUILD_DIR}/install/lib/libssl.a ${LIBRESSL_BUILD_DIR}/install/lib/libcrypto.a ${LIBRESSL_BUILD_DIR}/install/include/tls.h
  COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRESSL_BUILD_DIR}
  COMMAND cd ${LIBRESSL_DIR} && ./autogen.sh
  COMMAND cd ${LIBRESSL_BUILD_DIR} && ${LIBRESSL_DIR}/configure --target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} --prefix=${LIBRESSL_BUILD_DIR}/install --enable-static --disable-shared CC="${LIBRESSL_CC}" CFLAGS="${LIBRESSL_CFLAGS}" AR="${LIBRESSL_AR}" RANLIB="${LIBRESSL_RANLIB}"
  COMMAND cd ${LIBRESSL_BUILD_DIR} && make
  COMMAND cd ${LIBRESSL_BUILD_DIR} && make install
  WORKING_DIRECTORY ${LIBRESSL_BUILD_DIR}
  COMMENT "Building libressl with autotools"
)

add_custom_target(libressl_build DEPENDS ${LIBRESSL_BUILD_DIR}/install/lib/libtls.a ${LIBRESSL_BUILD_DIR}/install/lib/libssl.a ${LIBRESSL_BUILD_DIR}/install/lib/libcrypto.a ${LIBRESSL_BUILD_DIR}/install/include/tls.h)

# Import the built libraries
add_library(tls STATIC IMPORTED)
set_target_properties(tls PROPERTIES
  IMPORTED_LOCATION ${LIBRESSL_BUILD_DIR}/install/lib/libtls.a
)
add_dependencies(tls libressl_build)

add_library(ssl STATIC IMPORTED)
set_target_properties(ssl PROPERTIES
  IMPORTED_LOCATION ${LIBRESSL_BUILD_DIR}/install/lib/libssl.a
)
add_dependencies(ssl libressl_build)

add_library(crypto STATIC IMPORTED)
set_target_properties(crypto PROPERTIES
  IMPORTED_LOCATION ${LIBRESSL_BUILD_DIR}/install/lib/libcrypto.a
)
add_dependencies(crypto libressl_build)

###

# KC3 source path (absolute)
get_filename_component(KC3_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.." ABSOLUTE)

# Run configure in KC3 root to generate config.h
set(KC3_CC "${ANDROID_TOOLCHAIN_ROOT}/bin/aarch64-linux-android${ANDROID_NATIVE_API_LEVEL}-clang")
set(KC3_AR "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(KC3_RANLIB "${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(KC3_CPPFLAGS "-I${LIBFFI_BUILD_DIR}/install/include -I${LIBEVENT_DIR}/include -I${LIBEVENT_BUILD_DIR}/include -I${LIBRESSL_BUILD_DIR}/install/include")
execute_process(
  COMMAND bash -c "cd ${KC3_ROOT} && target=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} host=aarch64-linux-android${ANDROID_NATIVE_API_LEVEL} CC='${KC3_CC}' AR='${KC3_AR}' RANLIB='${KC3_RANLIB}' CPPFLAGS='${KC3_CPPFLAGS}' HAVE_EGL=true HAVE_ANDROID=true HAVE_XCB=false HAVE_LIBTLS=true ./configure"
  RESULT_VARIABLE CONFIGURE_RESULT
)

# Build KC3 libraries with make
add_custom_command(
  OUTPUT ${KC3_ROOT}/libkc3/.libs/libkc3.a ${KC3_ROOT}/gl/.libs/libkc3_gl.a
  COMMAND make -C ${KC3_ROOT}
  WORKING_DIRECTORY ${KC3_ROOT}
  COMMENT "Building KC3 libraries with make"
  DEPENDS libevent_build libffi_build libressl_build
)

add_custom_target(kc3_libs DEPENDS ${KC3_ROOT}/libkc3/.libs/libkc3.a ${KC3_ROOT}/gl/.libs/libkc3_gl.a)

# Include directories
include_directories(
    ${KC3_ROOT}
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${FREETYPE_BUILD_DIR}/install/include/freetype2
    ${LIBFFI_BUILD_DIR}/install/include
    ${LIBPNG_DIR}
    ${LIBPNG_BUILD_DIR}
    ${LIBRESSL_BUILD_DIR}/install/include
)

# Demo source file
set(DEMO_SOURCES "${KC3_ROOT}/window/egl/android/demo/window_egl_android_demo.c")

# Add native app glue
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv2-lib GLESv2)

# Create the library
add_library(kc3_egl_android_demo SHARED
    ${DEMO_SOURCES}
    ${APP_GLUE_DIR}/android_native_app_glue.c
)

# Link libraries
target_link_libraries(kc3_egl_android_demo
    ${KC3_ROOT}/libkc3/.libs/libkc3.a
    ${KC3_ROOT}/gl/.libs/libkc3_gl.a
    ffi
    tls
    freetype
    png
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv2-lib}
    m  # math library
    z  # zlib (required by libpng)
)

# Make sure KC3 and all dependencies are built before the main library
add_dependencies(kc3_egl_android_demo kc3_libs libffi_build libressl_build libevent_build freetype_build libpng_build)

# Compiler flags
target_compile_definitions(kc3_egl_android_demo PRIVATE
    GL_GLEXT_PROTOTYPES
)

target_compile_options(kc3_egl_android_demo PRIVATE
    -Wall
    -Wextra
    -fPIC
)
