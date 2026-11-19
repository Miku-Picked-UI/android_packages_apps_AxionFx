/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.axion.axionfx.domain

import com.android.axion.axionfx.R

/** Miku-Picked UI: the bands and sound presets of the 9-band Equalizer app. */
object SoundEq {
    const val BANDS = 9
    const val MIN_DB = -10
    const val MAX_DB = 10

    val BAND_LABELS = arrayOf("63", "125", "250", "500", "1k", "2k", "4k", "8k", "16k")

    class Preset(val name: Int, val description: Int, val db: IntArray)

    const val BALANCED = 0
    const val CUSTOM = 6

    /** In button order; [CUSTOM] has no fixed curve (the user's own is stored separately). */
    val PRESETS = listOf(
        Preset(R.string.sound_eq_balanced, R.string.sound_eq_balanced_summary,
            intArrayOf(0, 0, 0, 0, 0, 0, 0, 0, 0)),
        Preset(R.string.sound_eq_bass_boost, R.string.sound_eq_bass_boost_summary,
            intArrayOf(6, 5, 3, 1, 0, 0, 0, 0, 0)),
        Preset(R.string.sound_eq_smooth, R.string.sound_eq_smooth_summary,
            intArrayOf(1, 1, 2, 1, 0, -1, -2, -3, -4)),
        Preset(R.string.sound_eq_dynamic, R.string.sound_eq_dynamic_summary,
            intArrayOf(4, 3, 0, -1, -1, 0, 2, 3, 3)),
        Preset(R.string.sound_eq_clear, R.string.sound_eq_clear_summary,
            intArrayOf(-1, -1, 0, 1, 2, 3, 3, 2, 1)),
        Preset(R.string.sound_eq_treble_boost, R.string.sound_eq_treble_boost_summary,
            intArrayOf(0, 0, 0, 0, 0, 1, 3, 5, 6)),
        Preset(R.string.sound_eq_custom, R.string.sound_eq_custom_summary, IntArray(BANDS)),
    )
}
