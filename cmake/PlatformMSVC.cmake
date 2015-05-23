# MSVC specific compiler settings
# -------------------------------

# Precompiled header support for MSVC:
# Call this after setting the source list (and don't add the source file used
# to generate the pch file, this will be done here automatically)
function(enable_precompiled_headers PRECOMPILED_HEADER SOURCE_FILE SOURCE_VARIABLE_NAME)
	set(files ${${SOURCE_VARIABLE_NAME}})

	# Generate precompiled header translation unit
	get_filename_component(pch_basename ${PRECOMPILED_HEADER} NAME_WE)
	set(pch_abs ${CMAKE_CURRENT_SOURCE_DIR}/${PRECOMPILED_HEADER})
	set(pch_unity ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})
	set_source_files_properties(${pch_unity}  PROPERTIES COMPILE_FLAGS
		"/Yc\"${pch_abs}\"")

	# Update properties of source files to use the precompiled header.
	# Additionally, force the inclusion of the precompiled header at
	# beginning of each source file.
	foreach(source_file ${files} )
		set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS
			"/Yu\"${pch_abs}\" /FI\"${pch_abs}\"")
	endforeach(source_file)

	# Finally, update the source file collection to contain the
	# precompiled header translation unit
	set(${SOURCE_VARIABLE_NAME} ${pch_unity} ${${SOURCE_VARIABLE_NAME}} PARENT_SCOPE)
endfunction(enable_precompiled_headers)

# enable_precompiled_headers(Src/stdafx.h Src/stdafx.cpp SRCS)


add_definitions(-D_SECURE_SCL=0)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
