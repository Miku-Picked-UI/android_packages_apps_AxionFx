@file:OptIn(ExperimentalMaterial3ExpressiveApi::class)

/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.axion.axionfx.ui.screens

import androidx.activity.compose.BackHandler
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.ExperimentalMaterial3ExpressiveApi
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringArrayResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import com.android.axion.axionfx.R
import com.android.axion.axionfx.domain.EffectDefaults
import com.android.axion.axionfx.domain.EffectKeys
import com.android.axion.axionfx.ui.AxionFxViewModel
import com.android.axion.axionfx.ui.components.EffectSlider
import com.android.axion.compose.preferences.ListPreference
import com.android.axion.compose.preferences.PreferenceGroup
import com.android.axion.compose.preferences.SwitchPreference
import com.android.axion.compose.scaffold.AxionScaffold

/** Clarity: Natural, OZone+ or XHiFi treble clarity with a strength slider. */
@Composable
fun ClarityScreen(viewModel: AxionFxViewModel, onBackClick: () -> Unit) {
    BackHandler(onBack = onBackClick)

    var enabled by remember {
        mutableStateOf(viewModel.loadBoolean(EffectKeys.CLARITY_ENABLED, EffectDefaults.CLARITY_ENABLED))
    }
    var mode by remember {
        mutableIntStateOf(viewModel.loadInt(EffectKeys.CLARITY_MODE, EffectDefaults.CLARITY_MODE))
    }
    var amount by remember {
        mutableFloatStateOf(viewModel.loadInt(EffectKeys.CLARITY_AMOUNT, EffectDefaults.CLARITY_AMOUNT).toFloat())
    }
    val modeNames = stringArrayResource(R.array.clarity_mode_entries)
    val modeOptions = modeNames.mapIndexed { i, name -> i.toString() to name }

    AxionScaffold(title = stringResource(R.string.clarity_screen_title), onBackClick = onBackClick) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(innerPadding)
                .padding(horizontal = 16.dp)
        ) {
            Spacer(modifier = Modifier.height(8.dp))

            PreferenceGroup(title = stringResource(R.string.clarity_category)) {
                item {
                    SwitchPreference(
                        title = stringResource(R.string.clarity_enable_title),
                        summary = stringResource(R.string.clarity_enable_summary),
                        checked = enabled,
                        onCheckedChange = {
                            enabled = it
                            viewModel.interactor.setClarityEnabled(it)
                        },
                    )
                }
                item {
                    ListPreference(
                        title = stringResource(R.string.clarity_mode_title),
                        options = modeOptions,
                        value = mode.toString(),
                        enabled = enabled,
                        onValueChange = {
                            mode = it.toInt()
                            viewModel.interactor.setClarityMode(mode)
                        },
                    )
                }
                item {
                    EffectSlider(
                        title = stringResource(R.string.clarity_amount_title),
                        summary = stringResource(R.string.clarity_amount_summary),
                        value = amount,
                        valueRange = 0f..100f,
                        unit = "%",
                        enabled = enabled,
                        onValueChange = {
                            amount = it
                            viewModel.interactor.setClarityAmount(it.toInt())
                        },
                    )
                }
            }

            Spacer(modifier = Modifier.height(32.dp))
        }
    }
}
