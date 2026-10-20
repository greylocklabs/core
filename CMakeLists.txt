cmake_minimum_required(VERSION 3.28)

# The greylock::core library the apps link, with the built-in themes compiled in.
# An app adds it with add_subdirectory(core); on its own it builds the library and
# its tests, as core's CI does.
project(greylock LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Test)
    qt_standard_project_setup(REQUIRES 6.8)
    option(GREYLOCK_WERROR "Treat compiler warnings as errors" OFF)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wall -Wextra -Wpedantic)
        if(GREYLOCK_WERROR)
            add_compile_options(-Werror)
        endif()
    endif()
endif()

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui)
find_package(tomlplusplus REQUIRED)

add_library(greylock_core STATIC
    cpp/include/greylock/Base16.h
    cpp/src/Base16.cpp
    cpp/include/greylock/Color.h
    cpp/src/Color.cpp
    cpp/include/greylock/ThemeLoader.h
    cpp/src/ThemeLoader.cpp
    cpp/include/greylock/ThemeSpec.h
)
add_library(greylock::core ALIAS greylock_core)
# Required here too, so it holds when an app adds core before setting the standard.
target_compile_features(greylock_core PUBLIC cxx_std_20)
target_include_directories(greylock_core PUBLIC cpp/include)
target_link_libraries(greylock_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
    PRIVATE
        tomlplusplus::tomlplusplus
)

# Built-in themes are compiled in, so an app works uninstalled and a broken install
# can never leave it without its default look.
file(GLOB greylock_themes RELATIVE "${PROJECT_SOURCE_DIR}" CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/themes/*.toml")
qt_add_resources(greylock_core "greylock_themes"
    PREFIX "/greylock"
    BASE "${PROJECT_SOURCE_DIR}"
    FILES ${greylock_themes}
)

option(GREYLOCK_BUILD_TESTS "Build core's tests" ${PROJECT_IS_TOP_LEVEL})
if(GREYLOCK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(cpp/tests)
endif()
