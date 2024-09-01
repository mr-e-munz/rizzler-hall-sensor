/* BLE control */

/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#include <zephyr/types.h>
#include <zephyr/device.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/services/hrs.h>

#include "bluetooth_adv.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER( bluetooth, CONFIG_APP_LOG_LEVEL );

struct bt_conn* active_conn = NULL;
struct ble_adv_api* active_adv_api = NULL;

static void refresh_data_work_handler(struct k_work* work);
K_WORK_DEFINE( refresh_data_work, refresh_data_work_handler );

static void refresh_timer_handler(struct k_timer * timer_id);
K_TIMER_DEFINE(refresh_timer, refresh_timer_handler, NULL);

static inline void resume_adv_data_update(void)
{
	active_adv_api->ble_adv_handle_data_resume();
	k_timer_start( &refresh_timer, K_NO_WAIT, K_USEC( ((BT_ADV_INT_MIN >> 1) - 1) * 1000 ) );
}

static inline void terminate_adv_data_update(void)
{
	k_timer_stop( &refresh_timer );
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_INF("Connection failed (err 0x%02x)", err);
		active_conn = NULL;
	} else {
		LOG_INF("Connected");
		active_conn = conn;
	}

	terminate_adv_data_update();
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason 0x%02x)", reason);
	active_conn = NULL;
	resume_adv_data_update();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void)
{
	int err;

	LOG_INF("Bluetooth initialized");

	err = bt_le_adv_start(
						active_adv_api->advertising_params(), 
						active_adv_api->advertising_data(),
						active_adv_api->advertising_data_size(),
						NULL, 0);
	if (err) {
		LOG_INF("Advertising failed to start (err %d)", err);
		return;
	}

	LOG_INF("Advertising successfully started");

	resume_adv_data_update();
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

static void refresh_data_work_handler(struct k_work* work)
{
	active_adv_api->refresh_handler();
	bt_le_adv_update_data(
		active_adv_api->advertising_data(), 
		active_adv_api->advertising_data_size(), NULL, 0
	);
}

static void refresh_timer_handler(struct k_timer * timer_id)
{
	k_work_submit( &refresh_data_work );
}

void bt_prep_for_shutdown(void)
{
	LOG_INF("Stopping advertising...");
	if ( NULL != active_conn ) {
		(void)bt_conn_disconnect(active_conn, BT_HCI_ERR_REMOTE_POWER_OFF);
	}
	bt_le_adv_stop();
}

int bt_init(void)
{
	int err;
	size_t ble_addr_cnt = -1;
	bt_addr_le_t ble_addr;

	err = bt_enable(NULL);
	if ( 0!= err ) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return 0;
	}

	active_adv_api = ble_adv_api_standard();
	
	err = active_adv_api->ble_adv_init();
	if ( 0 != err ) {
		LOG_ERR("Failed to initialise advertising (err %d)", err);
		return 0;
	}

	/* register the refresh handler */

	bt_ready();

	bt_conn_auth_cb_register(&auth_cb_display);

	bt_id_get(&ble_addr, &ble_addr_cnt);
	LOG_INF( "There are %d BLE addresses.", ble_addr_cnt );
	if ( 1 >= ble_addr_cnt ) {
		LOG_INF( "ble addr: %02x:%02x:%02x:%02x:%02x:%02x",
			ble_addr.a.val[5],
			ble_addr.a.val[4],
			ble_addr.a.val[3],
			ble_addr.a.val[2],
			ble_addr.a.val[1],
			ble_addr.a.val[0]
		);
	}

	return 0;
}

SYS_INIT( bt_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY );