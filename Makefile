CC      := clang
CFLAGS  := -std=c99 -Wall -Werror
LDFLAGS := -Wl,-z,relro,-z,now -lncurses
SRC     := src/main.c
BIN     := 2048-tui

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(BIN)

install: $(BIN)
	install -Dm755 $(BIN) $(DESTDIR)/usr/bin/$(BIN)

uninstall:
	rm -f $(DESTDIR)/usr/bin/$(BIN)

.PHONY: all clean install uninstall
