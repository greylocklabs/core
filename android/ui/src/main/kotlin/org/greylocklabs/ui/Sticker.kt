package org.greylocklabs.ui

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.spring
import androidx.compose.foundation.interaction.InteractionSource
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import kotlin.math.roundToInt

/** The suite's press: quick, with a slight overshoot when something settles. */
val PRESS_SPRING = spring<Float>(dampingRatio = 0.42f, stiffness = 900f)

/** Squishes whatever it is on while [interaction] is pressed, springing back on release. */
@Composable
fun Modifier.pressBounce(interaction: InteractionSource, by: Float = 0.06f): Modifier {
  val pressed by interaction.collectIsPressedAsState()
  val scale by animateFloatAsState(if (pressed) 1f - by else 1f, PRESS_SPRING, label = "bounce")
  return graphicsLayer {
    scaleX = scale
    scaleY = scale
  }
}

/** A filter chip that squishes when pressed. */
@Composable
fun GreylockChip(selected: Boolean, onClick: () -> Unit, label: @Composable () -> Unit, modifier: Modifier = Modifier) {
  val interaction = remember { MutableInteractionSource() }
  FilterChip(selected = selected, onClick = onClick, label = label, modifier = modifier.pressBounce(interaction), interactionSource = interaction)
}

/**
 * A button raised on an edge in a darker shade, as the apps draw everything you can press, by the theme's sticker
 * edge and radius. It sinks onto its edge while pressed. Defaults to the accent, the primary action.
 */
@Composable
fun StickerButton(
  onClick: () -> Unit,
  modifier: Modifier = Modifier,
  sticker: Sticker? = null,
  radius: Dp = LocalTheme.current.shape.radius,
  contentPadding: PaddingValues = ButtonDefaults.ContentPadding,
  content: @Composable RowScope.() -> Unit,
) {
  val colors = LocalColors
  val fill = sticker?.fill ?: colors.accent
  val ink = sticker?.ink ?: colors.accentText
  val edge = sticker?.edge ?: colors.accentEdge
  val edgeHeight = LocalTheme.current.shape.stickerEdge
  val interaction = remember { MutableInteractionSource() }
  val pressed by interaction.collectIsPressedAsState()
  // Sinks onto its edge and squishes a touch while held, then springs back past rest on release.
  val sink by animateFloatAsState(if (pressed) 1f else 0f, PRESS_SPRING, label = "sink")
  Box(
    modifier.graphicsLayer {
      val squish = 1f - 0.025f * sink
      scaleX = squish
      scaleY = squish
    }.drawBehind {
      val r = radius.toPx()
      drawRoundRect(edge, topLeft = Offset(0f, edgeHeight.toPx()), size = Size(size.width, size.height - edgeHeight.toPx()), cornerRadius = CornerRadius(r, r))
    }.padding(bottom = edgeHeight),
    propagateMinConstraints = true,
  ) {
    Button(
      onClick = onClick,
      modifier = Modifier.offset { IntOffset(0, (edgeHeight.toPx() * sink).roundToInt()) },
      shape = RoundedCornerShape(radius),
      colors = ButtonDefaults.buttonColors(containerColor = fill, contentColor = ink),
      elevation = null,
      contentPadding = contentPadding,
      interactionSource = interaction,
      content = content,
    )
  }
}

/** The neutral sticker: raised surface, body text, dark edge. */
val neutralSticker: Sticker
  @Composable get() = LocalColors.let { Sticker(it.surfaceAlt, it.text, it.edge) }

/** The back control: a small neutral sticker with a chevron, set apart from the page's own actions. Short, it
 * says only "Back"; an editor's says "Cancel". */
@Composable
fun BackButton(onClick: () -> Unit, modifier: Modifier = Modifier, label: String = "Back") {
  StickerButton(onClick = onClick, modifier = modifier.height(40.dp), sticker = neutralSticker, contentPadding = PaddingValues(start = 10.dp, end = 14.dp)) {
    Text("‹", style = MaterialTheme.typography.titleLarge, modifier = Modifier.padding(end = 6.dp))
    Text(label, style = MaterialTheme.typography.labelLarge)
  }
}
