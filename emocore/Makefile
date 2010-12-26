ROOTDIR = $(CURDIR)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

TARGET = libemocore.a

SUBDIRS_ALL = src src/xml src/xml/expat
SUBDIRS = src src/xml src/xml/expat

ifeq "$(TARGET_ARCH)" "win32"
#  SUBDIRS += 
else
#  SUBDIRS += 
endif

.objects:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;

$(TARGET): .objects
	@echo -n 'Creating $@ ...'
	@$(RM_QUIET) $(TARGET)
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS))
	@echo ' OK'


clean:
	@for subdir in $(SUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.* *.dylib *.a


do_install:
	@install -d $(BINDIR)
	@install -m 0644 $(wildcard data/*.xml) $(BINDIR)
	@install -d $(BINDIR)/haarcascades
	@install -m 0644 $(wildcard data/haarcascades/*.xml) $(BINDIR)/haarcascades
#	@install -d $(BINDIR)/pca
#	@install -m 0644 $(wildcard data/pca/*.xml) $(BINDIR)/pca
	@install -d $(BINDIR)/mlp
	@install -m 0644 $(wildcard data/mlp/*.mlp) $(BINDIR)/mlp

