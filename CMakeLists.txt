cmake_minimum_required(VERSION 3.11)

project(ScreenCapture)

link_libraries(msimg32)

set(SRC_FILES CImage.cpp ImageFactory.cpp main.cpp Dpi.cpp win/CharsetConvert.cpp)

## window system does not show console window
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    endif()
endif()

add_executable(ScreenCapture ${SRC_FILES})
