cmake_minimum_required(VERSION 3.24)
project(LBA1_Win32_M1 LANGUAGES C)

# Independent modern build; original DOS root CMakeLists.txt stays untouched.
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT WIN32)
    message(FATAL_ERROR "M1 is a Windows target. Use MSVC or MinGW.")
endif()

find_package(SDL3 CONFIG QUIET)
if(NOT TARGET SDL3::SDL3)
    include(FetchContent)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.18
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(SDL3)
endif()
find_package(OpenGL REQUIRED)
add_executable(lba1-win32-m1 WIN32 main.c video_gl.c video_gl.h)
target_link_libraries(lba1-win32-m1 PRIVATE SDL3::SDL3 OpenGL::GL)
set_target_properties(lba1-win32-m1 PROPERTIES OUTPUT_NAME "LBA1-Win32-M1")
if(MSVC)
    target_compile_options(lba1-win32-m1 PRIVATE /W4)
else()
    target_compile_options(lba1-win32-m1 PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(TARGET SDL3::SDL3-shared)
    add_custom_command(TARGET lba1-win32-m1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3-shared>"
            "$<TARGET_FILE_DIR:lba1-win32-m1>"
        VERBATIM)
endif()
