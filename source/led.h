#ifndef LED_H
#define LED_H

#include <stdint.h>

int SetNotificationLED(uint8_t mask);

enum {
    NOTIF_LED_OFF               = 0,
    NOTIF_LED_ORANGE_BLINKING   = 1 << 0,
    NOTIF_LED_ORANGE            = 1 << 1,
    NOTIF_LED_RED_BLINKING      = 1 << 2,
    NOTIF_LED_RED               = 1 << 3,
    NOTIF_LED_BLUE_BLINKING     = 1 << 4,
    NOTIF_LED_BLUE              = 1 << 5,
};

#endif // LED_H