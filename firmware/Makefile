#
#             LUFA Library
#     Copyright (C) Dean Camera, 2021.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU          = atmega32u2
ARCH         = AVR8
BOARD        = BOARD_NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = 3
TARGET       = build/main
SRC          = src/main.c src/Descriptors.c $(LUFA_SRC_USB)
LUFA_PATH    = drivers/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iinc/
OBJDIR		 = build/obj
LD_FLAGS     = 

all:

# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk