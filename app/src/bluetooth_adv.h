/*
 * Copyright (c) 2024 Emunz
 *
 * All rights reserved
 */

#ifndef RIZZ_BLUETOOTH_ADV_H_
#define RIZZ_BLUETOOTH_ADV_H_

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

#define BT_ADV_INT_MIN (0x664>>2) /* 1022.5 ms */
#define BT_ADV_INT_MAX (0x780>>2) /* 1200 ms */

typedef uint8_t (*data_refresh_handler)(void);

struct ble_adv_api {
    void (*ble_adv_handle_data_resume)();
    int (*ble_adv_init)();
    struct bt_data* (*advertising_data)();
    size_t (*advertising_data_size)();
    struct bt_le_adv_param* (*advertising_params)();
    data_refresh_handler refresh_handler;
};

struct ble_adv_api* ble_adv_active_api();

/* concrete api instances for resolution */
struct ble_adv_api* ble_adv_api_standard();

#endif /* RIZZ_BLUETOOTH_ADV_H_ */
