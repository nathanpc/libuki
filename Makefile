PROJECT = uki
PLATFORM := $(shell uname -s)

CC = gcc
RM = rm -f
GDB = gdb
MKDIR = mkdir -p
W3M = w3m -T text/html
DILLO = dillo

SRCDIR = src
BUILDDIR := build
TARGET = $(BUILDDIR)/lib/lib$(PROJECT).so
TESTTARGET = $(BUILDDIR)/bin/$(PROJECT)
TESTWIKI = test/
TESTARTICLE = something
TESTRUNLD = LD_LIBRARY_PATH=$(BUILDDIR)/lib:$LD_LIBRARY_PATH
TESTRUN = ./$(TESTTARGET) $(TESTWIKI) $(TESTARTICLE)

SOURCES += $(SRCDIR)/uki.c $(SRCDIR)/config.c $(SRCDIR)/template.c $(SRCDIR)/fileutils.c
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/obj/%,$(SOURCES:.c=.o))
CFLAGS = -Wall
LDFLAGS = -shared

.PHONY: all run test debug memcheck clean
all: $(TARGET)

$(TARGET): CFLAGS += -fPIC
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TESTTARGET): $(TARGET) $(BUILDDIR)/obj/main.o
	$(CC) -L$(BUILDDIR)/lib $(CFLAGS) $(BUILDDIR)/obj/main.o -luki -o $@

$(BUILDDIR)/obj/%.o: $(SRCDIR)/%.c
	@$(MKDIR) $(BUILDDIR)/bin
	@$(MKDIR) $(BUILDDIR)/lib
	@$(MKDIR) $(BUILDDIR)/obj
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET) $(TESTTARGET)
	$(TESTRUNLD) $(TESTRUN)

test: $(TARGET) $(TESTTARGET)
	@#$(TESTRUNLD) $(TESTRUN) | $(W3M)
	$(TESTRUNLD) $(TESTRUN) > $(TESTWIKI)/test.html
	$(DILLO) $(TESTWIKI)/test.html

debug: CFLAGS += -g3 # -DDEBUG
debug: clean $(TARGET) $(TESTTARGET)
	$(TESTRUNLD) $(GDB)

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: clean $(TARGET)
	$(TESTRUNLD) valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file=valgrind.log $(TESTRUN)
	cat valgrind.log

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) valgrind.log
