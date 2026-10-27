package org.greylocklabs.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ColorScheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Shapes
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.ReadOnlyComposable
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.ui.graphics.lerp

/** The theme in use, for what Material's color scheme has no name for, such as sticker edges. */
val LocalTheme = staticCompositionLocalOf { Themes.callie }

/** The colors of the theme in use. */
val LocalColors: GreylockColors
  @Composable @ReadOnlyComposable
  get() = LocalTheme.current.colors

/** Material's corners from a theme's radii: small for small things, then buttons, cards and popovers. */
fun shapes(shape: GreylockShape) = Shapes(
  extraSmall = RoundedCornerShape(shape.radiusSmall),
  small = RoundedCornerShape(shape.radiusSmall),
  medium = RoundedCornerShape(shape.radius),
  large = RoundedCornerShape(shape.radiusLarge),
  extraLarge = RoundedCornerShape(shape.radiusXlarge),
)

/** Material's color scheme from a theme's colors, with its accent as the one accent everywhere. */
fun colorScheme(theme: ThemeTokens): ColorScheme {
  val c = theme.colors
  // Selected chips and tonal buttons get an accent wash over the raised surface.
  val wash = lerp(c.surfaceAlt, c.accent, if (theme.dark) 0.28f else 0.22f)
  val base = if (theme.dark) darkColorScheme() else lightColorScheme()
  return base.copy(
    primary = c.accent,
    onPrimary = c.accentText,
    primaryContainer = wash,
    onPrimaryContainer = c.text,
    secondary = c.accent,
    onSecondary = c.accentText,
    secondaryContainer = wash,
    onSecondaryContainer = c.text,
    tertiary = c.accent,
    onTertiary = c.accentText,
    background = c.background,
    onBackground = c.text,
    surface = c.background,
    onSurface = c.text,
    surfaceVariant = c.surfaceAlt,
    onSurfaceVariant = c.textMuted,
    surfaceContainerLowest = c.background,
    surfaceContainerLow = c.surface,
    surfaceContainer = c.surface,
    surfaceContainerHigh = c.surfaceAlt,
    surfaceContainerHighest = c.surfaceAlt,
    surfaceBright = c.surfaceAlt,
    surfaceDim = c.background,
    outline = c.border,
    outlineVariant = c.hairline,
    error = c.danger,
    onError = c.accentText,
  )
}

/** Draws [content] in [theme]: Callie, or Callie Light when the system is light, unless another is given. */
@Composable
fun GreylockTheme(theme: ThemeTokens = if (isSystemInDarkTheme()) Themes.callie else Themes.callieLight, content: @Composable () -> Unit) {
  CompositionLocalProvider(LocalTheme provides theme) {
    MaterialTheme(colorScheme = colorScheme(theme), typography = typography(theme.type), shapes = shapes(theme.shape), content = content)
  }
}
