TARGET := iphone:clang:latest
ARCHS = arm64

include $(THEOS)/makefiles/common.mk

TOOL_NAME = SBTool

SBTool_FILES = main.m
SBTool_CFLAGS = -fobjc-arc -isystem
SBTool_LDFLAGS = -L./ -lbsm
SBTool_CODESIGN_FLAGS = -SSpringBoardEnts.plist

after-package::
	ct_bypass -i .theos/obj/debug/SBTool -o SBToolsign
include $(THEOS_MAKE_PATH)/tool.mk
