set(
	dcool_core_sources
	""
)

set(
	dcool_core_exportation_sources
	"./exportation.cppm"
)

if(dcool_might_use_modules AND dcool_core_exportation_sources)
	list(INSERT dcool_core_sources 0 ${dcool_core_exportation_sources})
endif()

if(dcool_core_sources)
	add_library(
		dcool_core
		SHARED
		${dcool_core_sources}
	)

	target_link_libraries(dcool_core PUBLIC dcool_base)
else()
	add_library(dcool_core INTERFACE)

	target_link_libraries(dcool_core INTERFACE dcool_base)
endif()
