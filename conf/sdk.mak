ifeq ($(origin top_srcdir), undefined)
top_srcdir := $(abspath $(join $(lastword ${MAKEFILE_LIST}),/../../))
endif

libdir = ${top_srcdir}/lib
includedir = ${top_srcdir}/include
appsdir = ${top_srcdir}/apps
confdir = ${top_srcdir}/conf

CROSS_COMPILE ?= arm-none-eabi-

CC = ${CROSS_COMPILE}gcc
CFLAGS = -Wall -Os -g -mcpu=cortex-m3 -mthumb -fno-common
CPPFLAGS = -I${includedir} -I${includedir}/arm -I${appsdir}/common
LIBGCC    := $(shell ${CC} ${CFLAGS} -print-libgcc-file-name)

ASFLAGS = -mthumb -march=armv7-m

LD = ${CROSS_COMPILE}ld
# <app_name>-virt=no is required to disable virtual build
VIRT = $(if $(filter no,$($(*)-virt)),,-virt)
LDSFILE = ${confdir}/firmware${VIRT}.lds
LDFLAGS = -L${libdir} -L${confdir} -warn-common -zmax-page-size=256 -T${LDSFILE}
DEFAULTLIBS = --start-group -lkernel -lc --end-group ${LIBGCC}

# generate list of dependency files based on variables named *_OBJS
DEPS = $(patsubst %.o,%.d,$(sort $(foreach var,$(filter %_OBJS,${.VARIABLES}),$(value ${var}))))

# Comment out these two lines to disable dead code removal (gc-sections)
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += --gc-sections

%.elf:
	${LD} -Map $*.map -o $@ $(filter %.o,$^) ${LDFLAGS} ${LIBS} ${DEFAULTLIBS}

%.o: %.c
	${CC} -MD -MP ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

%.o: %.S
	${CC} ${ASFLAGS} ${CPPFLAGS} -c -o $@ $<

%.elf.sign : %.elf.strip
	${top_srcdir}/script/signature.py --output signature.bin --key=$(KEY) $<
	arm-none-eabi-objcopy --add-section .signature=signature.bin $< $@
	rm -f signature.bin

%.elf.strip: %.elf
	${CROSS_COMPILE}objcopy --strip-all $< $@
