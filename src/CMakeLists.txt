cmake_minimum_required (VERSION 3.0)

project (bezav)

set(GLAD_DIR /Users/benroye/forge/glsandbox/gladHello/glad)
set(OBJPARS_DIR /Users/benroye/forge/csandbox/objpars)
include_directories(${GLAD_DIR}/build/include /opt/local/include ${OBJPARS_DIR}/src)
link_directories(${GLAD_DIR}/build/src /opt/local/lib ${OBJPARS_DIR}/build)

find_library(GLFW_LIB glfw HINTS /opt/local/lib)

message(${GLFW_LIB})

add_executable(bezav bezav.cpp glad.c linmath.h lodepng.h lodepng.cpp)
target_link_libraries(bezav ${GLFW_LIB} m ObjParser)
