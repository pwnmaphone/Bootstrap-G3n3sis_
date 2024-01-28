TARGET := iphone:clang:latest:7.0

include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = launchdhooker

launchdhooker_FILES = launchdhooker.m
launchdhooker_CFLAGS = -fobjc-arc
launchdhooker_INSTALL_PATH = /usr/local/lib

include $(THEOS_MAKE_PATH)/library.mk
