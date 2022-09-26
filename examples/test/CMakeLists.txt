project(g2d_test_example)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	find_program(LSB_RELEASE_EXEC lsb_release REQUIRED)
	execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
		OUTPUT_VARIABLE LINUX_DISTR
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if("${LINUX_DISTR}" MATCHES "Ubuntu")
		list(APPEND LINK_LIBS "rt;pthread;subunit")
	endif()
else()
	set(LINUX_DISTR "")
endif()

file(GLOB_RECURSE CXX_SRC
	"${PROJECT_SOURCE_DIR}/*.cc"
)

add_executable(g2d_test_example MACOSX_BUNDLE
  ${CXX_SRC}
)

set_property(TARGET g2d_test_example PROPERTY MACOSX_BUNDLE_BUNDLE_NAME "g2d")
set_property(TARGET g2d_test_example PROPERTY MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
set_property(TARGET g2d_test_example PROPERTY MACOSX_BUNDLE_ICON_FILE "Resources/stardew.icns")

add_custom_target(run_g2d_test_example
	COMMAND ${METAL_VALIDATION_ENV} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
	DEPENDS g2d_test_example
)

target_compile_options(g2d_test_example PRIVATE
	-Wpedantic -Wall -Wextra -Wno-unused-function -Wno-format
	-march=native -mtune=native
)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

if (NOT COMPILER_SUPPORTS_CXX17)
	message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} does not support "
		"c++17, please use a different compiler")
endif()

set_property(TARGET g2d_test_example PROPERTY CXX_STANDARD 17)

target_include_directories(g2d_test_example
	PUBLIC
		"/usr/local/include"
)

target_link_directories(g2d_test_example
	PUBLIC "/usr/local/lib"
)

target_link_libraries(g2d_test_example
	PRIVATE
		g2d utils "${LINK_LIBS}"
)

get_filename_component(UTILS_DIR ../../ ABSOLUTE)

target_include_directories(g2d_test_example
	PRIVATE
		${UTILS_DIR}/include
)



