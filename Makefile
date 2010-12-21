ROOTDIR = $(CURDIR)

include makefiles/platforms.mk

EMOLIBDIRS = emocore
APPDIRS = 

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

