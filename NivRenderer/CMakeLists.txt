# CMakeList.txt : CMake project for NivRenderer, include source and define
# project specific logic here.
#

set(LOG_LEVEL "DEBUG")

set(DEAR_IMGUI_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
    CACHE STRING "Path to Dear ImGui"
)

set(INCLUDE_DIRS
"src"
"vendor/glfw/include"
"vendor/glad/include"
"vendor/glm"
"vendor/OpenGLStarter"
"vendor/stb_image"
"vendor/imgui"
"vendor/assimp/include"
)

set(SRC_FILES
 "src/Base.h"
 "src/NivRenderer.cpp" "src/NivRenderer.h"
 "src/Application/Event.h"
 "src/Application/Window/Window.cpp" "src/Application/Window/Window.h" 
 "src/Application/Window/SceneHierarchy.h"
 "src/Application/Window/Properties.h"
 "src/Application/Window/RenderWindow.h"
 "src/Application/Window/CameraControllerArcball.h" "src/Application/Window/CameraControllerArcball.cpp"
 "src/Application/Window/WindowEvents.h"
 "src/Application/Input.h"
 "src/Rendering/Renderer.cpp" "src/Rendering/Renderer.h"
 "src/Entity/ECSRegistry.cpp" "src/Entity/ECSRegistry.h"
 "src/Entity/Entity.h"
 "src/Entity/Component.h"
 "src/Entity/Assets/AssetManager.h" "src/Entity/Assets/AssetManager.cpp"
 "src/Entity/Assets/MeshAsset.h" "src/Entity/Assets/MeshAsset.cpp"
 "src/Entity/Assets/TextureAsset.h" "src/Entity/Assets/TextureAsset.cpp"
 "src/Entity/Components/TransformComponent.h"
 "src/Entity/Components/MeshComponent.h" "src/Entity/Components/MeshComponent.cpp"
 "src/Entity/Components/MaterialComponent.h" "src/Entity/Components/MaterialComponent.cpp"
 "src/Entity/Entities/SceneObject.h" "src/Entity/Entities/SceneObject.cpp"
 "src/Entity/Entities/CameraObject.h"
 "src/Entity/Entities/LightObject.h"
 "src/Application/Scene.h" "src/Application/Scene.cpp"
 "src/Rendering/OpenGL/Framebuffer.h" "src/Rendering/OpenGL/Framebuffer.cpp"
 "src/Rendering/OpenGL/Texture.h" "src/Rendering/OpenGL/Texture.cpp"
 "src/Rendering/RenderPipeline.cpp" "src/Rendering/RenderPipeline.h"
 "src/Rendering/RenderPass.h"
 "src/Rendering/Proxy/Proxy.h"
 "src/Rendering/Proxy/ProxyManager.cpp" "src/Rendering/Proxy/ProxyManager.h"
 "src/Rendering/Proxy/SceneObjectProxy.h" "src/Rendering/Proxy/SceneObjectProxy.cpp"
 "src/Rendering/Proxy/CameraProxy.h"
 "src/Rendering/Proxy/LightProxy.h"
 "src/Rendering/ForwardPipeline/ForwardPass.h"
 "src/Rendering/OpenGL/VertexLayout.h"
)

list(APPEND SRC_FILES
    ${DEAR_IMGUI_PREFIX}/imgui.cpp
    ${DEAR_IMGUI_PREFIX}/misc/cpp/imgui_stdlib.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_draw.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_tables.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_widgets.cpp
    ${DEAR_IMGUI_PREFIX}/backends/imgui_impl_glfw.cpp
    ${DEAR_IMGUI_PREFIX}/backends/imgui_impl_opengl3.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_demo.cpp
    "vendor/glad/src/glad.c"
    "vendor/stb_image/stb_image.cpp"
    "vendor/stb_image/stb_image.h"
)

#GLFW
add_subdirectory("vendor/glfw")
#GLM
add_subdirectory("vendor/glm")
#Assimp
add_subdirectory("vendor/assimp")
#Spdlog
add_subdirectory("vendor/spdlog")

add_executable (${CMAKE_PROJECT_NAME} ${SRC_FILES})

include_directories(${INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw assimp spdlog::spdlog_header_only)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=${LOG_LEVEL})

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${CMAKE_PROJECT_NAME} copy_assets)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()