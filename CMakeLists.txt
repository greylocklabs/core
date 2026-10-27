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
    find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Network Test)
    qt_standard_project_setup(REQUIRES 6.8)
    option(GREYLOCK_WERROR "Treat compiler warnings as errors" OFF)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wall -Wextra -Wpedantic)
        if(GREYLOCK_WERROR)
            add_compile_options(-Werror)
        endif()
    endif()
endif()

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Network)
find_package(tomlplusplus REQUIRED)

# The app core is built into, set by the app before add_subdirectory(core). On its own,
# core is built for a made-up Demo app, as its tests expect.
if(PROJECT_IS_TOP_LEVEL AND NOT DEFINED GREYLOCK_APP_ID)
    set(GREYLOCK_APP_ID "org.example.Demo")
    set(GREYLOCK_APP_NAME "Demo")
    set(GREYLOCK_APP_DIR "demo")
    set(GREYLOCK_APP_ENV "DEMO")
    set(GREYLOCK_APP_REPO "greylocklabs/demo")
    set(GREYLOCK_APP_DEVEL OFF)
endif()
foreach(value GREYLOCK_APP_ID GREYLOCK_APP_NAME GREYLOCK_APP_DIR GREYLOCK_APP_ENV GREYLOCK_APP_REPO)
    if("${${value}}" STREQUAL "")
        message(FATAL_ERROR "${value} is not set; an app sets GREYLOCK_APP_ID, _NAME, _DIR, _ENV and _REPO before add_subdirectory(core)")
    endif()
endforeach()

add_library(greylock_core STATIC
    cpp/include/greylock/App.h
    cpp/src/App.cpp
    cpp/include/greylock/Base16.h
    cpp/src/Base16.cpp
    cpp/include/greylock/Color.h
    cpp/src/Color.cpp
    cpp/include/greylock/Desktop.h
    cpp/src/Desktop.cpp
    cpp/include/greylock/Identity.h
    cpp/src/Identity.cpp
    cpp/include/greylock/ReleaseNotes.h
    cpp/src/ReleaseNotes.cpp
    cpp/include/greylock/Releases.h
    cpp/src/Releases.cpp
    cpp/include/greylock/ThemeLoader.h
    cpp/src/ThemeLoader.cpp
    cpp/include/greylock/ThemeLibrary.h
    cpp/src/ThemeLibrary.cpp
    cpp/include/greylock/ThemeSpec.h
    cpp/include/greylock/UpdateCheck.h
    cpp/src/UpdateCheck.cpp
)
add_library(greylock::core ALIAS greylock_core)
# Required here too, so it holds when an app adds core before setting the standard.
target_compile_features(greylock_core PUBLIC cxx_std_20)
target_include_directories(greylock_core PUBLIC cpp/include)
target_compile_definitions(greylock_core PRIVATE
    GREYLOCK_APP_ID="${GREYLOCK_APP_ID}"
    GREYLOCK_APP_NAME="${GREYLOCK_APP_NAME}"
    GREYLOCK_APP_DIR="${GREYLOCK_APP_DIR}"
    GREYLOCK_APP_ENV="${GREYLOCK_APP_ENV}"
    GREYLOCK_APP_REPO="${GREYLOCK_APP_REPO}"
    GREYLOCK_APP_UBUNTU_DEBS=$<BOOL:${GREYLOCK_APP_UBUNTU_DEBS}>
    GREYLOCK_APP_DEVEL=$<BOOL:${GREYLOCK_APP_DEVEL}>
)
target_link_libraries(greylock_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Network
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
