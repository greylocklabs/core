find_package(Qt6 6.8 REQUIRED COMPONENTS Test)

function(greylock_add_test name)
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Qt6::Test greylock::core)
    add_test(NAME ${name} COMMAND ${name})
    # Tests that want a Flatpak say so.
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen;DEMO_FLATPAK=0")
endfunction()

greylock_add_test(tst_color)
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
