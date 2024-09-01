/* BLE standard advertising */

/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/bluetooth/bluetooth.h>

#include "bluetooth_adv.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( ble_adv, CONFIG_APP_LOG_LEVEL );

#include "hall_sensor.h"

/* flags format */
/*
 *  ------------------------------------------------------------------------------- 
 * |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
 *  ------------------------------------------------------------------------------- 
 * |   RSV   |   RSV   |   RSV   |   RSV   |   RSV   |       DATA VERSION          |
 *  ------------------------------------------------------------------------------- 
 * 
 * where:
 * 	RSV			 - Reseverd
 *  DATA VERSION - Payload version - starting at 0b000
 */

#define DATA_VERSION 0b000

#define MSK_DATA_VERSION 0b00000111
#define MSK_HAS_XL		 0b00100000

struct rizz_data_block {
	uint16_t manu;
	uint8_t  flags;
	uint8_t  hall_state;
} __packed;

static struct rizz_data_block rizz_ad = {
	.manu = sys_cpu_to_be16(0xffff),
	.flags = ~0,
	.hall_state = ~0,
};

static struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, &rizz_ad, sizeof(rizz_ad)),
};

static struct bt_le_adv_param adv_params[] = 
	BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | \
                    BT_LE_ADV_OPT_USE_NAME, \
                    BT_ADV_INT_MIN, \
                    BT_ADV_INT_MAX, NULL);

static void build_flags()
{
	rizz_ad.flags = DATA_VERSION;
}

static uint8_t refresh_handler()
{
	uint8_t ret = hall_sensor_get_state();
	rizz_ad.hall_state = ret;

	return ret;
}

static struct bt_data* advertising_data()
{
    return ad;
}

static size_t advertising_data_size()
{
    return ARRAY_SIZE(ad);
}

static struct bt_le_adv_param* advertising_params()
{
    return adv_params;
}

static void ble_adv_handle_data_resume()
{
    build_flags();
}

static int ble_adv_init()
{
    build_flags();

    return 0;
}

static struct ble_adv_api api = {
	.ble_adv_handle_data_resume = ble_adv_handle_data_resume,
	.ble_adv_init = ble_adv_init,
	.advertising_data = advertising_data,
	.advertising_data_size = advertising_data_size,
	.advertising_params = advertising_params,
	.refresh_handler = refresh_handler,
};

struct ble_adv_api* ble_adv_api_standard() {
	return &api;
}
