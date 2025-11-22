# Simple Window Example

if(ANDROID)
	# On Android, build as shared library
	add_library(simple_window SHARED)
	add_apk(
		TARGET simple_window
		PACKAGE_NAME net.stereokit.simple_window
		APP_NAME "Simple Window"
	)
else()
	# On desktop, build as executable
	add_executable(simple_window)
endif()

target_link_libraries(simple_window PRIVATE sk_app)
target_sources       (simple_window PRIVATE
	simple_window.c
)