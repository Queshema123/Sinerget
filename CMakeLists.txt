cmake_minimum_required(VERSION 3.5)

project(SinergetProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(Qt6 REQUIRED COMPONENTS HttpServer)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Xml)
find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
find_package(Qt6 REQUIRED COMPONENTS Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SinergetProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
	converter.h token.h
	converter.cpp token.cpp
	server.h server.cpp
	serverwidget.h serverwidget.cpp
	filterwidget.h filterwidget.cpp
	type.h type.cpp
        searchwidget.h searchwidget.cpp




    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SinergetProject APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SinergetProject SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SinergetProject
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::HttpServer)
target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::Core5Compat)
target_link_libraries(SinergetProject PRIVATE Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SinergetProject)
endif()
set_target_properties(SinergetProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SinergetProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SinergetProject)
endif()
