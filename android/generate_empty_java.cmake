# generate_empty_java.cmake - Generate Empty.java file
# Usage: cmake -DOUTPUT_FILE=<path> -P generate_empty_java.cmake

if(NOT OUTPUT_FILE)
	message(FATAL_ERROR "OUTPUT_FILE is required")
endif()

get_filename_component(OUTPUT_DIR "${OUTPUT_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${OUTPUT_DIR}")
file(WRITE "${OUTPUT_FILE}" "public class Empty {}")
