/* SPDX-License-Identifier: MIT */
/*
 * Unified endpoint/BLE status listener.
 *
 * Subscribes to zmk_endpoint_changed and zmk_ble_active_profile_changed
 * once, then fans out to all registered page callbacks.
 */

#pragma once

#include <lvgl.h>
#include <zmk/endpoints.h>

/* ── State struct passed to every registered callback ─────────────────── */

struct endpoint_state {
	struct zmk_endpoint_instance selected_endpoint;
	enum zmk_transport preferred_transport;
	bool active_profile_connected;
	bool active_profile_bonded;
};

typedef void (*endpoint_status_cb_t)(struct endpoint_state state);

/**
 * Register a callback to be notified on every endpoint state change.
 * The first registration also starts the ZMK event listener.
 * Must be called from the display work queue (inside a page_*_create()).
 */
void endpoint_status_register_cb(endpoint_status_cb_t cb);

/**
 * Shared text-formatting helper: write endpoint state into lbl.
 * Factored out so both home and BT pages render identical status text.
 */
void endpoint_status_update_label(lv_obj_t *lbl, struct endpoint_state state);

/**
 * Factory: create a label widget suitable for output status display.
 * Caller is responsible for positioning the returned object.
 */
lv_obj_t *create_output_status_label(lv_obj_t *parent, const lv_font_t *font);

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
struct zmk_keyboard_status;
/**
 * Update endpoint status using Prospector scanner data from keyboard advertisement.
 */
void endpoint_status_update_from_scanner(const struct zmk_keyboard_status *kb);
#endif
