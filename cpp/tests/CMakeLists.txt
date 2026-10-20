find_package(Qt6 6.8 REQUIRED COMPONENTS Test)

function(greylock_add_test name)
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Qt6::Test greylock::core)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endfunction()

greylock_add_test(tst_color)
greylock_add_test(tst_theme)
