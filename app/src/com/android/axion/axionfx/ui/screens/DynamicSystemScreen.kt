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

/**
 * Dynamic System: listening device, dynamic bass strength and, for the Custom device, the
 * bass and low-mid frequencies.
 */
@Composable
fun DynamicSystemScreen(viewModel: AxionFxViewModel, onBackClick: () -> Unit) {
    BackHandler(onBack = onBackClick)

    var enabled by remember {
        mutableStateOf(viewModel.loadBoolean(EffectKeys.DYNSYS_ENABLED, EffectDefaults.DYNSYS_ENABLED))
    }
    var profile by remember {
        mutableIntStateOf(viewModel.loadInt(EffectKeys.DYNSYS_PROFILE, EffectDefaults.DYNSYS_PROFILE))
    }
    var strength by remember {
        mutableFloatStateOf(viewModel.loadInt(EffectKeys.DYNSYS_STRENGTH, EffectDefaults.DYNSYS_STRENGTH).toFloat())
    }
    var bassHz by remember {
        mutableFloatStateOf(viewModel.loadInt(EffectKeys.DYNSYS_CUSTOM_BASS_HZ, EffectDefaults.DYNSYS_CUSTOM_BASS_HZ).toFloat())
    }
    var lowMidHz by remember {
        mutableFloatStateOf(viewModel.loadInt(EffectKeys.DYNSYS_CUSTOM_LOWMID_HZ, EffectDefaults.DYNSYS_CUSTOM_LOWMID_HZ).toFloat())
    }
    val deviceNames = stringArrayResource(R.array.dynsys_device_entries)
    val deviceOptions = deviceNames.mapIndexed { i, name -> i.toString() to name }
    val isCustom = profile == deviceNames.size - 1

    AxionScaffold(title = stringResource(R.string.dynsys_screen_title), onBackClick = onBackClick) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(innerPadding)
                .padding(horizontal = 16.dp)
        ) {
            Spacer(modifier = Modifier.height(8.dp))

            PreferenceGroup(title = stringResource(R.string.dynsys_category)) {
                item {
                    SwitchPreference(
                        title = stringResource(R.string.dynsys_enable_title),
                        summary = stringResource(R.string.dynsys_enable_summary),
                        checked = enabled,
                        onCheckedChange = {
                            enabled = it
                            viewModel.interactor.setDynSysEnabled(it)
                        },
                    )
                }
                item {
                    ListPreference(
                        title = stringResource(R.string.dynsys_device_title),
                        options = deviceOptions,
                        value = profile.toString(),
                        enabled = enabled,
                        onValueChange = {
                            profile = it.toInt()
                            viewModel.interactor.setDynSysProfile(profile)
                        },
                    )
                }
                item {
                    EffectSlider(
                        title = stringResource(R.string.dynsys_strength_title),
                        summary = stringResource(R.string.dynsys_strength_summary),
                        value = strength,
                        valueRange = 0f..100f,
                        unit = "%",
                        enabled = enabled,
                        onValueChange = {
                            strength = it
                            viewModel.interactor.setDynSysStrength(it.toInt())
                        },
                    )
                }
            }

            Spacer(modifier = Modifier.height(16.dp))

            PreferenceGroup(title = stringResource(R.string.dynsys_custom_category)) {
                item {
                    EffectSlider(
                        title = stringResource(R.string.dynsys_custom_bass_title),
                        summary = stringResource(R.string.dynsys_custom_bass_summary),
                        value = bassHz,
                        valueRange = 30f..300f,
                        unit = "Hz",
                        enabled = enabled && isCustom,
                        onValueChange = {
                            bassHz = it
                            viewModel.interactor.setDynSysCustomBassHz(it.toInt())
                        },
                    )
                }
                item {
                    EffectSlider(
                        title = stringResource(R.string.dynsys_custom_lowmid_title),
                        summary = stringResource(R.string.dynsys_custom_lowmid_summary),
                        value = lowMidHz,
                        valueRange = 150f..1000f,
                        unit = "Hz",
                        enabled = enabled && isCustom,
                        onValueChange = {
                            lowMidHz = it
                            viewModel.interactor.setDynSysCustomLowMidHz(it.toInt())
                        },
                    )
                }
            }

            Spacer(modifier = Modifier.height(32.dp))
        }
    }
}
