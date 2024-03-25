cmake_minimum_required(VERSION 3.5)

project(myUiForQQ VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


set(CMAKE_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/../clientQQ/src")
include(${CMAKE_CURRENT_LIST_DIR}/../clientQQ/src/myClient.cmake)

#add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES} )

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Logindialog.ui
        Logindialog.cpp
        Logindialog.h
        ${CLIENT}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(myUiForQQ
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(myUiForQQ SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(myUiForQQ
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(myUiForQQ PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(myUiForQQ PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(myUiForQQ)
endif()
