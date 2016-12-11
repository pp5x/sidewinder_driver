/*
 * Userland driver using libUSB.
 * Device: Microsoft Sidewinder Game Controller
 */

#define _GNU_SOURCE

#include <libusb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define VENDOR_ID 0x045e
#define PRODUCT_ID 0x000e

/* Convert Hz to microseconds */
#define TIME_US_FREQUENCY(Hz) (1000000 / (Hz))

#define POLL_TIME TIME_US_FREQUENCY(2000)

static bool running = true;

static void exit_handler(int unused)
{
    (void)unused;
    running = false;
}

static libusb_device_handle *get_sidewinder_handle(void)
{
    struct libusb_device_handle *h;
    h = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);

    if (!h)
    {
        fputs("Microsoft Sidewinder not found.\n", stderr);
        return NULL;
    }

    if (libusb_kernel_driver_active(h, 0) &&
        libusb_detach_kernel_driver(h, 0) < 0)
    {
        fputs("Kernel driver already attached.\n", stderr);
    }

    libusb_claim_interface(h, 0);

    return h;
}

struct sidewinder
{
    struct
    {
        unsigned char pad       : 4;
        unsigned char a         : 1;
        unsigned char b         : 1;
        unsigned char c         : 1;
        unsigned char x         : 1;
    } buttons_1;
    unsigned char x_axis;
    unsigned char y_axis;
    unsigned char unknown;
    unsigned char buttons_2;
    unsigned char wheel;
};

static void read_state(struct libusb_device_handle *h, struct sidewinder *state)
{
    int transferred = 0;
    int ret = libusb_interrupt_transfer(h, 0x81, (unsigned char *)state,
                                        sizeof (*state), &transferred, 50);
    if (ret < 0 && ret != LIBUSB_ERROR_TIMEOUT)
    {
        fputs("fail?\n", stderr);
        running = false;
    }
}

enum pad
{
    UP          = 0,
    UP_RIGHT    = 1,
    RIGHT       = 2,
    DOWN_RIGHT  = 3,
    DOWN        = 4,
    DOWN_LEFT   = 5,
    LEFT        = 6,
    UP_LEFT     = 7,
    CENTER      = 8,
};

int main(void)
{
    signal(SIGINT, exit_handler);
    libusb_init(NULL);

    struct libusb_device_handle *h = NULL;
    h = get_sidewinder_handle();
    if (!h)
        running = false;

    struct sidewinder state = {0};

    while (running)
    {
        read_state(h, &state);
        printf("                   \r");

        switch (state.buttons_1.pad)
        {
        case UP:
            printf("UP");
            break;
        case UP_RIGHT:
            printf("UP_RIGHT");
            break;
        case RIGHT:
            printf("RIGHT");
            break;
        case DOWN_RIGHT:
            printf("DOWN_RIGHT");
            break;
        case DOWN:
            printf("DOWN");
            break;
        case DOWN_LEFT:
            printf("DOWN_LEFT");
            break;
        case LEFT:
            printf("LEFT");
            break;
        case UP_LEFT:
            printf("UP_LEFT");
            break;
        case CENTER:
            printf("CENTER");
            break;
        }

        putchar(' ');

        if (state.buttons_1.a)
            putc('A', stdout);
        if (state.buttons_1.b)
            putc('B', stdout);
        if (state.buttons_1.c)
            putc('C', stdout);
        if (state.buttons_1.x)
            putc('X', stdout);
        putchar('\r');
        fflush(stdout);
        /* usleep(POLL_TIME); */
    }

    libusb_close(NULL);
}
