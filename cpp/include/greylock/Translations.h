#pragma once

class QCoreApplication;

namespace greylock {

/// Puts the app's words in place, from the files greylock_add_translations() compiles in: the
/// desktop's language where the app has it, else English as its file words it, which falls back
/// to the words in the code. Call it right after making the application.
void installTranslations(QCoreApplication &app);

} // namespace greylock
