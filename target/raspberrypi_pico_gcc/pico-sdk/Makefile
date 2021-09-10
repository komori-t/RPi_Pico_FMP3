KERNEL_DIRS := $(KERNEL_DIRS) $(TARGETDIR)/pico-sdk

INCLUDES := $(INCLUDES) -I$(TARGETDIR)/pico-sdk

KERNEL_COBJS := $(KERNEL_COBJS) unique_id.o pll.o clocks.o \
	flash.o bootrom.o xosc.o watchdog.o adc.o gpio.o spi.o

KERNEL_ASMOBJS := $(KERNEL_ASMOBJS) mem_ops_aeabi.o

OBJ_LDFLAGS := $(OBJ_LDFLAGS) -Wl,--wrap=memset -Wl,--wrap=__aeabi_memset -Wl,--wrap=__aeabi_memset4 -Wl,--wrap=__aeabi_memset8 \
	-Wl,--wrap=memcpy -Wl,--wrap=__aeabi_memcpy -Wl,--wrap=__aeabi_memcpy4 -Wl,--wrap=__aeabi_memcpy8
