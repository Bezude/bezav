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

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
set(GLAD_DIR /Users/benroye/forge/glsandbox/gladHello/glad)
set(OBJPARS_DIR /Users/benroye/forge/csandbox/objpars)
include_directories(${GLAD_DIR}/build/include /opt/local/include ${OBJPARS_DIR}/src ${BEZAV_BINARY_DIR} ${BEZAV_SOURCE_DIR}/include ${GTK3_INCLUDE_DIRS})
link_directories(${GLAD_DIR}/build/src /opt/local/lib ${OBJPARS_DIR}/build ${GTK3_LIBRARY_DIRS})

find_library(GLFW_LIB glfw HINTS /opt/local/lib)

message(${GLFW_LIB})

add_executable(bezav bezav.cpp glad.c lodepng.cpp)
target_link_libraries(bezav ${GLFW_LIB} ObjParser ${GTK3_LIBRARIES})
