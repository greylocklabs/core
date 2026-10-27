package org.greylocklabs.ui

import androidx.compose.runtime.Immutable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.Dp

/** A theme from core/themes: its colors, corners, type and motion. [Themes] has the built-in ones. */
@Immutable
data class ThemeTokens(
  /** The theme file's name, such as "callie-light". */
  val id: String,
  val name: String,
  val dark: Boolean,
  val colors: GreylockColors,
  val shape: GreylockShape,
  val type: GreylockType,
  val motion: GreylockMotion,
)

/** A theme's [colors]: neutrals, one accent, and the edges raised buttons sit on. */
@Immutable
data class GreylockColors(
  val background: Color,
  val surface: Color,
  val surfaceAlt: Color,
  val hairline: Color,
  val border: Color,
  val text: Color,
  val textMuted: Color,
  val textFaint: Color,
  val accent: Color,
  val accentText: Color,
  val danger: Color,
  val edge: Color,
  val accentEdge: Color,
)

/** A theme's corner radii, and how far a sticker button stands on its edge; 0 draws it flat. */
@Immutable
data class GreylockShape(val radiusSmall: Dp, val radius: Dp, val radiusLarge: Dp, val radiusXlarge: Dp, val stickerEdge: Dp)

/** A theme's fonts, by family name with empty for the system's, and the weights it draws text in. */
@Immutable
data class GreylockType(
  val family: String,
  val displayFamily: String,
  val monoFamily: String,
  /** For strong text such as titles in a list. */
  val weightStrong: Int,
  /** For buttons and headings. */
  val weightBold: Int,
  /** For what is selected, or today. */
  val weightHeavy: Int,
)

/** A theme's durations in milliseconds, and the overshoot when something arrives; 0 turns it off. */
@Immutable
data class GreylockMotion(val durationFast: Int, val duration: Int, val durationSlow: Int, val bounce: Float)

/** A pastel button: its fill, dark ink of the same hue for text, and a deeper edge it sits on. */
@Immutable
data class Sticker(val fill: Color, val ink: Color, val edge: Color)
