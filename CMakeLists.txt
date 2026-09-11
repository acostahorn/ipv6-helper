cmake_minimum_required(VERSION 3.16)

project(ipv6_helper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(ipv6_helper
    main.cpp
    ipv6Helper.cpp
    ipv6Helper.h
    ipv6Parser.cpp
    ipv6Parser.h
    # style.qss removed from here!
)

# 🎁 Bundle style.qss directly into the binary
qt_add_resources(ipv6_helper "app_resources"
    PREFIX "/"
    FILES
        style.qss
)

add_executable(cli_test
    cli_test.cpp
    ipv6Parser.cpp
)

target_link_libraries(ipv6_helper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(ipv6_helper PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ipv6_helper
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)