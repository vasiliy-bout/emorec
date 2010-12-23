ROOTDIR = $(CURDIR)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = emoeigen

ALL_SUBDIRS = src
SUBDIRS = src

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n 'Linking $(TARGET) ...'
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` $(CORE_LIBS)
	@echo ' OK'

clean:
	@for subdir in $(ALL_SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@$(RM) $(TARGET) err

do_install:
	@install -d $(BINDIR)
	@install $(TARGET) $(BINDIR)/$(TARGET)
	@install -m 0644 data/pcaconfig.xml $(BINDIR)/pcaconfig.xml

