cmake_minimum_required(VERSION 3.11 FATAL_ERROR)


project(DHTXX VERSION 1.0 DESCRIPTION "Library to work with DHT11 temperature and humidity sensors " LANGUAGES C CXX)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
    lgpio
    GIT_REPOSITORY https://github.com/EddyTheCo/lg.git
    GIT_TAG main
)
FetchContent_MakeAvailable(lgpio)

find_package(Qt6 COMPONENTS Core)

if (Qt6_FOUND)
    qt_standard_project_setup()
endif(Qt6_FOUND)

add_library(dhtxx src/dhtxx.cpp include/dhtxx.hpp)
target_link_libraries(dhtxx PRIVATE lgpio)
if (Qt6_FOUND)
    target_link_libraries(dhtxx PUBLIC  Qt6::Core)
endif(Qt6_FOUND)
target_include_directories(dhtxx PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dhtxx>")

qt_add_executable(testdhtxx test/testdhtxx.cpp)
target_link_libraries(testdhtxx PRIVATE dhtxx)
install(TARGETS testdhtxx DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_add_executable(dhtxxBin test/dhtxx.c)
target_link_libraries(dhtxxBin PRIVATE lgpio)
install(TARGETS dhtxxBin DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS dhtxx EXPORT dhtxxTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dhtxx
)
install(EXPORT dhtxxTargets
    FILE dhtxxTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dhtxx
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/dhtxxConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dhtxx
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dhtxxConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dhtxxConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dhtxxConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dhtxx
)
export(EXPORT dhtxxTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/dhtxxTargets.cmake"
)

