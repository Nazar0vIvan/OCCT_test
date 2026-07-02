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

  src/3d/mathtypes.h
  src/3d/utils.h
  src/3d/utils.cpp
  src/3d/occpartmodel.h

  src/3d/occt/occcontroller.cpp
  src/3d/occt/occcontroller.h
  src/3d/occt/occviewwindow.cpp
  src/3d/occt/occviewwindow.h
  src/3d/occt/occworldaxes.h
  src/3d/occt/occworldaxes.cpp
  src/3d/occt/occscene.h
  src/3d/occt/occscene.cpp
  src/3d/occt/occutils.h
  src/3d/occt/occviewer.h
  src/3d/occt/occviewer.cpp
  src/3d/occt/occviewcube.h
  src/3d/occt/occviewcube.cpp
  src/3d/occt/occviewport.h
  src/3d/occt/occviewport.cpp
  src/3d/occt/occpart.h
  src/3d/occt/occpart.cpp
  src/3d/occt/stepimporter.h
  src/3d/occt/stepimporter.cpp
  src/3d/occt/occinputcontroller.h
  src/3d/occt/occinputcontroller.cpp
  src/3d/occt/cachedshapeloader.h
  src/3d/occt/cachedshapeloader.cpp

  src/3d/robot/model/jointmodel.h
  src/3d/robot/model/robotmodel.h

  src/3d/robot/model/linkmodel.h

  #src/3d/robot/kinematics/robotframes.h
  #src/3d/robot/kinematics/forwardkinematics.h
  #src/3d/robot/kinematics/forwardkinematics.cpp

  src/3d/robot/visualization/robotsceneadapter.h
  src/3d/robot/visualization/robotsceneadapter.cpp
)

qt_add_qml_module(appOCCT_test
  URI OCCT_test
  VERSION 1.0
  QML_FILES
    qml/Main.qml
    SOURCES src/3d/robot/visualization/robotsceneapi.h
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
