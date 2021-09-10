KERNEL_DIRS := $(KERNEL_DIRS) $(TARGETDIR)/usb_cdc \
	$(TARGETDIR)/usb_cdc/tinyusb \
	$(TARGETDIR)/usb_cdc/tinyusb/class/cdc \
	$(TARGETDIR)/usb_cdc/tinyusb/common \
	$(TARGETDIR)/usb_cdc/tinyusb/device \
	$(TARGETDIR)/usb_cdc/tinyusb/portable/raspberrypi/rp2040

INCLUDES := $(INCLUDES) -I$(TARGETDIR)/usb_cdc \
	-I$(TARGETDIR)/usb_cdc/tinyusb \
	-I$(TARGETDIR)/usb_cdc/tinyusb/class/cdc \
	-I$(TARGETDIR)/usb_cdc/tinyusb/common \
	-I$(TARGETDIR)/usb_cdc/tinyusb/device \
	-I$(TARGETDIR)/usb_cdc/tinyusb/portable/raspberrypi/rp2040

COPTS := $(COPTS) -DTOPPERS_RPI_PICO_USB_CDC

KERNEL_COBJS := $(KERNEL_COBJS) target_serial.o cdc_device.o \
	tusb_fifo.o usbd.o usbd_control.o dcd_rp2040.o rp2040_usb.o \
	stdio_usb_descriptors.o tusb.o
