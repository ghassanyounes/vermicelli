cmake_minimum_required(VERSION 3.23)
project(vermicelli)

set(CMAKE_CXX_STANDARD 20)

include(FindPkgConfig)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)
find_package(glm REQUIRED)
#find_package(X11 REQUIRED)
#add_link_options(-lXxf86vm -lXrandr -lXi)

# Automatically download stuff with FetchContent
include(FetchContent)

set(SDL2PP_WITH_IMAGE ON) # for SDL_image support
set(SDL2PP_WITH_MIXER ON) # for SDL_mixer support
set(SDL2PP_WITH_TTF ON)   # for SDL_ttf support
FetchContent_Declare(SDL2pp
        GIT_REPOSITORY https://github.com/libSDL2pp/libSDL2pp.git
        GIT_TAG origin/master
        )
FetchContent_MakeAvailable(SDL2pp)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG origin/master
        )
FetchContent_MakeAvailable(fmt)

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

add_compile_options(-g -O2)

file(GLOB SOURCES src/*.cpp)

add_executable(vermicelli ${SOURCES})

if (THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(vermicelli PUBLIC "-pthread")
endif ()
if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(vermicelli "${CMAKE_THREAD_LIBS_INIT}")
endif ()

include_directories(libs/)

target_link_libraries(
        vermicelli
        SDL2pp::SDL2pp glm fmt::fmt
        ${CMAKE_DL_LIBS} ${glm_LIBRARIES} ${SDL2pp_LIBRARIES} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES}
        ${Threads_LIBRARIES}
)

add_dependencies(vermicelli shaders)

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    add_compile_definitions(NDEBUG)
    add_custom_command(TARGET vermicelli
            POST_BUILD
            COMMAND /bin/sh ${CMAKE_SOURCE_DIR}/gen_documentation.sh
            )
endif ()
