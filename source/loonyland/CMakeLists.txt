file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	loonyland
	SOURCES ${sources}
)
target_link_options(loonyland PRIVATE /ZI /INCREMENTAL)
set(CMAKE_CXX_FLAGS_DEBUG  "/ZI")
target_link_libraries(loonyland steam_api APCpp nlohmann_json)
