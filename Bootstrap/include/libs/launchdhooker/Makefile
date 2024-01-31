TARGET := iphone:clang:latest:7.0
ARCHS = arm64

include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = launchdhooker

launchdhooker_FILES = $(wildcard *.m) $(wildcard *.c)
launchdhooker_CFLAGS = -fobjc-arc -isystem "../../privateheaders" -Wno-error
launchdhook_CODESIGN_FLAGS = -S../../../basebin/launchdents.plist

include $(THEOS_MAKE_PATH)/library.mk
