PROJECT = dfm09-firmware
BUILD_DIR = bin

SHARED_DIR = ../common-code
CFILES = main.c
CFILES += clock_setup.c
CFILES += gps.c
CFILES += usart.c

# TODO - you will need to edit these two lines!
DEVICE=stm32f100c8t6
OOCD_FILE = ../ftdi_jtagkey.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
