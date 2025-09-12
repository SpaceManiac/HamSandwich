set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
add_library(imgui STATIC
	"${IMGUI_DIR}/imgui.cpp"
	#"${IMGUI_DIR}/imgui_demo.cpp"
	"${IMGUI_DIR}/imgui_draw.cpp"
	"${IMGUI_DIR}/imgui_tables.cpp"
	"${IMGUI_DIR}/imgui_widgets.cpp"
	"${IMGUI_DIR}/backends/imgui_impl_sdl.cpp"
	"${IMGUI_DIR}/backends/imgui_impl_sdlrenderer.cpp"
)
target_link_libraries(imgui PUBLIC SDL2)
target_include_directories(imgui PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/imgui/LICENSE.txt" TYPE BIN COMPONENT launcher/executables RENAME "LICENSE.imgui.txt")
