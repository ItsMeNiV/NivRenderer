# CMakeList.txt : CMake project for NivRenderer, include source and define
# project specific logic here.
#

set(INCLUDE_DIRS
"src"
"vendor/glfw/include"
"vendor/glad/include"
"vendor/glm"
"vendor/OpenGLStarter"
"vendor/stb_image"
)

set(SRC_FILES
"src/NivRenderer.cpp" "src/NivRenderer.h"
)

#GLFW
add_subdirectory("vendor/glfw")
#GLM
add_subdirectory("vendor/glm")

add_executable (${CMAKE_PROJECT_NAME} ${SRC_FILES} "vendor/glad/src/glad.c" "vendor/stb_image/stb_image.cpp" "vendor/stb_image/stb_image.h")

include_directories(${INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${CMAKE_PROJECT_NAME} copy_assets)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()