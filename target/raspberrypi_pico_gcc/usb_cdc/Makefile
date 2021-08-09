KERNEL_DIRS := $(KERNEL_DIRS) $(TARGETDIR)/usb_cdc/tinyusb \
	$(TARGETDIR)/usb_cdc/tinyusb/class/cdc \
	$(TARGETDIR)/usb_cdc/tinyusb/common \
	$(TARGETDIR)/usb_cdc/tinyusb/device \
	$(TARGETDIR)/usb_cdc/tinyusb/portable/raspberrypi/rp2040 \
	$(TARGETDIR)/usb_cdc/pico-sdk

INCLUDES := $(INCLUDES) -I$(TARGETDIR)/usb_cdc/tinyusb \
	-I$(TARGETDIR)/usb_cdc/tinyusb/class/cdc \
	-I$(TARGETDIR)/usb_cdc/tinyusb/common \
	-I$(TARGETDIR)/usb_cdc/tinyusb/device \
	-I$(TARGETDIR)/usb_cdc/tinyusb/portable/raspberrypi/rp2040 \
	-I$(TARGETDIR)/usb_cdc/pico-sdk

KERNEL_COBJS := $(KERNEL_COBJS) target_serial.o cdc_device.o \
	tusb_fifo.o usbd.o usbd_control.o dcd_rp2040.o rp2040_usb.o \
	stdio_usb_descriptors.o tusb.o unique_id.o pll.o clocks.o \
	flash.o bootrom.o xosc.o watchdog.o

KERNEL_ASMOBJS := $(KERNEL_ASMOBJS) mem_ops_aeabi.o

# LIBS := -Wl,--start-group -lkernel $(LIBS) -Wl,--end-group
OBJ_LDFLAGS := $(OBJ_LDFLAGS) -Wl,--wrap=memset -Wl,--wrap=__aeabi_memset -Wl,--wrap=__aeabi_memset4 -Wl,--wrap=__aeabi_memset8 \
	-Wl,--wrap=memcpy -Wl,--wrap=__aeabi_memcpy -Wl,--wrap=__aeabi_memcpy4 -Wl,--wrap=__aeabi_memcpy8
