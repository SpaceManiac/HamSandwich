file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}")

HamSandwich_metadata(loonyland2 SOURCES [[
{
	"installers": [
		{
			"filename": "LL2CEinstall.exe",
			"kind": "nsis",
			"sha256sum": "0806e1615eb94332bf805128d2d3857af420e93ee6f48692eebf17c05e9b14e2",
			"link": "https://hamumu.itch.io/loonyland-2-winter-woods",
			"file_id": 705653
		}
	]
}
]])

add_executable(loonyland2 ${SOURCES})
target_link_libraries(loonyland2 ham)
target_compile_definitions(loonyland2 PRIVATE DIRECTORS)
