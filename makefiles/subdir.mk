include $(ROOTDIR)/makefiles/config.mk

INCLUDE = $(EMOINCLUDE)

HEADERS = $(wildcard *.h)
SOURCES_CPP = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES_CPP))

.SUFFIXES: .cpp .o .h

.cpp.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

.M.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

.m.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

all: $(OBJECTS)

clean:
	@$(RM) *.o *.s *.ld *.d

-include *.d

