/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#include "zephyr/kernel.h"
#include "zephyr/drivers/gpio.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( hall_sensor, CONFIG_APP_LOG_LEVEL );

#include "led_ctl.h"

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define HALL_SW_NODE DT_ALIAS(hall_sw)
static const struct  gpio_dt_spec hall_sw = GPIO_DT_SPEC_GET(HALL_SW_NODE, gpios);
static struct        gpio_callback hall_sw_cb_data;
static uint8_t 		 hall_activated = 0;

static void hall_state_changed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	LOG_INF("Hall changed at %" PRIu32 "", k_cycle_get_32());
	hall_activated = (uint8_t)gpio_pin_get_dt(&hall_sw);
	led_ctl_set(LED_ID_RED, hall_activated ? 255 : 0);
}

uint8_t hall_sensor_get_state(void)
{
	return hall_activated;
}

static int hall_sensor_init(void)
{
    int ret;

	if (!gpio_is_ready_dt(&hall_sw)) {
		LOG_INF("Error: Hall switch %s is not ready", hall_sw.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&hall_sw, GPIO_INPUT | GPIO_PULL_UP );
	if (ret != 0) {
		LOG_INF("Error %d: failed to configure %s pin %d",
		       ret, hall_sw.port->name, hall_sw.pin);
		return -ENODEV;
	}

	ret = gpio_pin_interrupt_configure_dt(&hall_sw,
					      GPIO_INT_EDGE_BOTH);
	if (ret != 0) {
		LOG_INF("Error %d: failed to configure interrupt on %s pin %d",
			ret, hall_sw.port->name, hall_sw.pin);
		return -ENODEV;
	}

	gpio_init_callback(&hall_sw_cb_data, hall_state_changed, BIT(hall_sw.pin));
	gpio_add_callback(hall_sw.port, &hall_sw_cb_data);
	LOG_INF("Set up hall switch at %s pin %d", hall_sw.port->name, hall_sw.pin);

    return 0;
}

SYS_INIT( hall_sensor_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY );
