function(install_no_symlink component fname namepart)
	if(NOT namepart)
		get_filename_component(namepart "${fname}" NAME)
	endif()
	install(CODE "
		get_filename_component(realpath \"${fname}\" REALPATH)
		file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\" TYPE FILE MESSAGE_LAZY RENAME \"${namepart}\" FILES \"\${realpath}\")
	" COMPONENT "${component}")
endfunction()
function(install_as_soname component)
	foreach(fname IN LISTS ARGN)
		install(CODE "
			execute_process(COMMAND patchelf --print-soname \"${fname}\" OUTPUT_VARIABLE soname OUTPUT_STRIP_TRAILING_WHITESPACE)
			get_filename_component(realpath \"${fname}\" REALPATH)
			file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\" TYPE FILE MESSAGE_LAZY RENAME \"\${soname}\" FILES \"\${realpath}\")
		" COMPONENT "${component}")
	endforeach()
endfunction()

include("zlib.cmake")
include("SDL2.cmake")
include("steamworks_sdk.cmake")
add_subdirectory("lzma1900" EXCLUDE_FROM_ALL)
add_subdirectory("nsis_exehead" EXCLUDE_FROM_ALL)
add_subdirectory("APCpp")

include("jtjson.cmake")

if(NOT EMSCRIPTEN AND NOT ANDROID)
	# Launcher and dev tools dependencies.
	add_subdirectory("tinyfiledialogs")
	include("imgui.cmake")
	include("curl.cmake")
endif()
