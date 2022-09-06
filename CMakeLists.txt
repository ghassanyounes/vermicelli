cmake_minimum_required(VERSION 3.23)
project(vermicelli)

include(FindPkgConfig)
find_package(SDL2pp REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-g -O2)
add_link_options(-lXxf86vm -lXrandr -lXi)

add_executable(vermicelli main.cpp application.h application.cpp)
if (THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(vermicelli PUBLIC "-pthread")
endif ()
if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(vermicelli "${CMAKE_THREAD_LIBS_INIT}")
endif ()
target_link_libraries(vermicelli glfw SDL2pp::SDL2pp ${CMAKE_DL_LIBS} ${SDL2pp_LIBRARIES} ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES} ${Threads_LIBRARIES})
