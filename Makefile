PROJECT = uki
PLATFORM := $(shell uname -s)

CC = gcc
RM = rm -f
GDB = gdb
MKDIR = mkdir -p

SRCDIR = src
INCDIR = include
BUILDDIR := build
TARGET = $(BUILDDIR)/bin/$(PROJECT)

SOURCES += $(SRCDIR)/main.c
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/obj/%,$(SOURCES:.c=.o))
CFLAGS = -Wall -I $(INCDIR)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILDDIR)/obj/%.o: $(SRCDIR)/%.c
	@$(MKDIR) $(BUILDDIR)/bin
	@$(MKDIR) $(BUILDDIR)/obj
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET)

debug: CFLAGS += -g3 -DDEBUG
debug: clean $(TARGET)
	$(GDB) $(TARGET)

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: clean $(TARGET)
	valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file=valgrind.log ./$(TARGET)
	cat valgrind.log

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) valgrind.log
