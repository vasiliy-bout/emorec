ROOTDIR = $(CURDIR)

include makefiles/platforms.mk

EMOLIBDIRS = emocore
APPDIRS = tools emoguess emoeigen emotrain emoshow emoextract

all:
	@for dir in $(EMOLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; \
			if ! $(MAKE); then \
				exit 1; \
			fi; \
			cd $(ROOTDIR); \
		fi; \
	done;

clean:
	@for dir in $(EMOLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done
	@rm -rf bin


install: all do_install

do_install:
	@for dir in $(EMOLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

