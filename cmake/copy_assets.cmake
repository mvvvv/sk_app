# copy_assets.cmake - Copy directory contents, touch stamp only if changed
#
# Usage:
#   cmake -DSRC_DIR=<source> -DDEST_DIR=<dest> -DSTAMP_FILE=<stamp> -P copy_assets.cmake
#
# Copies files from SRC_DIR to DEST_DIR only if content differs.
# Touches STAMP_FILE only if any files were actually copied.

if(NOT SRC_DIR OR NOT DEST_DIR)
	message(FATAL_ERROR "SRC_DIR and DEST_DIR are required")
endif()

# Ensure destination exists
file(MAKE_DIRECTORY "${DEST_DIR}")

# Find all files in source directory
file(GLOB_RECURSE SRC_FILES RELATIVE "${SRC_DIR}" "${SRC_DIR}/*")

set(CHANGED FALSE)

foreach(REL_PATH ${SRC_FILES})
	set(SRC_FILE "${SRC_DIR}/${REL_PATH}")
	set(DEST_FILE "${DEST_DIR}/${REL_PATH}")

	# Check if we need to copy
	set(NEEDS_COPY FALSE)

	if(NOT EXISTS "${DEST_FILE}")
		set(NEEDS_COPY TRUE)
	else()
		# Compare file hashes
		file(MD5 "${SRC_FILE}" SRC_HASH)
		file(MD5 "${DEST_FILE}" DEST_HASH)
		if(NOT "${SRC_HASH}" STREQUAL "${DEST_HASH}")
			set(NEEDS_COPY TRUE)
		endif()
	endif()

	if(NEEDS_COPY)
		# Ensure parent directory exists
		get_filename_component(DEST_PARENT "${DEST_FILE}" DIRECTORY)
		file(MAKE_DIRECTORY "${DEST_PARENT}")

		# Copy the file
		file(COPY "${SRC_FILE}" DESTINATION "${DEST_PARENT}")
		message(STATUS "Copied: ${REL_PATH}")
		set(CHANGED TRUE)
	endif()
endforeach()

# Touch stamp only if something changed
if(CHANGED AND STAMP_FILE)
	file(TOUCH "${STAMP_FILE}")
	message(STATUS "Assets changed, updated stamp: ${STAMP_FILE}")
endif()
