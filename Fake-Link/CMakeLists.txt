cmake_minimum_required(VERSION 3.16)

project(Fake-Link VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#查找Qt模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Test)  # 加入 Test 模块

#独立出test文件
add_executable(simpletest
    simpletest.cpp
    simpletest.h

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    pausedialog.h pausedialog.cpp pausedialog.ui
    scoreboard.h scoreboard.cpp
    playercharacter.h playercharacter.cpp

)
target_link_libraries(simpletest
    Qt6::Core
    Qt6::Test
    Qt6::Gui
    Qt6::Widgets
)

# 添加源文件和图标
set(APP_RC "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Fake-Link
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Resources.qrc
        playerui.h playerui.cpp playerui.ui
        pausedialog.h pausedialog.cpp pausedialog.ui
        settings.h settings.cpp settings.ui
        gameSettings.h

        ${APP_RC}# 这里加入图标文件
        scoreboard.h scoreboard.cpp
        playercharacter.h playercharacter.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Fake-Link APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Fake-Link SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Fake-Link
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Fake-Link PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Fake-Link)
endif()
set_target_properties(Fake-Link PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Fake-Link
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Fake-Link)
endif()
