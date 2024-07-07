PROJNAME := SoundPlayer
BINDIR := bin
BUILDDIR := build
SRCDIR := src
INCDIR := include

INCPATH := $(INCDIR)

CC := clang++
CFLAGS := -Wall -std=c++17 -I$(INCPATH)
LDFLAGS := -lopenal -laudio

BIN := $(BINDIR)/$(PROJNAME)

OBJ := $(BUILDDIR)/main.o
OBJ += $(BUILDDIR)/SoundPlayer.o


all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp $(INCDIR)/SoundPlayer.hpp Makefile 
	@mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILDDIR)/SoundPlayer.o: $(SRCDIR)/SoundPlayer.cpp $(INCDIR)/SoundPlayer.hpp Makefile
	@mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: clean
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	@if [ -e $(BINDIR) ]; then rmdir $(BINDIR); fi
	@if [ -e $(BUILDDIR) ]; then rmdir $(BUILDDIR); fi

.PHONY: run
run:
	@./$(BIN)
