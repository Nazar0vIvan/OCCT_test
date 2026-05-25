cmake_minimum_required(VERSION 3.16)

project(OCCT_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2 Widgets)
qt_standard_project_setup(REQUIRES 6.8)


set(OpenCASCADE_DIR "E:/Qt/QtProjects/libs/OCCT-7_9_3/occt-install/cmake")

find_package(OpenCASCADE REQUIRED)

qt_add_executable(appOCCT_test
    qml/main.cpp
    src/occcontroller.cpp
    src/occcontroller.h
    src/occviewwindow.cpp
    src/occviewwindow.h
    src/occworldaxes.h
    src/occworldaxes.cpp
)

qt_add_qml_module(appOCCT_test
    URI OCCT_test
    VERSION 1.0
    QML_FILES
      qml/Main.qml
)

target_compile_definitions(appOCCT_test PRIVATE
  OCCT_TEST_CAD_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/cad"
)

target_include_directories(appOCCT_test PRIVATE
    ${OpenCASCADE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(appOCCT_test
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
)

include(GNUInstallDirs)
install(TARGETS appOCCT_test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
