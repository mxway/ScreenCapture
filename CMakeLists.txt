cmake_minimum_required(VERSION 3.11)

project(ScreenCapture)

if(WIN32)
    link_libraries(msimg32)
else()
    link_libraries(X11 Xrender Xext)
endif()

FILE(GLOB SRC_FILES "*.cpp")

if(WIN32)
    FILE(GLOB SRC_OS_FILES "win/*.cpp")
else()
    FILE(GLOB SRC_OS_FILES "linux/*.cpp")
endif()

## window system does not show console
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    endif()
endif()

add_executable(ScreenCapture ${SRC_FILES} ${SRC_OS_FILES})
