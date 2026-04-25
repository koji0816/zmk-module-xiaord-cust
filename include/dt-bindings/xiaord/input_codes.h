/*
 * SPDX-License-Identifier: MIT
 *
 * Virtual input event codes for INPUT_EV_ZMK_BEHAVIORS (0xF1).
 * Zephyr reserves INPUT_EV_VENDOR_START (0xF0) .. INPUT_EV_VENDOR_STOP (0xFF)
 * for vendor use. We claim 0xF1 for behavior events.
 *
 * Three categories:
 *   INPUT_VIRTUAL_POS_<n>          — Home button positions    (0x00-0x0B)
 *   INPUT_VIRTUAL_SCROLL_*         — UI scroll actions        (0x0C-0x0D)
 *   INPUT_VIRTUAL_ZMK_<behavior>   — ZMK BT behavior codes    (0x40-0x6B)
 *
 * Usable from both C source and DTS overlays via #include.
 */

#pragma once

/* Custom event type: "invoke ZMK behavior by index".
 * Using standard INPUT_EV_KEY (0x01) to ensure ZMK input-processors route it. */
#define INPUT_EV_ZMK_BEHAVIORS 0x01

/* ── Category 0: Home button positions (0x300-0x30B) ──────────────────────── */

#define INPUT_VIRTUAL_POS_0              0x300
#define INPUT_VIRTUAL_POS_1              0x301
#define INPUT_VIRTUAL_POS_2              0x302
#define INPUT_VIRTUAL_POS_3              0x303
#define INPUT_VIRTUAL_POS_4              0x304
#define INPUT_VIRTUAL_POS_5              0x305
#define INPUT_VIRTUAL_POS_6              0x306
#define INPUT_VIRTUAL_POS_7              0x307
#define INPUT_VIRTUAL_POS_8              0x308
#define INPUT_VIRTUAL_POS_9              0x309
#define INPUT_VIRTUAL_POS_10             0x30A
#define INPUT_VIRTUAL_POS_11             0x30B

/* ── Category 1: UI actions (0x30C-0x30F) ─────────────────────────────────── */

#define INPUT_VIRTUAL_SCROLL_CW          0x30C
#define INPUT_VIRTUAL_SCROLL_CCW         0x30D

/* ── Category 2: ZMK BT behavior codes ─────────────────────────────────── */
/*
 * 0x340-0x343  BT management (requires: CONFIG_ZMK_BLE)
 * 0x350-0x35B  BT_SEL n    (requires: CONFIG_ZMK_BLE)
 * 0x360-0x36B  BT_CLR n    (per-profile; leave unmapped or define via keyboard overlay)
 */

#define INPUT_VIRTUAL_ZMK_BT_CLR        0x340
#define INPUT_VIRTUAL_ZMK_BT_CLR_ALL    0x341
#define INPUT_VIRTUAL_ZMK_BT_NXT        0x342
#define INPUT_VIRTUAL_ZMK_BT_PRV        0x343

#define INPUT_VIRTUAL_ZMK_OUT_USB       0x344
#define INPUT_VIRTUAL_ZMK_OUT_BLE       0x345
#define INPUT_VIRTUAL_ZMK_OUT_TOG       0x346

#define INPUT_VIRTUAL_ZMK_BT_SEL_0      0x350
#define INPUT_VIRTUAL_ZMK_BT_SEL_1      0x351
#define INPUT_VIRTUAL_ZMK_BT_SEL_2      0x352
#define INPUT_VIRTUAL_ZMK_BT_SEL_3      0x353
#define INPUT_VIRTUAL_ZMK_BT_SEL_4      0x354
#define INPUT_VIRTUAL_ZMK_BT_SEL_5      0x355
#define INPUT_VIRTUAL_ZMK_BT_SEL_6      0x356
#define INPUT_VIRTUAL_ZMK_BT_SEL_7      0x357
#define INPUT_VIRTUAL_ZMK_BT_SEL_8      0x358
#define INPUT_VIRTUAL_ZMK_BT_SEL_9      0x359
#define INPUT_VIRTUAL_ZMK_BT_SEL_10     0x35A
#define INPUT_VIRTUAL_ZMK_BT_SEL_11     0x35B

#define INPUT_VIRTUAL_ZMK_BT_CLR_0      0x360
#define INPUT_VIRTUAL_ZMK_BT_CLR_1      0x361
#define INPUT_VIRTUAL_ZMK_BT_CLR_2      0x362
#define INPUT_VIRTUAL_ZMK_BT_CLR_3      0x363
#define INPUT_VIRTUAL_ZMK_BT_CLR_4      0x364
#define INPUT_VIRTUAL_ZMK_BT_CLR_5      0x365
#define INPUT_VIRTUAL_ZMK_BT_CLR_6      0x366
#define INPUT_VIRTUAL_ZMK_BT_CLR_7      0x367
#define INPUT_VIRTUAL_ZMK_BT_CLR_8      0x368
#define INPUT_VIRTUAL_ZMK_BT_CLR_9      0x369
#define INPUT_VIRTUAL_ZMK_BT_CLR_10     0x36A
#define INPUT_VIRTUAL_ZMK_BT_CLR_11     0x36B

/* ── Page indices for use in DTS overlays ───────────────────────────────── */

#define XIAORD_PAGE_HOME   0
#define XIAORD_PAGE_CLOCK  1
#define XIAORD_PAGE_BT     2
