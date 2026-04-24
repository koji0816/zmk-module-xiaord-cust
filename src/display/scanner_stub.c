/*
 * SPDX-License-Identifier: MIT
 *
 * Scanner stub / bootstrap for xiaord shield.
 *
 * The prospector-zmk-module's status_scanner.c expects certain functions
 * to be provided by the shield (originally in prospector_scanner/src/scanner_stub.c).
 * This file provides minimal implementations and, critically:
 *  1. Calls bt_enable() to start the BLE stack (ZMK doesn't do this
 *     when CONFIG_ZMK_BLE is not set)
 *  2. Calls zmk_status_scanner_start() to begin BLE scanning
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/logging/log.h>
#include <stdint.h>

LOG_MODULE_REGISTER(scanner_stub, LOG_LEVEL_INF);

/* Forward declarations */
struct zmk_status_adv_data;
extern int zmk_status_scanner_start(void);

/* ========== Stub functions required by status_scanner.c ========== */

int scanner_msg_send_keyboard_data(const struct zmk_status_adv_data *data,
                                   int8_t rssi, const char *device_name,
                                   const uint8_t *ble_addr, uint8_t addr_type) {
    /* status_scanner.c also calls process_advertisement_with_name() directly
     * (transitional path), so the keyboards[] array in status_scanner.c
     * is already updated. We don't need to duplicate storage here. */
    return 0;
}

int scanner_msg_send_timeout_check(void) {
    return 0;
}

void scanner_update_keyboard_name_by_addr(const uint8_t *addr, const char *name) {
    /* Name updates handled in status_scanner.c process_advertisement_with_name() */
}

/* ========== BLE enable + Scanner Start ========== */

static void scanner_start_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(scanner_start_work, scanner_start_work_handler);

static void scanner_start_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    /* Step 1: Enable BLE stack if not already enabled.
     * ZMK only calls bt_enable() when CONFIG_ZMK_BLE=y.
     * For scanner-only mode we must do it ourselves. */
    int ret = bt_enable(NULL);
    if (ret && ret != -EALREADY) {
        LOG_ERR("bt_enable failed: %d — retrying in 1s", ret);
        k_work_schedule(&scanner_start_work, K_SECONDS(1));
        return;
    }
    LOG_INF("BLE stack enabled (ret=%d)", ret);

    /* Step 2: Start scanning for Prospector advertisements */
    LOG_INF("Starting BLE scanner...");
    ret = zmk_status_scanner_start();
    if (ret == 0) {
        LOG_INF("BLE scanner started successfully");
    } else {
        LOG_ERR("Failed to start BLE scanner: %d — retrying in 1s", ret);
        k_work_schedule(&scanner_start_work, K_SECONDS(1));
    }
}

static int scanner_init_start(void) {
    /* Schedule after 500ms to allow hardware to stabilise */
    k_work_schedule(&scanner_start_work, K_MSEC(500));
    return 0;
}

SYS_INIT(scanner_init_start, APPLICATION, 99);
