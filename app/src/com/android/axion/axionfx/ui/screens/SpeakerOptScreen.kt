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
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import com.android.axion.axionfx.R
import com.android.axion.axionfx.domain.EffectDefaults
import com.android.axion.axionfx.domain.EffectKeys
import com.android.axion.axionfx.ui.AxionFxViewModel
import com.android.axion.axionfx.ui.components.EffectSlider
import com.android.axion.compose.preferences.PreferenceGroup
import com.android.axion.compose.preferences.SwitchPreference
import com.android.axion.compose.scaffold.AxionScaffold

/** Speaker optimization: correction for the built-in phone speaker. */
@Composable
fun SpeakerOptScreen(viewModel: AxionFxViewModel, onBackClick: () -> Unit) {
    BackHandler(onBack = onBackClick)

    var enabled by remember {
        mutableStateOf(viewModel.loadBoolean(EffectKeys.SPKOPT_ENABLED, EffectDefaults.SPKOPT_ENABLED))
    }
    var amount by remember {
        mutableFloatStateOf(viewModel.loadInt(EffectKeys.SPKOPT_AMOUNT, EffectDefaults.SPKOPT_AMOUNT).toFloat())
    }

    AxionScaffold(title = stringResource(R.string.spkopt_screen_title), onBackClick = onBackClick) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(innerPadding)
                .padding(horizontal = 16.dp)
        ) {
            Spacer(modifier = Modifier.height(8.dp))

            PreferenceGroup(title = stringResource(R.string.spkopt_category)) {
                item {
                    SwitchPreference(
                        title = stringResource(R.string.spkopt_enable_title),
                        summary = stringResource(R.string.spkopt_enable_summary),
                        checked = enabled,
                        onCheckedChange = {
                            enabled = it
                            viewModel.interactor.setSpeakerOptEnabled(it)
                        },
                    )
                }
                item {
                    EffectSlider(
                        title = stringResource(R.string.spkopt_amount_title),
                        summary = stringResource(R.string.spkopt_amount_summary),
                        value = amount,
                        valueRange = 0f..100f,
                        unit = "%",
                        enabled = enabled,
                        onValueChange = {
                            amount = it
                            viewModel.interactor.setSpeakerOptAmount(it.toInt())
                        },
                    )
                }
            }

            Spacer(modifier = Modifier.height(32.dp))
        }
    }
}
