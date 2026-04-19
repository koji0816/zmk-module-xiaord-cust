/*
 * SPDX-License-Identifier: MIT
 * 
 * Stub functions for prospector-zmk-module.
 * The prospector module relies on some functions provided by its original shield.
 * Since we are using xiaord shield as a scanner, we provide dummy implementations here.
 */

#include <zephyr/kernel.h>
#include <stdint.h>

/* Forward declarations to avoid including prospector-specific headers */
struct zmk_status_adv_data;

int scanner_msg_send_keyboard_data(const struct zmk_status_adv_data *data, int8_t rssi, const char *device_name, const uint8_t *ble_addr, uint8_t addr_type) {
    return 0;
}

int scanner_msg_send_timeout_check(void) {
    return 0;
}

void scanner_update_keyboard_name_by_addr(const uint8_t *addr, const char *name) {
    // Do nothing
}
