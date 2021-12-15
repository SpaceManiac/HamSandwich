file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}")

HamSandwich_metadata(loonyland SOURCES [[
{
	"installers": [
		{
			"filename": "loonyland_install.EXE",
			"kind": "inno",
			"sha256sum": "cf3cdc555297e41f6c2da61d89815dbbc740d6fc677c83ec6c6e1acfa117de34",
			"link": "https://hamumu.itch.io/loonyland-halloween-hill",
			"file_id": 702760
		},
		{
			"filename": "loonyland_editor.exe",
			"kind": "nsis",
			"sha256sum": "865550d077e984ca28324aaf4291211aa4009cdad9f2b74144179c6342f2be39",
			"link": "https://hamumu.itch.io/loonyland-halloween-hill",
			"file_id": 782965
		}
	]
}
]])

add_executable(loonyland ${SOURCES})
target_link_libraries(loonyland ham)
