/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#include "zephyr/kernel.h"
#include "zephyr/drivers/gpio.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( led_ctl, CONFIG_APP_LOG_LEVEL );

#include "led_ctl.h"

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static const struct gpio_dt_spec* leds[] = {
	&led0,
	&led1,
};

static int leds_init(void)
{
    if (!gpio_is_ready_dt(&led0)) {
		return -ENODEV;
	}
    if (!gpio_is_ready_dt(&led1)) {
		return -ENODEV;
	}

    return 0;
}



int led_ctl_set( enum led_id id, uint8_t intensity )
{
	if ( id >= LED_ID_Count ) {
		return -EINVAL;
	}

	gpio_flags_t target_lvl = 0 == intensity ? GPIO_OUTPUT_INACTIVE : GPIO_OUTPUT_ACTIVE;

	(void)gpio_pin_configure_dt(leds[id], target_lvl);

	return 0;
}

SYS_INIT( leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY );