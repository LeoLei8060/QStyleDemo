cmake_minimum_required(VERSION 3.5)

project(QStyleDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(
    ${CMAKE_SOURCE_DIR}
)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        styletitlebar.h
        styletitlebar.cpp
        styletitlebar.ui
        fontStyleDefines.h
        res.qrc
)

set(THIRD_PARTY_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/Frameless/Frameless.h
    ${CMAKE_SOURCE_DIR}/3rdparty/Frameless/Frameless.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/Frameless/FrameEventFilter.h
    ${CMAKE_SOURCE_DIR}/3rdparty/Frameless/FrameEventFilter.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/qutils/FontManager.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qutils/FontManager.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/qutils/QEventForwarder.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qutils/QEventForwarder.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QStyleDemo
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QStyleDemo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QStyleDemo SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QStyleDemo
            ${PROJECT_SOURCES} ${THIRD_PARTY_SOURCES}
        )
    endif()
endif()

target_link_libraries(QStyleDemo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QStyleDemo)
endif()
set_target_properties(QStyleDemo PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QStyleDemo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QStyleDemo)
endif()
