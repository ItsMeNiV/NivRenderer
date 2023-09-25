# CMakeList.txt : CMake project for NivRenderer, include source and define
# project specific logic here.
#

# ASSIMP SETTINGS
OPTION( BUILD_SHARED_LIBS
  "Build package with shared libraries."
  OFF
)
OPTION( ASSIMP_BUILD_FRAMEWORK
  "Build package as Mac OS X Framework bundle."
  OFF
)
OPTION( ASSIMP_DOUBLE_PRECISION
  "Set to ON to enable double precision processing"
  OFF
)
OPTION( ASSIMP_OPT_BUILD_PACKAGES
  "Set to ON to generate CPack configuration files and packaging targets"
  OFF
)
OPTION( ASSIMP_ANDROID_JNIIOSYSTEM
  "Android JNI IOSystem support is active"
  OFF
)
OPTION( ASSIMP_NO_EXPORT
  "Disable Assimp's export functionality."
  ON
)
OPTION( ASSIMP_BUILD_ASSIMP_TOOLS
  "If the supplementary tools for Assimp are built in addition to the library."
  OFF
)
OPTION ( ASSIMP_BUILD_SAMPLES
  "If the official samples are built as well (needs Glut)."
  OFF
)
OPTION ( ASSIMP_BUILD_TESTS
  "If the test suite for Assimp is built in addition to the library."
  OFF
)
OPTION ( ASSIMP_COVERALLS
  "Enable this to measure test coverage."
  OFF
)
OPTION( ASSIMP_INSTALL
  "Disable this if you want to use assimp as a submodule."
  ON
)
OPTION ( ASSIMP_ERROR_MAX
  "Enable all warnings."
  OFF
)
OPTION ( ASSIMP_ASAN
  "Enable AddressSanitizer."
  OFF
)
OPTION ( ASSIMP_UBSAN
  "Enable Undefined Behavior sanitizer."
  OFF
)
OPTION ( ASSIMP_BUILD_DOCS
  "Build documentation using Doxygen."
  OFF
)
OPTION( ASSIMP_INJECT_DEBUG_POSTFIX
  "Inject debug postfix in .a/.so/.dll lib names"
  ON
)
OPTION ( ASSIMP_IGNORE_GIT_HASH
   "Don't call git to get the hash."
   OFF
)
OPTION (ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT
   "Don't build all importers."
   OFF
)
OPTION (ASSIMP_BUILD_GLTF_IMPORTER
   "Don't build all importers."
   ON
)
OPTION (ASSIMP_BUILD_OBJ_IMPORTER
   "Don't build all importers."
   ON
)
OPTION (ASSIMP_BUILD_FBX_IMPORTER
   "Don't build all importers."
   ON
)
#Json Settings
set(JSON_BuildTests OFF CACHE INTERNAL "")

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
"vendor/ImGuizmo"
"vendor/assimp/include"
"vendor/json/include/nlohmann"
)

set(SRC_FILES
 "src/Base.h"
 "src/IdManager.h"
 "src/NivRenderer.cpp" "src/NivRenderer.h"
 "src/Application/Event.h"
 "src/Application/Input.h"
 "src/Application/Serialization/Serializable.h"
 "src/Application/Serialization/SerializationManager.h" "src/Application/Serialization/SerializationManager.cpp"
 "src/Application/Util/Instrumentor.h"
 "src/Application/Util/Math.h"
 "src/Application/Window/Window.cpp" "src/Application/Window/Window.h" 
 "src/Application/Window/SceneHierarchy.h"
 "src/Application/Window/Properties.h"
 "src/Application/Window/RenderWindow.h"
 "src/Application/Window/CameraControllerArcball.h" "src/Application/Window/CameraControllerArcball.cpp"
 "src/Application/Window/WindowEvents.h"
 "src/Rendering/Renderer.cpp" "src/Rendering/Renderer.h"
 "src/Entity/ECSRegistry.cpp" "src/Entity/ECSRegistry.h"
 "src/Entity/Entity.h"
 "src/Entity/Component.h"
 "src/Entity/PropertyType.h"
 "src/Entity/Asset.h"
 "src/Entity/Assets/AssetManager.h" "src/Entity/Assets/AssetManager.cpp"
 "src/Entity/Assets/MeshAsset.h" "src/Entity/Assets/MeshAsset.cpp"
 "src/Entity/Assets/TextureAsset.h" "src/Entity/Assets/TextureAsset.cpp"
 "src/Entity/Assets/MaterialAsset.h" "src/Entity/Assets/MaterialAsset.cpp"
 
 "src/Entity/Components/TransformComponent.h" "src/Entity/Components/TransformComponent.cpp"
 "src/Entity/Components/MeshComponent.h" "src/Entity/Components/MeshComponent.cpp"
 "src/Entity/Components/MaterialComponent.h" "src/Entity/Components/MaterialComponent.cpp"
 "src/Entity/Entities/SceneObject.h" "src/Entity/Entities/SceneObject.cpp"
 "src/Entity/Entities/CameraObject.h"
 "src/Entity/Entities/LightObject.h"
 "src/Entity/Entities/SkyboxObject.h" "src/Entity/Entities/SkyboxObject.cpp"
 "src/Application/Scene.h" "src/Application/Scene.cpp"
 "src/Rendering/OpenGL/Framebuffer.h" "src/Rendering/OpenGL/Framebuffer.cpp"
 "src/Rendering/OpenGL/Texture.h" "src/Rendering/OpenGL/Texture.cpp"
 "src/Rendering/RenderPipeline.cpp" "src/Rendering/RenderPipeline.h"
 "src/Rendering/RenderPass.h"
 "src/Rendering/RenderQueue.h"
 "src/Rendering/Proxy/Proxy.h"
 "src/Rendering/Proxy/ProxyManager.cpp" "src/Rendering/Proxy/ProxyManager.h"
 "src/Rendering/Proxy/SceneObjectProxy.h" "src/Rendering/Proxy/SceneObjectProxy.cpp"
 "src/Rendering/Proxy/CameraProxy.h"
 "src/Rendering/Proxy/LightProxy.h"
 "src/Rendering/Proxy/SkyboxProxy.h"
 "src/Rendering/Proxy/MeshProxy.h" "src/Rendering/Proxy/MeshProxy.cpp"
 "src/Rendering/Proxy/TextureProxy.h" "src/Rendering/Proxy/TextureProxy.cpp"
 "src/Rendering/Proxy/MaterialProxy.h" "src/Rendering/Proxy/MaterialProxy.cpp"
 "src/Rendering/ForwardPipeline/ForwardPass.h"
 
 "src/Rendering/ForwardPipeline/ForwardPass.cpp"   "src/Rendering/OpenGL/Buffer.h" "src/Rendering/OpenGL/Buffer.cpp")

list(APPEND SRC_FILES
    ${DEAR_IMGUI_PREFIX}/imgui.cpp
    ${DEAR_IMGUI_PREFIX}/misc/cpp/imgui_stdlib.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_draw.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_tables.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_widgets.cpp
    ${DEAR_IMGUI_PREFIX}/backends/imgui_impl_glfw.cpp
    ${DEAR_IMGUI_PREFIX}/backends/imgui_impl_opengl3.cpp
    ${DEAR_IMGUI_PREFIX}/imgui_demo.cpp
    "vendor/ImGuizmo/ImGuizmo.cpp"
    "vendor/ImGuizmo/ImGuizmo.h"
    "vendor/glad/src/glad.c"
    "vendor/stb_image/stb_image.cpp"
    "vendor/stb_image/stb_image.h"
    "vendor/stb_image/stb_image_resize.cpp"
    "vendor/stb_image/stb_image_resize.h"
)

#GLFW
add_subdirectory("vendor/glfw")
#GLM
add_subdirectory("vendor/glm")
#Assimp
add_subdirectory("vendor/assimp")
#Spdlog
add_subdirectory("vendor/spdlog")
#Json
add_subdirectory("vendor/json")

add_executable (${CMAKE_PROJECT_NAME} ${SRC_FILES})

include_directories(${INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw assimp spdlog::spdlog_header_only nlohmann_json::nlohmann_json)

add_compile_definitions(CMAKE_CXX_STANDARD_REQUIRED=ON)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${CMAKE_PROJECT_NAME} copy_assets)

set(CMAKE_CXX_STANDARD 20)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if(MSVC)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:RELWITHDEBINFO>:/PROFILE>)
endif()