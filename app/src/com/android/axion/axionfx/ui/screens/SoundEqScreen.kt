/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

package com.android.axion.axionfx.ui.screens

import android.content.SharedPreferences
import androidx.compose.animation.animateColorAsState
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.spring
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.gestures.detectVerticalDragGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.hapticfeedback.HapticFeedbackType
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.LocalHapticFeedback
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.android.axion.axionfx.R
import com.android.axion.axionfx.data.EffectRepository
import com.android.axion.axionfx.domain.EffectInteractor
import com.android.axion.axionfx.domain.EffectKeys
import com.android.axion.axionfx.domain.SoundEq
import com.android.axion.axionfx.service.AxionFxService
import com.android.axion.compose.scaffold.AxionScaffold
import kotlin.math.roundToInt

/**
 * Miku-Picked UI: what the Equalizer app shows, kept in AxionFx's settings so the service
 * restores it and device profiles / presets carry it. Gains are whole dB here and tenths of a
 * dB in the settings and the engine.
 */
class SoundEqState(private val prefs: SharedPreferences) {
    private val interactor = EffectInteractor(EffectRepository(prefs))

    var preset by mutableIntStateOf(SoundEq.BALANCED)
        private set

    /** The curve on screen and in the engine, dB per band. */
    val gains = mutableStateListOf(*Array(SoundEq.BANDS) { 0 })

    /** The user's own curve, what Custom brings back. */
    private val custom = IntArray(SoundEq.BANDS)

    private val listener = SharedPreferences.OnSharedPreferenceChangeListener { _, key ->
        if (key == EffectKeys.SOUND_EQ_PRESET ||
            key?.startsWith(EffectKeys.SOUND_EQ_BAND_PREFIX) == true
        ) {
            reload()
        }
    }

    init {
        reload()
        prefs.registerOnSharedPreferenceChangeListener(listener)
    }

    fun close() = prefs.unregisterOnSharedPreferenceChangeListener(listener)

    fun reload() {
        preset = prefs.getInt(EffectKeys.SOUND_EQ_PRESET, SoundEq.BALANCED)
            .coerceIn(0, SoundEq.PRESETS.lastIndex)
        for (band in 0 until SoundEq.BANDS) {
            val db = toDb(prefs.getInt("${EffectKeys.SOUND_EQ_BAND_PREFIX}$band", 0))
            if (gains[band] != db) gains[band] = db
            custom[band] = toDb(
                prefs.getInt(
                    "${EffectKeys.SOUND_EQ_CUSTOM_PREFIX}$band",
                    if (preset == SoundEq.CUSTOM) db * 10 else 0,
                )
            )
        }
    }

    fun selectPreset(index: Int) {
        val db = if (index == SoundEq.CUSTOM) custom.copyOf() else SoundEq.PRESETS[index].db
        preset = index
        for (band in 0 until SoundEq.BANDS) gains[band] = db[band]
        push()
    }

    /**
     * Moving a band makes the curve the user's own, starting from what was shown.
     * Returns whether anything changed.
     */
    fun setBand(band: Int, db: Int): Boolean {
        val value = db.coerceIn(SoundEq.MIN_DB, SoundEq.MAX_DB)
        if (preset == SoundEq.CUSTOM && gains[band] == value) return false
        if (preset != SoundEq.CUSTOM) {
            preset = SoundEq.CUSTOM
            for (i in 0 until SoundEq.BANDS) custom[i] = gains[i]
        }
        gains[band] = value
        custom[band] = value
        interactor.setSoundEqCustom(IntArray(SoundEq.BANDS) { custom[it] * 10 })
        push()
        return true
    }

    private fun push() {
        interactor.setSoundEq(preset, IntArray(SoundEq.BANDS) { gains[it] * 10 })
    }

    fun turnOnAxionFx() {
        interactor.setMasterEnabled(true)
        AxionFxService.updateMasterEnabledFlow(true)
    }

    private fun toDb(tenths: Int) =
        (tenths / 10f).roundToInt().coerceIn(SoundEq.MIN_DB, SoundEq.MAX_DB)
}

@Composable
fun SoundEqScreen(state: SoundEqState, onBackClick: () -> Unit) {
    val masterEnabled by AxionFxService.masterEnabledFlow.collectAsState()

    AxionScaffold(
        title = stringResource(R.string.sound_eq_app_name),
        onBackClick = onBackClick,
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .verticalScroll(rememberScrollState())
                .padding(innerPadding)
                .padding(horizontal = 16.dp),
        ) {
            if (!masterEnabled) {
                FxOffCard(onTurnOn = state::turnOnAxionFx)
                Spacer(Modifier.height(12.dp))
            }

            BandCard(state)

            Spacer(Modifier.height(24.dp))

            PresetGrid(state)

            Spacer(Modifier.height(20.dp))

            Text(
                text = stringResource(SoundEq.PRESETS[state.preset].description),
                style = MaterialTheme.typography.bodyLarge,
                color = MaterialTheme.colorScheme.onSurface,
                modifier = Modifier.padding(horizontal = 24.dp),
            )

            Spacer(Modifier.height(32.dp))
        }
    }
}

@Composable
private fun FxOffCard(onTurnOn: () -> Unit) {
    Surface(
        shape = RoundedCornerShape(24.dp),
        color = MaterialTheme.colorScheme.errorContainer,
        modifier = Modifier.fillMaxWidth(),
    ) {
        Row(
            modifier = Modifier.padding(start = 20.dp, end = 12.dp, top = 12.dp, bottom = 12.dp),
            verticalAlignment = Alignment.CenterVertically,
        ) {
            Text(
                text = stringResource(R.string.sound_eq_fx_off),
                style = MaterialTheme.typography.bodyMedium,
                color = MaterialTheme.colorScheme.onErrorContainer,
                modifier = Modifier.weight(1f),
            )
            FilledTonalButton(onClick = onTurnOn) {
                Text(stringResource(R.string.sound_eq_fx_turn_on))
            }
        }
    }
}

@Composable
private fun BandCard(state: SoundEqState) {
    val grid = MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.35f)
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(32.dp))
            .background(MaterialTheme.colorScheme.surfaceContainerHigh)
            .padding(horizontal = 16.dp, vertical = 24.dp),
    ) {
        Column {
            Row(Modifier.fillMaxWidth()) {
                for (band in 0 until SoundEq.BANDS) {
                    val db = state.gains[band]
                    Text(
                        text = if (db > 0) "+$db" else "$db",
                        style = MaterialTheme.typography.labelLarge,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        textAlign = TextAlign.Center,
                        modifier = Modifier.weight(1f),
                    )
                }
            }
            Spacer(Modifier.height(12.dp))
            Box(Modifier.fillMaxWidth().height(SLIDER_HEIGHT)) {
                // Faint guides at +max, 0 and -max.
                Canvas(Modifier.fillMaxSize()) {
                    val top = KNOB_RADIUS.toPx()
                    val bottom = size.height - KNOB_RADIUS.toPx()
                    for (y in floatArrayOf(top, (top + bottom) / 2f, bottom)) {
                        drawLine(grid, Offset(0f, y), Offset(size.width, y), 1.dp.toPx())
                    }
                }
                Row(Modifier.fillMaxSize()) {
                    for (band in 0 until SoundEq.BANDS) {
                        BandSlider(
                            band = band,
                            db = state.gains[band],
                            onChange = { state.setBand(band, it) },
                            modifier = Modifier.weight(1f).fillMaxSize(),
                        )
                    }
                }
            }
            Spacer(Modifier.height(12.dp))
            Row(Modifier.fillMaxWidth()) {
                for (label in SoundEq.BAND_LABELS) {
                    Text(
                        text = label,
                        style = MaterialTheme.typography.labelLarge,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        textAlign = TextAlign.Center,
                        modifier = Modifier.weight(1f),
                    )
                }
            }
        }
    }
}

private val SLIDER_HEIGHT = 300.dp
private val KNOB_RADIUS = 11.dp

@Composable
private fun BandSlider(band: Int, db: Int, onChange: (Int) -> Boolean, modifier: Modifier) {
    val haptics = LocalHapticFeedback.current
    val scheme = MaterialTheme.colorScheme
    val level by animateFloatAsState(
        targetValue = db.toFloat(),
        animationSpec = spring(dampingRatio = 0.8f, stiffness = 900f),
        label = "band level",
    )
    // Flat bands stay grey, as the curve's resting point.
    val active by animateColorAsState(
        if (db == 0) scheme.outline else scheme.primary,
        label = "band color",
    )
    val track = scheme.outlineVariant
    val knobFill = scheme.surfaceContainerHigh
    val description = stringResource(R.string.sound_eq_band_description, SoundEq.BAND_LABELS[band], db)

    // y (px) -> whole dB, top is +max.
    fun dbAt(y: Float, height: Float, knob: Float): Int {
        val span = (height - 2 * knob).coerceAtLeast(1f)
        val fraction = ((y - knob) / span).coerceIn(0f, 1f)
        return (SoundEq.MAX_DB - fraction * (SoundEq.MAX_DB - SoundEq.MIN_DB)).roundToInt()
    }

    // The gesture handlers outlive this composition, so they call through State.
    val currentOnChange by rememberUpdatedState(onChange)
    fun set(value: Int) {
        if (currentOnChange(value)) {
            haptics.performHapticFeedback(HapticFeedbackType.TextHandleMove)
        }
    }

    Canvas(
        modifier = modifier
            .semantics { contentDescription = description }
            .pointerInput(band) {
                detectTapGestures { set(dbAt(it.y, size.height.toFloat(), KNOB_RADIUS.toPx())) }
            }
            .pointerInput(band) {
                detectVerticalDragGestures(
                    onDragStart = { set(dbAt(it.y, size.height.toFloat(), KNOB_RADIUS.toPx())) },
                ) { change, _ ->
                    change.consume()
                    set(dbAt(change.position.y, size.height.toFloat(), KNOB_RADIUS.toPx()))
                }
            },
    ) {
        val knob = KNOB_RADIUS.toPx()
        val x = size.width / 2f
        val top = knob
        val bottom = size.height - knob
        val y = top + (SoundEq.MAX_DB - level) / (SoundEq.MAX_DB - SoundEq.MIN_DB) * (bottom - top)
        val width = 4.dp.toPx()

        drawLine(track, Offset(x, top), Offset(x, bottom), width, StrokeCap.Round)
        drawLine(active, Offset(x, y), Offset(x, bottom), width, StrokeCap.Round)
        drawCircle(knobFill, knob, Offset(x, y))
        drawCircle(active, knob - 1.5.dp.toPx(), Offset(x, y), style = Stroke(3.dp.toPx()))
    }
}

@Composable
private fun PresetGrid(state: SoundEqState) {
    Column(verticalArrangement = Arrangement.spacedBy(12.dp)) {
        SoundEq.PRESETS.indices.chunked(2).forEach { row ->
            Row(horizontalArrangement = Arrangement.spacedBy(16.dp)) {
                for (index in row) {
                    PresetButton(
                        text = stringResource(SoundEq.PRESETS[index].name),
                        selected = state.preset == index,
                        onClick = { state.selectPreset(index) },
                        modifier = Modifier.weight(1f),
                    )
                }
                if (row.size == 1) Spacer(Modifier.weight(1f))
            }
        }
    }
}

@Composable
private fun PresetButton(
    text: String,
    selected: Boolean,
    onClick: () -> Unit,
    modifier: Modifier,
) {
    val scheme = MaterialTheme.colorScheme
    val container by animateColorAsState(
        if (selected) scheme.primary else scheme.surfaceContainerHighest,
        label = "preset container",
    )
    val content by animateColorAsState(
        if (selected) scheme.onPrimary else scheme.onSurface,
        label = "preset content",
    )
    Surface(
        onClick = onClick,
        shape = CircleShape,
        color = container,
        contentColor = content,
        modifier = modifier.height(60.dp),
    ) {
        Box(contentAlignment = Alignment.Center) {
            Text(
                text = text,
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.SemiBold,
                color = Color.Unspecified,
            )
        }
    }
}
