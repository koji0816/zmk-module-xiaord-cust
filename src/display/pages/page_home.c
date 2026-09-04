/*
 * SPDX-License-Identifier: MIT
 *
 * Home screen: date/time labels (upper half) + peripheral battery arc gauges
 * (lower half) + icon buttons (delegated to home_buttons.c).
 */
#include <zephyr/kernel.h>

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
#define BATTERY_ARC_COUNT 2
#elif IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#define BATTERY_ARC_COUNT ZMK_SPLIT_CENTRAL_PERIPHERAL_COUNT
#else
#define BATTERY_ARC_COUNT 0
#endif

#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <lvgl.h>
#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#include <zmk/split/central.h>
#endif
#include "page_iface.h"
#include "display_api.h"
#include "endpoint_status.h"
#include "battery_status.h"
#include "home_buttons.h"

/* ── RTC device ────────────────────────────────────────────────────────── */

static const struct device *s_rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

/* ── Widget handles ────────────────────────────────────────────────────── */

static lv_obj_t   *s_date_lbl;
static lv_obj_t   *s_time_lbl;
static lv_timer_t *s_timer;
static lv_obj_t   *s_output_lbl;
#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
static lv_obj_t   *s_layer_lbl;

void page_home_update_layer(const char *layer_name)
{
	if (s_layer_lbl) {
		lv_label_set_text(s_layer_lbl, layer_name ? layer_name : "");
	}
}
#endif

/* ── Endpoint status callback ──────────────────────────────────────────── */

static void home_endpoint_cb(struct endpoint_state state)
{
	endpoint_status_update_label(s_output_lbl, state);
}

/* ── Month / weekday name tables ───────────────────────────────────────── */

static const char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *day_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

/* ── Timer callback ────────────────────────────────────────────────────── */

static void update_datetime(lv_timer_t *t)
{
	ARG_UNUSED(t);

	struct rtc_time time = {};

	if (rtc_get_time(s_rtc, &time) != 0) {
		return;
	}

	/* "Jan 01 Mon" */
	lv_label_set_text_fmt(s_date_lbl, "%s %02d %s",
			      month_names[time.tm_mon],
			      time.tm_mday,
			      day_names[time.tm_wday]);

	/* "23:59" */
	lv_label_set_text_fmt(s_time_lbl, "%02d:%02d",
			      time.tm_hour, time.tm_min);
}

/* ── Page create ───────────────────────────────────────────────────────── */

static int page_home_create(lv_obj_t *tile)
{
	/* ── Date label — upper area ────────────────────────────────────── */
	s_date_lbl = lv_label_create(tile);
	lv_label_set_text(s_date_lbl, "--- -- ---");
	lv_obj_align(s_date_lbl, LV_ALIGN_CENTER, 0, -67);

	/* ── Time label ─────────────────────────────────────────────────── */
	s_time_lbl = lv_label_create(tile);
	lv_label_set_text(s_time_lbl, "--:--");
	lv_obj_set_style_text_font(s_time_lbl, &lv_font_montserrat_48, 0);
	lv_obj_align(s_time_lbl, LV_ALIGN_CENTER, 0, -27);

	/* ── Output status label ────────────────────────────────────────── */
	s_output_lbl = create_output_status_label(tile, &lv_font_montserrat_16);
	lv_obj_align(s_output_lbl, LV_ALIGN_BOTTOM_MID, 0, -37);

#if IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
	/* ── Layer label — between time and battery ───────────────────────── */
	s_layer_lbl = lv_label_create(tile);
	lv_label_set_text(s_layer_lbl, "");
	lv_obj_set_style_text_font(s_layer_lbl, &lv_font_montserrat_16, 0);
	lv_obj_set_style_text_color(s_layer_lbl, lv_palette_main(LV_PALETTE_TEAL), 0);
	lv_obj_align(s_layer_lbl, LV_ALIGN_CENTER, 0, 4);
#endif

	/* ── Peripheral battery arc gauges — lower half ─────────────────── */
	lv_obj_t *periph_bat_arcs[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];
	lv_obj_t *periph_bat_lbls[BATTERY_ARC_COUNT > 0 ? BATTERY_ARC_COUNT : 1];

	const int n_periph     = BATTERY_ARC_COUNT;
	const int spacing      = 74;
	const int arc_sz       = 48;
	const int center_y_off = 34;

	for (int i = 0; i < n_periph; i++) {
		/* X offset: centres the group symmetrically around x=0 */
		int x = (int)((i - (n_periph - 1) / 2.0f) * spacing);

		/* Arc widget */
		lv_obj_t *arc = lv_arc_create(tile);
		lv_obj_set_size(arc, arc_sz, arc_sz);
		lv_arc_set_range(arc, 0, 100);
		lv_arc_set_value(arc, 0);
		lv_arc_set_rotation(arc, 270);    /* start sweep at 12 o'clock */
		lv_arc_set_bg_angles(arc, 0, 360); /* full circle background */
		lv_arc_set_angles(arc, 0, 0);     /* value arc starts empty */

		/* Hide interactive knob */
		lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
		lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

		/* Background arc: dim */
		lv_obj_set_style_arc_color(arc, lv_color_hex(0x333333), LV_PART_MAIN);
		lv_obj_set_style_arc_width(arc, 4, LV_PART_MAIN);

		/* Indicator (value) arc: white */
		lv_obj_set_style_arc_color(arc, lv_color_white(), LV_PART_INDICATOR);
		lv_obj_set_style_arc_width(arc, 4, LV_PART_INDICATOR);

		/* Transparent background */
		lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, 0);

		lv_obj_align(arc, LV_ALIGN_CENTER, x, center_y_off);

		/* Percentage label inside the arc */
		lv_obj_t *lbl = lv_label_create(tile);
		lv_label_set_text(lbl, "--");
		lv_obj_align(lbl, LV_ALIGN_CENTER, x, center_y_off);

		periph_bat_arcs[i] = arc;
		periph_bat_lbls[i] = lbl;
	}

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) || IS_ENABLED(CONFIG_PROSPECTOR_MODE_SCANNER)
	endpoint_status_register_cb(home_endpoint_cb);
	battery_status_init(periph_bat_arcs, periph_bat_lbls);
#endif
	
	/* ── Button ring ─────────────────────────────────────────────────── */
	home_buttons_create(tile);

	/* 1-second timer, created paused — resumed only while page is active */
	s_timer = lv_timer_create(update_datetime, 1000, NULL);
	lv_timer_pause(s_timer);

	return 0;
}

/* ── Page lifecycle ────────────────────────────────────────────────────── */

static void page_home_enter(void)
{
	update_datetime(NULL); /* show current time immediately on entry */
	lv_timer_resume(s_timer);
	home_buttons_set_visible(false);
}

static void page_home_leave(void)
{
	lv_timer_pause(s_timer);
	home_buttons_pause();
}

/* ── Page ops ──────────────────────────────────────────────────────────── */

const struct page_ops page_home_ops = {
	.name         = "home",
	.create       = page_home_create,
	.on_enter     = page_home_enter,
	.on_leave     = page_home_leave,
};
