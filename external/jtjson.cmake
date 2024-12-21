add_subdirectory("jtjson" EXCLUDE_FROM_ALL)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/jtjson/double-conversion/LICENSE" TYPE BIN COMPONENT launcher/executables RENAME "LICENSE.double-conversion.txt")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/jtjson/LICENSE" TYPE BIN COMPONENT launcher/executables RENAME "LICENSE.jtjson.txt")
