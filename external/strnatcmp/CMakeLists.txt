# Main JSON library
add_library(strnatcmp STATIC
    src/strnatcmp.c
)
target_include_directories(strnatcmp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
