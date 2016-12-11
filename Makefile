OUT		= sidewinder.out
SRC		= sidewinder.c

LIBUSB_VERSION	= libusb-1.0
LIBUSB_CFLAGS	= $(shell pkg-config --cflags $(LIBUSB_VERSION))
LIBUSB_LDFLAGS	= $(shell pkg-config --libs $(LIBUSB_VERSION))

CFLAGS=-g3 -Wall -Wextra -pedantic -std=c99
CFLAGS+= $(LIBUSB_CFLAGS)

LDFLAGS= $(LIBUSB_LDFLAGS)


OBJ=$(SRC:.c=.o)

all: $(OUT)

test: $(OUT)
	-sudo ./$<

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(OUT)

.PHONY: all test clean
