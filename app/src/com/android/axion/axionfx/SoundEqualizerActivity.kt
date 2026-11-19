/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.axion.axionfx

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.ui.Modifier
import com.android.axion.axionfx.service.AxionFxService
import com.android.axion.axionfx.ui.screens.SoundEqScreen
import com.android.axion.axionfx.ui.screens.SoundEqState
import com.android.axion.axionfx.ui.theme.AxionFxTheme

/**
 * Miku-Picked UI: "Equalizer", a launcher app of its own for AxionFx's 9-band equalizer stage:
 * one band per octave and a handful of sound presets, in the spirit of the equalizer phones
 * ship with. It runs in AxionFx's process, so it drives the same effect directly.
 */
class SoundEqualizerActivity : ComponentActivity() {

    private lateinit var state: SoundEqState

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        AxionFxService.start(this)
        AxionFxService.primeFromContext(this)
        state = SoundEqState(AxionFxService.getPrefs(this))

        enableEdgeToEdge()
        setContent {
            AxionFxTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.surfaceContainer,
                ) {
                    SoundEqScreen(state = state, onBackClick = { finish() })
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        // A device profile may have switched the sound while we were away.
        state.reload()
    }

    override fun onDestroy() {
        state.close()
        super.onDestroy()
    }
}
