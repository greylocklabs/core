plugins {
  alias(libs.plugins.android.library) apply false
  alias(libs.plugins.compose.compiler) apply false
  alias(libs.plugins.spotless)
}

// Kotlin style is ktlint's, configured in .editorconfig. `spotlessApply` formats; CI runs `spotlessCheck`.
spotless {
  kotlin {
    target("ui/src/**/*.kt")
    ktlint(libs.versions.ktlint.get())
  }
  kotlinGradle {
    target("*.gradle.kts", "ui/*.gradle.kts")
    ktlint(libs.versions.ktlint.get())
  }
}
