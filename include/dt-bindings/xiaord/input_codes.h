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

/* ── Category 0: Home button positions (0x50-0x5B) ──────────────────────── */

#define INPUT_VIRTUAL_POS_0              0x50
#define INPUT_VIRTUAL_POS_1              0x51
#define INPUT_VIRTUAL_POS_2              0x52
#define INPUT_VIRTUAL_POS_3              0x53
#define INPUT_VIRTUAL_POS_4              0x54
#define INPUT_VIRTUAL_POS_5              0x55
#define INPUT_VIRTUAL_POS_6              0x56
#define INPUT_VIRTUAL_POS_7              0x57
#define INPUT_VIRTUAL_POS_8              0x58
#define INPUT_VIRTUAL_POS_9              0x59
#define INPUT_VIRTUAL_POS_10             0x5A
#define INPUT_VIRTUAL_POS_11             0x5B

/* ── Category 1: UI actions (0x5C-0x5F) ─────────────────────────────────── */

#define INPUT_VIRTUAL_SCROLL_CW          0x5C
#define INPUT_VIRTUAL_SCROLL_CCW         0x5D

/* ── Category 2: ZMK BT behavior codes ─────────────────────────────────── */
/*
 * 0x60-0x63  BT management (requires: CONFIG_ZMK_BLE)
 * 0x70-0x7B  BT_SEL n    (requires: CONFIG_ZMK_BLE)
 * 0x80-0x8B  BT_CLR n    (per-profile; leave unmapped or define via keyboard overlay)
 */

#define INPUT_VIRTUAL_ZMK_BT_CLR        0x60
#define INPUT_VIRTUAL_ZMK_BT_CLR_ALL    0x61
#define INPUT_VIRTUAL_ZMK_BT_NXT        0x62
#define INPUT_VIRTUAL_ZMK_BT_PRV        0x63

#define INPUT_VIRTUAL_ZMK_OUT_USB       0x64
#define INPUT_VIRTUAL_ZMK_OUT_BLE       0x65
#define INPUT_VIRTUAL_ZMK_OUT_TOG       0x66

#define INPUT_VIRTUAL_ZMK_BT_SEL_0      0x70
#define INPUT_VIRTUAL_ZMK_BT_SEL_1      0x71
#define INPUT_VIRTUAL_ZMK_BT_SEL_2      0x72
#define INPUT_VIRTUAL_ZMK_BT_SEL_3      0x73
#define INPUT_VIRTUAL_ZMK_BT_SEL_4      0x74
#define INPUT_VIRTUAL_ZMK_BT_SEL_5      0x75
#define INPUT_VIRTUAL_ZMK_BT_SEL_6      0x76
#define INPUT_VIRTUAL_ZMK_BT_SEL_7      0x77
#define INPUT_VIRTUAL_ZMK_BT_SEL_8      0x78
#define INPUT_VIRTUAL_ZMK_BT_SEL_9      0x79
#define INPUT_VIRTUAL_ZMK_BT_SEL_10     0x7A
#define INPUT_VIRTUAL_ZMK_BT_SEL_11     0x7B

#define INPUT_VIRTUAL_ZMK_BT_CLR_0      0x80
#define INPUT_VIRTUAL_ZMK_BT_CLR_1      0x81
#define INPUT_VIRTUAL_ZMK_BT_CLR_2      0x82
#define INPUT_VIRTUAL_ZMK_BT_CLR_3      0x83
#define INPUT_VIRTUAL_ZMK_BT_CLR_4      0x84
#define INPUT_VIRTUAL_ZMK_BT_CLR_5      0x85
#define INPUT_VIRTUAL_ZMK_BT_CLR_6      0x86
#define INPUT_VIRTUAL_ZMK_BT_CLR_7      0x87
#define INPUT_VIRTUAL_ZMK_BT_CLR_8      0x88
#define INPUT_VIRTUAL_ZMK_BT_CLR_9      0x89
#define INPUT_VIRTUAL_ZMK_BT_CLR_10     0x8A
#define INPUT_VIRTUAL_ZMK_BT_CLR_11     0x8B

/* ── Page indices for use in DTS overlays ───────────────────────────────── */

#define XIAORD_PAGE_HOME   0
#define XIAORD_PAGE_CLOCK  1
#define XIAORD_PAGE_BT     2
