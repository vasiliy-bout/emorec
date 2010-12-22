include $(ROOTDIR)/makefiles/platforms.mk

MAKE = make ROOTDIR=$(ROOTDIR)
LIBMAKE = $(MAKE) ZLSHARED=$(ZLSHARED)

include $(ROOTDIR)/makefiles/arch/$(TARGET_ARCH).mk

BINDIR ?= $(INSTALLDIR)/bin


ifeq "$(TARGET_STATUS)" "release"
	CFLAGS += -O3
	LDFLAGS += -s
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
	LDFLAGS += -pg
endif

EMOINCLUDE = -I "$(ROOTDIR)/emocore/include"

CORE_LIBS = -L$(ROOTDIR)/emocore -lemocore -lcv -lml -lhighgui -lexpat


ifneq "$(BUILD_RESOURCE_OBJECT)" "yes"
.resources:
endif

