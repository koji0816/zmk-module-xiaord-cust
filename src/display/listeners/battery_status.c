/*
 * SPDX-License-Identifier: MIT
 *
 * Peripheral battery status listener — home screen arc gauges.
 *
 * In Scanner mode, a split keyboard's central half advertises both
 * its own battery AND the peripheral half's battery in a single
 * advertisement.  We extract both from the first active keyboard's
 * zmk_status_adv_data:
 *   Arc 0  ←  data.battery_level          (central / left)
 *   Arc 1  ←  data.peripheral_battery[0]  (peripheral / right)
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

/* ── Central mode: ZMK event listener ──────────────────────────────────── */

#if !IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)

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
			if (arc) lv_arc_set_value(arc, state.level[i]);
			if (lbl) lv_label_set_text_fmt(lbl, "%d%%", state.level[i]);
		} else {
			if (arc) lv_arc_set_value(arc, 0);
			if (lbl) lv_label_set_text(lbl, "--");
		}
	}
}

ZMK_DISPLAY_WIDGET_LISTENER(periph_battery, struct periph_bat_state,
			    periph_bat_update_cb, periph_bat_get_state)
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
ZMK_SUBSCRIPTION(periph_battery, zmk_peripheral_battery_state_changed);
#endif

#endif /* !CONFIG_PROSPECTOR_MODE_SCANNER */

/* ── Scanner mode: LVGL timer polling ──────────────────────────────────── */

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)

static lv_timer_t *s_scanner_poll_timer;

/**
 * Helper: update one arc + label pair.
 */
static void update_arc(int idx, int level, bool valid)
{
	lv_obj_t *arc = s_bat_arc[idx];
	lv_obj_t *lbl = s_bat_lbl[idx];

	if (valid && level > 0) {
		if (arc) lv_arc_set_value(arc, level);
		if (lbl) lv_label_set_text_fmt(lbl, "%d%%", level);
	} else {
		if (arc) lv_arc_set_value(arc, 0);
		if (lbl) lv_label_set_text(lbl, "--");
	}
}

/**
 * Poll all scanner keyboard slots and display batteries.
 *
 * For a split keyboard the central advertises:
 *   data.battery_level         → central half battery
 *   data.peripheral_battery[0] → peripheral half battery
 *
 * We map these to Arc 0 and Arc 1 respectively.
 * If two standalone keyboards exist, we show each one's battery_level.
 */
static void scanner_poll_timer_cb(lv_timer_t *timer)
{
	int arc_used = 0;

	/* Scan all available keyboard slots */
	for (int slot = 0;
	     slot < ZMK_STATUS_SCANNER_MAX_KEYBOARDS && arc_used < BATTERY_ARC_COUNT;
	     slot++) {
		struct zmk_keyboard_status *kb = zmk_status_scanner_get_keyboard(slot);
		if (!kb || !kb->active) {
			continue;
		}

		/* Arc for this keyboard's own battery */
		update_arc(arc_used, kb->data.battery_level, true);
		arc_used++;

		/* If this is a split-central, also show peripheral battery */
		if (arc_used < BATTERY_ARC_COUNT &&
		    kb->data.peripheral_battery[0] > 0) {
			update_arc(arc_used, kb->data.peripheral_battery[0], true);
			arc_used++;
		}
	}

	/* Clear remaining arcs that have no data */
	for (int i = arc_used; i < BATTERY_ARC_COUNT; i++) {
		update_arc(i, 0, false);
	}
}

#endif /* CONFIG_PROSPECTOR_MODE_SCANNER */

/* ── Public init ────────────────────────────────────────────────────────── */

void battery_status_init(lv_obj_t **arcs, lv_obj_t **lbls)
{
	for (int i = 0; i < BATTERY_ARC_COUNT; i++) {
		s_bat_arc[i] = arcs[i];
		s_bat_lbl[i] = lbls[i];
	}

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
	/* Poll every 1 second from LVGL thread (thread-safe). */
	s_scanner_poll_timer = lv_timer_create(scanner_poll_timer_cb, 1000, NULL);
	scanner_poll_timer_cb(NULL); /* initial update */
#else
	periph_battery_init();
#endif
}

#endif /* IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) || IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER) */
