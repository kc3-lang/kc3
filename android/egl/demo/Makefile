## kc3
## Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
##
## Permission is hereby granted to use this software granted the above
## copyright notice and this permission paragraph are included in all
## copies and substantial portions of this software.
##
## THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
## PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
## AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
## THIS SOFTWARE.

APP_DIR = ${PROJECT_DIR}/app
BUILD_DIR = ${APP_DIR}/build
JNI_DIR = ${APP_DIR}/src/main/jni

GRADLE = gradle

DEBUG_APK = ${BUILD_DIR}/outputs/apk/debug/app-debug.apk
RELEASE_APK = ${BUILD_DIR}/outputs/apk/release/app-release.apk

all: debug

include configure.mk

apk-info:
	@if [ -f ${DEBUG_APK} ]; then \
		echo "Debug APK info:"; \
		aapt dump badging ${DEBUG_APK} | head -20; \
	fi
	@if [ -f ${RELEASE_APK} ]; then \
		echo "Release APK info:"; \
		aapt dump badging ${RELEASE_APK} | head -20; \
	fi

help:
	@echo "KC3 EGL Android Demo - Build Targets"
	@echo ""
	@echo "  make build         - Build debug APK"
	@echo "  make debug         - Build debug APK (same as build)"
	@echo "  make release       - Build release APK"
	@echo "  make install       - Build and install debug APK"
	@echo "  make install-release - Install release APK"
	@echo "  make uninstall     - Uninstall app from device"
	@echo "  make clean         - Clean build artifacts"
	@echo "  make logcat        - Show filtered logcat output"
	@echo ""

build: lib
	${GRADLE} assembleDebug
	@echo "Debug APK built: ${DEBUG_APK}"

clean:
	${GRADLE} clean
	rm -rf ${BUILD_DIR}
	rm -rf .gradle

debug: build

devices:
	adb devices -l

install: build
	${GRADLE} installDebug
	@echo "Debug APK installed"

install-release: release
	adb install -r ${RELEASE_APK}
	@echo "Release APK installed"

lib:
	mkdir -p app/src/main/assets
	rsync -a --delete ${SRC_TOP}/lib app/src/main/assets/

logcat:
	adb logcat | grep -iE "(kc3|egl|nativeactivity|runtimeerror)"

rebuild: clean build

release: lib
	${GRADLE} assembleRelease
	@echo "Release APK built: ${RELEASE_APK}"

run: install
	adb shell am start -n \
	  io.kmx.kc3.demo/android.app.NativeActivity

uninstall:
	adb uninstall io.kmx.kc3.demo || true

.PHONY: all clean build debug release install lib \
        uninstall logcat help
