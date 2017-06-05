cmake_minimum_required (VERSION 3.0)

project (bezav)

set(BEZAV_SOURCE_DIR /users/benroye/forge/glsandbox/bezav/src)
set(BEZAV_BINARY_DIR /users/benroye/forge/glsandbox/bezav/build)
set(BEZAV_VERSION_MAJOR 0)
set(BEZAV_VERSION_MINOR 1)
# configure a header file to pass some of the CMake settings to the source
configure_file (
	"${BEZAV_SOURCE_DIR}/include/bezavCMakeConfig.h.in"
	"${BEZAV_BINARY_DIR}/bezavCMakeConfig.h"
	)

set(GLAD_DIR /Users/benroye/forge/glsandbox/gladHello/glad)
set(OBJPARS_DIR /Users/benroye/forge/csandbox/objpars)
include_directories(${GLAD_DIR}/build/include /opt/local/include ${OBJPARS_DIR}/src ${BEZAV_BINARY_DIR})
link_directories(${GLAD_DIR}/build/src /opt/local/lib ${OBJPARS_DIR}/build)

find_library(GLFW_LIB glfw HINTS /opt/local/lib)

message(${GLFW_LIB})

add_executable(bezav bezav.cpp glad.c linmath.h lodepng.h lodepng.cpp)
target_link_libraries(bezav ${GLFW_LIB} m ObjParser)
