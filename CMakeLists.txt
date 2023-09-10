# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("NivRenderer")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# Check if submodules were pulled successfully
#GLFW
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#GLM
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/glm/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#OpenGLStarter
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/OpenGLStarter/OpenGLStarter.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#Imgui
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/Imgui/imgui.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#Imguizmo
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/ImGuizmo/ImGuizmo.h")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#Assimp
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/assimp/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
#Spdlog
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/vendor/spdlog/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


# Include sub-projects.
add_subdirectory ("NivRenderer")
