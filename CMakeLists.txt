cmake_minimum_required(VERSION 3.23)
project(vermicelli)

include(FindPkgConfig)
find_package(SDL2pp REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)
find_package(glm REQUIRED)
find_package(X11 REQUIRED)

# Automatically download cmake-spirv with FetchContent
include(FetchContent)

FetchContent_Declare(cmake-spirv
        GIT_REPOSITORY https://github.com/liliolett/cmake-spirv.git
        GIT_TAG origin/v1
        )
FetchContent_MakeAvailable(cmake-spirv)

# Add cmake-spirv to module path
list(APPEND CMAKE_MODULE_PATH ${cmake-spirv_SOURCE_DIR}/include)

# Include required modules
include(AddSpirvModules)

# Create shader target
add_spirv_modules(shaders
        SOURCE_DIR shaders/
        BINARY_DIR shaders/
        SOURCES simple_shader.vert simple_shader.frag)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-g -O2)
add_link_options(-lXxf86vm -lXrandr -lXi)

add_executable(vermicelli main.cpp application.cpp vermicelli_window.cpp vermicelli_pipeline.cpp vermicelli_device.cpp vermicelli_swap_chain.cpp vermicelli_model.cpp)

if (THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(vermicelli PUBLIC "-pthread")
endif ()
if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(vermicelli "${CMAKE_THREAD_LIBS_INIT}")
endif ()

target_link_libraries(vermicelli glfw SDL2pp::SDL2pp glm ${CMAKE_DL_LIBS} ${glm_LIBRARIES} ${SDL2pp_LIBRARIES} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES} ${Threads_LIBRARIES})

add_dependencies(vermicelli shaders)

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    add_compile_definitions(NDEBUG)
    add_custom_command(TARGET vermicelli
            POST_BUILD
            COMMAND /bin/sh ${CMAKE_SOURCE_DIR}gen_documentation.sh
            )
endif ()
