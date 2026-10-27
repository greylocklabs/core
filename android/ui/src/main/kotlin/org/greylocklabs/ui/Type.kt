package org.greylocklabs.ui

import androidx.compose.material3.Typography
import androidx.compose.ui.text.ExperimentalTextApi
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontVariation
import androidx.compose.ui.text.font.FontWeight

@OptIn(ExperimentalTextApi::class)
private fun variable(res: Int, vararg weights: Int) =
  FontFamily(weights.map { w -> Font(res, FontWeight(w), variationSettings = FontVariation.Settings(FontVariation.weight(w))) })

/** Nunito for everything you read, from core/fonts under the SIL OFL. */
val Nunito = variable(R.font.nunito, 400, 500, 600, 700, 800)

/** Fraunces for display text, from core/fonts under the SIL OFL. */
val Fraunces = variable(R.font.fraunces, 400, 600, 700)

/** The family a theme names: one core ships, or the system's for any other, as on the desktop. */
fun fontFamily(name: String): FontFamily = when (name) {
  "Nunito" -> Nunito
  "Fraunces" -> Fraunces
  else -> FontFamily.Default
}

/** Material's type scale in a theme's fonts: display and headlines in its display family, the rest in its own. */
fun typography(type: GreylockType): Typography {
  val base = Typography()
  val body = fontFamily(type.family)
  val display = fontFamily(type.displayFamily)
  val strong = FontWeight(type.weightStrong)
  val bold = FontWeight(type.weightBold)
  return Typography(
    displayLarge = base.displayLarge.copy(fontFamily = display, fontWeight = strong),
    displayMedium = base.displayMedium.copy(fontFamily = display, fontWeight = strong),
    displaySmall = base.displaySmall.copy(fontFamily = display, fontWeight = strong),
    headlineLarge = base.headlineLarge.copy(fontFamily = display, fontWeight = strong),
    headlineMedium = base.headlineMedium.copy(fontFamily = display, fontWeight = strong),
    headlineSmall = base.headlineSmall.copy(fontFamily = display, fontWeight = strong),
    titleLarge = base.titleLarge.copy(fontFamily = body, fontWeight = bold),
    titleMedium = base.titleMedium.copy(fontFamily = body, fontWeight = bold),
    titleSmall = base.titleSmall.copy(fontFamily = body, fontWeight = bold),
    bodyLarge = base.bodyLarge.copy(fontFamily = body),
    bodyMedium = base.bodyMedium.copy(fontFamily = body),
    bodySmall = base.bodySmall.copy(fontFamily = body),
    labelLarge = base.labelLarge.copy(fontFamily = body, fontWeight = bold),
    labelMedium = base.labelMedium.copy(fontFamily = body, fontWeight = strong),
    labelSmall = base.labelSmall.copy(fontFamily = body, fontWeight = strong),
  )
}
