set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
add_library(imgui STATIC
	"${IMGUI_DIR}/imgui.cpp"
	#"${IMGUI_DIR}/imgui_demo.cpp"
	"${IMGUI_DIR}/imgui_draw.cpp"
	"${IMGUI_DIR}/imgui_tables.cpp"
	"${IMGUI_DIR}/imgui_widgets.cpp"
	"${IMGUI_DIR}/backends/imgui_impl_sdl.cpp"
	"${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp"
)
target_link_libraries(imgui PUBLIC SDL2)
if(WIN32)
	target_link_libraries(imgui PUBLIC opengl32)
elseif(APPLE)
	target_link_options(imgui PUBLIC -framework OpenGL)
else()
	target_link_libraries(imgui PUBLIC GL)
endif()
target_include_directories(imgui PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/imgui/LICENSE.txt" TYPE BIN COMPONENT generic/executables RENAME "LICENSE.imgui.txt")
