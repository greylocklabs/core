# greylock_add_translations(<target> SOURCE_TARGETS <targets>... [DIRECTORY <dir>])
#
# Keeps every qsTr() and tr() string of the source targets, and of greylock::core, in
# <dir>/<GREYLOCK_APP_DIR>_<language>.ts, translations/ by default, and compiles them into
# <target> at :/i18n, where greylock::installTranslations() finds them. English is always there:
# its translations reword the app without touching the code. Another language is another file
# beside it, found at configure time.
#
# The update_translations target reads the strings out of the code again, and the
# translations_current test fails while the files and the code disagree.
function(greylock_add_translations target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "DIRECTORY" "SOURCE_TARGETS")
    if(NOT arg_SOURCE_TARGETS)
        message(FATAL_ERROR "greylock_add_translations: SOURCE_TARGETS is required")
    endif()
    if(NOT arg_DIRECTORY)
        set(arg_DIRECTORY "${CMAKE_SOURCE_DIR}/translations")
    endif()
    find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

    set(english "${arg_DIRECTORY}/${GREYLOCK_APP_DIR}_en.ts")
    file(GLOB others CONFIGURE_DEPENDS "${arg_DIRECTORY}/${GREYLOCK_APP_DIR}_*.ts")
    set(ts_files "${english}" ${others})
    list(REMOVE_DUPLICATES ts_files)
    # Named here, as the test reads the project file Qt names after it.
    set(lupdate "${GREYLOCK_APP_DIR}_lupdate")
    qt_add_translations(${target}
        SOURCE_TARGETS ${arg_SOURCE_TARGETS} greylock_core
        TS_FILES ${ts_files}
        RESOURCE_PREFIX /i18n
        LUPDATE_TARGET ${lupdate}
        LUPDATE_OPTIONS -locations none -no-obsolete)

    if(TARGET Qt6::lupdate)
        add_test(NAME translations_current
            COMMAND sh "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations_test.sh"
                $<TARGET_FILE:Qt6::lupdate> "${CMAKE_COMMAND}"
                "${Qt6LinguistTools_DIR}/GenerateLUpdateProject.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/.lupdate/${lupdate}_project.cmake"
                ${ts_files})
        set_tests_properties(translations_current PROPERTIES SKIP_RETURN_CODE 77)
    endif()
endfunction()
