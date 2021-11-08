.DEFAULT_GOAL := all
ROOT_LOC ?= $(realpath .)
libcomponents:
	make -C $(COMPONENTS_DIR)

clean_components:
	make -C $(COMPONENTS_DIR) clean 

COMPONENTS_LIB_NAME=libcomponents.a
COMPONENTS_DIR=$(ROOT_LOC)/components
COMPONENTS_LIB=$(COMPONENTS_DIR)/$(COMPONENTS_LIB_NAME)

SDK_BRANCH_NAME ?= sdk_2.3.1
EMB_APP_GIT_ID_FROM_FILE := $(shell cd $(ROOT_LOC) ; cat .git_version)

DEBUG ?= no
$(if $(patsubst yes,,$(DEBUG)),,$(info EMB_APP_GIT_ID_FROM_FILE=$(EMB_APP_GIT_ID_FROM_FILE)))
ifeq ($(EMB_APP_GIT_ID_FROM_FILE),)
    $(error ".git_version file not defined)
endif

DEVEL_PACKAGE ?= 0

COMMON_FILES := $(SDK_DIR)/apps/_app_start.o $(COMPONENTS_LIB)
%/src/utils.o : CFLAGS += -DSDK_BRANCH_NAME=\"$(SDK_BRANCH_NAME)\" -DEMB_APP_GIT_ID=\"$(EMB_APP_GIT_ID_FROM_FILE)\"
%.o: CFLAGS += -I$(COMPONENTS_DIR)/utils/inc -DDEVEL_PACKAGE=$(DEVEL_PACKAGE)
%.elf: LDFLAGS += -L$(COMPONENTS_DIR)

objdir := out

disp_info:
	$(info SDK_DIR=$(SDK_DIR))
	$(info objdir=$(objdir))
	$(info DEVEL_PACKAGE=$(DEVEL_PACKAGE))

mk_objdir: $(objdir)

$(objdir):
	mkdir -p $(objdir)

.PHONY: gordon
gordon: mk_objdir
ifeq ($(DEVEL_PACKAGE),1)
	make -C ${SDK_DIR}/apps gordon.elf
	cp ${SDK_DIR}/apps/gordon.elf ./$(objdir)
else
	cp ${SDK_DIR}/apps/gordon/bin/gordon.elf ./$(objdir)
endif

.PHONY: reset
reset:
	@echo "Resetting..."
	python3 ${SDK_DIR}/script/reset.py

ROOTFS_LOC ?= ./root

rootfs: mk_objdir
	make -C $(ROOTFS_LOC)
	cp $(ROOTFS_LOC)/root.img ./$(objdir)/

FAST_SSBL_LOC = $(ROOT_LOC)/apps/ssbl

fast_ssbl: mk_objdir
	make -C $(FAST_SSBL_LOC)
	cp -v $(FAST_SSBL_LOC)/fast_ssbl.img ./$(objdir)/

ssbl_package: mk_objdir
	make gordon
	make rootfs
	make fast_ssbl

clean_ssbl_package:
	make -C $(ROOTFS_LOC) clean
	make -C $(FAST_SSBL_LOC) clean

.PHONY: empty_image
empty_image:
	mkdir -p $(objdir)
	dd if=/dev/zero of=./$(objdir)/empty.img bs=1K count=1

$(objdir)/%.o: %.c
	@mkdir -p $(dir $@)
	${CC} -MD -MP ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

LIST_OF_LIBS := -lbt_controller -lbt_host -lbt_profiles -lc -lcomponents -ldragonfly -limath -lkernel -llwip2 -lmbedtls -lmqtt -lrfdrv -lsupplicant -lwifi

$(objdir)/%.elf:
	${LD} -Map $(objdir)/$*.map -o $@ $(filter %.o,$^) ${LDFLAGS} --start-group $(filter-out -linnobase,$(sort $(LIBGCC) $(LIST_OF_LIBS) $(LIBS))) --end-group
