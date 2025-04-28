FILE(GLOB SRC_FILES "magic/*.c")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
find_library(Z_LIBRARY NAMES z)
find_library(LZMA_LIBRARY NAMES lzma)
link_libraries(${Z_LIBRARY} ${LZMA_LIBRARY})
add_library(magic ${SRC_FILES})

set(CMAKE_FIND_LIBRARY_SUFFIXES .so .a)

add_custom_command(TARGET magic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/magic.mgc ${CMAKE_BINARY_DIR})