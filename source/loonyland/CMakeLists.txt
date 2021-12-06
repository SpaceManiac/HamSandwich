file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}")

file(GENERATE OUTPUT "metadata.cpp" INPUT "${CMAKE_CURRENT_SOURCE_DIR}/../metadata.cpp.in")
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/metadata.cpp")

add_executable(loonyland ${SOURCES})
target_link_libraries(loonyland ham)
