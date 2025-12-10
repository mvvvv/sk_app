# Simple Window Example

add_skapp(simple_window
	PACKAGE_NAME net.stereokit.simple_window
	APP_NAME "Simple Window"
)

target_sources(simple_window PRIVATE
	simple_window.c
)