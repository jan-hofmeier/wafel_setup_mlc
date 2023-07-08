#include <stdint.h>
#include "bsp.h"

int SetNotificationLED(uint8_t mask)
{
    return bspWrite("SMC", 0, "NotificationLED", 1, &mask);
}