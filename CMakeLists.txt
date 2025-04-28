cmake_minimum_required(VERSION 3.11)

project(ScreenCapture)

if(WIN32)
    link_libraries(msimg32)
else()
    link_libraries(X11 Xrender Xft)
endif()

FILE(GLOB SRC_FILES "src/*.cpp")

if(WIN32)
    FILE(GLOB SRC_OS_FILES "src/win/*.cpp")
else()
    include_directories(src/linux/third-party/file/magic)
    include_directories(src/linux/third-party/stb)
    include_directories(src/linux/third-party/lunasvg/include)
    FILE(GLOB SRC_OS_FILES "src/linux/*.cpp")
    FILE(GLOB DLG_SRC_FILES "src/linux/UIDlg/*.cpp")

    list(APPEND SRC_OS_FILES ${DLG_SRC_FILES})
    # xft extend use freetype for rendering text.
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    add_subdirectory(src/linux/third-party/lunasvg)
    add_subdirectory(src/linux/third-party/file)
    link_libraries(magic lunasvg)
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
