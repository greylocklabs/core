find_package(Qt6 6.8 REQUIRED COMPONENTS Test)

# Tests marked BUS need a session bus, and run in a private one under dbus-run-session where
# it is installed; without one they skip.
find_program(DBUS_RUN_SESSION dbus-run-session)

function(greylock_add_test name)
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Qt6::Test greylock::core)
    if("BUS" IN_LIST ARGN AND DBUS_RUN_SESSION)
        add_test(NAME ${name} COMMAND ${DBUS_RUN_SESSION} -- $<TARGET_FILE:${name}>)
    else()
        add_test(NAME ${name} COMMAND ${name})
    endif()
    # Tests that want a Flatpak say so.
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen;DEMO_FLATPAK=0")
endfunction()

greylock_add_test(tst_color)
greylock_add_test(tst_desktop)
greylock_add_test(tst_theme)
greylock_add_test(tst_identity)
greylock_add_test(tst_themelibrary)
greylock_add_test(tst_releasenotes)
# The Demo app's changelog, where an app builds its own in.
qt_add_resources(tst_releasenotes "demo_changelog"
    PREFIX "/demo"
    BASE data
    FILES data/CHANGELOG.md
)
greylock_add_test(tst_releases)
greylock_add_test(tst_updatecheck)
greylock_add_test(tst_autostart)
greylock_add_test(tst_notifications)
greylock_add_test(tst_singleinstance BUS)
greylock_add_test(tst_startatlogin BUS)
greylock_add_test(tst_trayitem BUS)
