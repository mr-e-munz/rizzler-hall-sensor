/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#ifndef RIZZ_LED_CTL_H_
#define RIZZ_LED_CTL_H_

#include "stdint.h"

enum led_id {
    LED_ID_RED,
    LED_ID_ORANGE,

    LED_ID_Count,
};

int led_ctl_set( enum led_id id, uint8_t intensity );

#endif /* RIZZ_LED_CTL_H_ */