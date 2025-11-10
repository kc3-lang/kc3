# Makefile for building KC3 EGL Android Demo APK

# Project directories
PROJECT_DIR := $(shell pwd)
APP_DIR := $(PROJECT_DIR)/app
BUILD_DIR := $(APP_DIR)/build
JNI_DIR := $(APP_DIR)/src/main/jni

# Use gradle directly
GRADLE := gradle

# Build variants
BUILD_TYPE ?= debug
APK_TYPE := $(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')

# Output APK paths
DEBUG_APK := $(BUILD_DIR)/outputs/apk/debug/app-debug.apk
RELEASE_APK := $(BUILD_DIR)/outputs/apk/release/app-release.apk

.PHONY: all clean build debug release install \
        uninstall logcat help

# Default target
all: debug

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

# Build debug APK
build:
	$(GRADLE) assembleDebug
	@echo "Debug APK built: $(DEBUG_APK)"

debug: build

# Build release APK
release:
	$(GRADLE) assembleRelease
	@echo "Release APK built: $(RELEASE_APK)"

# Install debug APK to connected device
install: build
	$(GRADLE) installDebug
	@echo "Debug APK installed"

# Install release APK to connected device
install-release: release
	adb install -r $(RELEASE_APK)
	@echo "Release APK installed"

# Uninstall app from device
uninstall:
	adb uninstall io.kmx.kc3.demo || true

# Clean build artifacts
clean:
	$(GRADLE) clean
	rm -rf $(BUILD_DIR)
	rm -rf .gradle

# Show filtered logcat
logcat:
	adb logcat | grep -E "(kc3|KC3|EGL|NativeActivity)"

# List connected devices
devices:
	adb devices -l

# Run app on device after installing
run: install
	adb shell am start -n \
	  io.kmx.kc3.demo/android.app.NativeActivity

# Full rebuild
rebuild: clean build

# Show APK info
apk-info:
	@if [ -f $(DEBUG_APK) ]; then \
		echo "Debug APK info:"; \
		aapt dump badging $(DEBUG_APK) | head -20; \
	fi
	@if [ -f $(RELEASE_APK) ]; then \
		echo "Release APK info:"; \
		aapt dump badging $(RELEASE_APK) | head -20; \
	fi
