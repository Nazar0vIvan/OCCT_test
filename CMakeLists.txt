cmake_minimum_required(VERSION 3.16)

project(OCCT_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2 Widgets)
qt_standard_project_setup(REQUIRES 6.8)

set(OCCT_ROOT "" CACHE PATH "Path to OCCT installation root")

if(NOT OCCT_ROOT AND DEFINED ENV{OCCT_ROOT})
  file(TO_CMAKE_PATH "$ENV{OCCT_ROOT}" OCCT_ROOT)
endif()

if(NOT OCCT_ROOT)
  message(FATAL_ERROR "OCCT_ROOT is not set. Pass -DOCCT_ROOT=... or define OCCT_ROOT environment variable.")
endif()

set(OpenCASCADE_DIR "${OCCT_ROOT}/lib/cmake/opencascade")

find_package(OpenCASCADE REQUIRED)

qt_add_executable(appOCCT_test
  qml/main.cpp
  src/3d/occcontroller.cpp
  src/3d/occcontroller.h
  src/3d/occviewwindow.cpp
  src/3d/occviewwindow.h
  src/3d/occworldaxes.h
  src/3d/occworldaxes.cpp
  src/3d/occscene.h
  src/3d/occscene.cpp
  src/3d/occviewer.h
  src/3d/occviewer.cpp
  src/3d/occviewcube.h
  src/3d/occviewcube.cpp
  src/3d/occviewport.h
  src/3d/occviewport.cpp
  src/3d/occpart.h
  src/3d/occpart.cpp
  src/3d/stepimporter.h
  src/3d/stepimporter.cpp
  src/3d/occinputcontroller.h
  src/3d/occinputcontroller.cpp
  src/3d/cachedshapeloader.h
  src/3d/cachedshapeloader.cpp
  src/robot/robottypes.h
  src/robot/model/linkmodel.h
  src/robot/model/jointmodel.h
  src/robot/model/robotmodel.h
  src/robot/model/robotstate.h
  src/robot/kinematics/robotframes.h
  src/robot/kinematics/forwardkinematics.h
  src/robot/kinematics/forwardkinematics.cpp
  src/robot//visualization/robotvisualmodel.h
  src/robot/visualization/robotsceneadapter.h
  src/robot/visualization/robotsceneadapter.cpp
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
