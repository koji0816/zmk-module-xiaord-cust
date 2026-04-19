/*
 * SPDX-License-Identifier: MIT
 *
 * Peripheral battery status listener — home screen arc gauges.
 *
 * Extracted from home_status.c so battery and endpoint concerns are
 * in separate compilation units.
 */

#include <zephyr/kernel.h>
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) || IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
#include <zmk/status_scanner.h>
#define BATTERY_ARC_COUNT 2
#else
#include <zmk/split/central.h>
#define BATTERY_ARC_COUNT ZMK_SPLIT_CENTRAL_PERIPHERAL_COUNT
BUILD_ASSERT(IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING),
	     "battery_status requires CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING=y");
#endif

#include "battery_status.h"

/* ── Static LVGL object references ─────────────────────────────────────── */

static lv_obj_t *s_bat_arc[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];
static lv_obj_t *s_bat_lbl[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];

/* ── ZMK listener ───────────────────────────────────────────────────────── */

struct periph_bat_state {
	uint8_t level[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];
	bool valid[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];
};

static struct periph_bat_state periph_bat_get_state(const zmk_event_t *eh)
{
	struct periph_bat_state state = {};

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
	for (int i = 0; i < BATTERY_ARC_COUNT; i++) {
		uint8_t level;
		int rc = zmk_split_central_get_peripheral_battery_level(i, &level);

		state.level[i] = level;
		state.valid[i] = (rc == 0);
	}
#endif

	return state;
}

static void periph_bat_update_cb(struct periph_bat_state state)
{
	for (int i = 0; i < BATTERY_ARC_COUNT; i++) {
		lv_obj_t *arc = s_bat_arc[i];
		lv_obj_t *lbl = s_bat_lbl[i];

		if (state.valid[i]) {
			if (arc) {
				lv_arc_set_value(arc, state.level[i]);
			}
			if (lbl) {
				lv_label_set_text_fmt(lbl, "%d%%", state.level[i]);
			}
		} else {
			if (arc) {
				lv_arc_set_value(arc, 0);
			}
			if (lbl) {
				lv_label_set_text(lbl, "--");
			}
		}
	}
}

ZMK_DISPLAY_WIDGET_LISTENER(periph_battery, struct periph_bat_state,
			    periph_bat_update_cb, periph_bat_get_state)
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
ZMK_SUBSCRIPTION(periph_battery, zmk_peripheral_battery_state_changed);
#endif

/* ── Public init ────────────────────────────────────────────────────────── */

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
static void scanner_update_cb(struct zmk_status_scanner_event_data *event_data)
{
	for (int i = 0; i < BATTERY_ARC_COUNT; i++) {
		struct zmk_keyboard_status *kb = zmk_status_scanner_get_keyboard(i);
		lv_obj_t *arc = s_bat_arc[i];
		lv_obj_t *lbl = s_bat_lbl[i];

		if (kb && kb->active) {
			if (arc) lv_arc_set_value(arc, kb->data.battery_level);
			if (lbl) lv_label_set_text_fmt(lbl, "%d%%", kb->data.battery_level);
		} else {
			if (arc) lv_arc_set_value(arc, 0);
			if (lbl) lv_label_set_text(lbl, "--");
		}
	}
}
#endif

void battery_status_init(lv_obj_t **arcs, lv_obj_t **lbls)
{
	for (int i = 0; i < BATTERY_ARC_COUNT; i++) {
		s_bat_arc[i] = arcs[i];
		s_bat_lbl[i] = lbls[i];
	}

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
	zmk_status_scanner_register_callback(scanner_update_cb);
	scanner_update_cb(NULL);
#else
	periph_battery_init();
#endif
}

#endif /* IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) || IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER) */
