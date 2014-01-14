cmake_minimum_required(VERSION 2.8.9)

project(MVCDemo)

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(CGAL REQUIRED)

# CGAL: Wrong rounding: -frounding-math option with GCC
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_definitions(-frounding-math)
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES ${SRC_DIR}/glwidget.cpp
            ${SRC_DIR}/main.cpp
            ${SRC_DIR}/mainwindow.cpp
            ${SRC_DIR}/MVCCloner.cpp
            ${SRC_DIR}/sourcewidget.cpp
            ${SRC_DIR}/utils.cpp
)

set(HEADERS ${SRC_DIR}/CloningParameters.h
            ${SRC_DIR}/CoordinatesMesh.h
            ${SRC_DIR}/glwidget.h
            ${SRC_DIR}/mainwindow.h
            ${SRC_DIR}/mvcshaders.h
            ${SRC_DIR}/MVCCloner.h
            ${SRC_DIR}/sourcewidget.h
            ${SRC_DIR}/utils.h
)

# Includes
include_directories(${GLU_INCLUDE_PATH} ${CGAL_INCLUDE_DIRS})

# Add Qt resources
qt5_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/images/images.qrc
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders.qrc
)

# Create an executable.
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Use Qt modules.
qt5_use_modules(${PROJECT_NAME} Core Gui OpenGL)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${CGAL_LIBRARY} gmp)
