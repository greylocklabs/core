package org.greylocklabs.ui

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test
import java.io.File

class ThemesTest {
  @Test
  fun everyThemeFileBecomesATheme() {
    val files = File("../../themes").listFiles { f -> f.name.endsWith(".toml") }!!.map { it.nameWithoutExtension }.sorted()
    assertEquals(files, Themes.all.map { it.id })
  }

  @Test
  fun callieReadsAsItsFileSays() {
    val callie = Themes.callie
    assertEquals("Callie", callie.name)
    assertTrue(callie.dark)
    assertEquals(Color(0xFFF08FB8), callie.colors.accent)
    assertEquals(11.dp, callie.shape.radius)
    assertEquals(3.dp, callie.shape.stickerEdge)
    assertEquals("Fraunces", callie.type.displayFamily)
    assertEquals(1.2f, callie.motion.bounce)
  }

  @Test
  fun aThemeTakesWhatItLeavesOutFromCallie() {
    val light = Themes.callieLight
    assertFalse(light.dark)
    assertEquals(Color(0xFFC2407A), light.colors.accent)
    // Callie Light sets no corners, type or motion of its own.
    assertEquals(Themes.callie.shape, light.shape)
    assertEquals(Themes.callie.motion, light.motion)
    // Minimal sets its own corners, a flat sticker and the system font.
    assertEquals(0.dp, Themes.minimal.shape.stickerEdge)
    assertEquals("", Themes.minimal.type.family)
    // It turns the bounce off but keeps Callie's durations.
    assertEquals(0f, Themes.minimal.motion.bounce)
    assertEquals(Themes.callie.motion.duration, Themes.minimal.motion.duration)
  }
}
