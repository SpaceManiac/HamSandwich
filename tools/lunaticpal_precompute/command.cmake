function(lunaticpal_precompute_add_command compressed)
	add_custom_command(
		OUTPUT "${compressed}"
		COMMAND lunaticpal_precompute "${compressed}"
		COMMENT "Generating Lunatic palette lookup table"
		VERBATIM
	)
endfunction()
