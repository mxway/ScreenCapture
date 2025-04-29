cmake_minimum_required(VERSION 3.11)

project(ScreenCapture)

set(CMAKE_CXX_STANDARD 11)

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

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	add_executable(ScreenCapture WIN32  ${SRC_FILES} ${SRC_OS_FILES})
else()
	add_executable(ScreenCapture ${SRC_FILES} ${SRC_OS_FILES})
endif()

