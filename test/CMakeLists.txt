project(g2d_unit_testing)

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

find_library(check check REQUIRED)

file(GLOB_RECURSE C_TEST_SRC
	"${PROJECT_SOURCE_DIR}/test_*.c"
)
file(GLOB_RECURSE CXX_TEST_SRC
	"${PROJECT_SOURCE_DIR}/test_*.cc"
)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
add_executable(g2d_unit_testing MACOSX_BUNDLE
	${C_TEST_SRC} ${CXX_TEST_SRC} "${PROJECT_SOURCE_DIR}/../src/metal/res/Main.storyboard"
)

set_target_properties(g2d_unit_testing PROPERTIES
    BUNDLE True
	RESOURCE "${PROJECT_SOURCE_DIR}/../src/metal/res/Main.storyboard"
	#MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.${CUR_TARGET}
    MACOSX_BUNDLE_BUNDLE_NAME g2d_unit_testing
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/metal/res/Info.plist
)
else()
add_executable(g2d_unit_testing
	${C_TEST_SRC} ${CXX_TEST_SRC}
)
endif()

add_custom_target(run_g2d_tests
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DEPENDS g2d_unit_testing
)

target_compile_options(g2d_unit_testing PRIVATE
	-Wpedantic -Wall -Wextra -Wno-unused-function -Wno-format
	-march=native -mtune=native
)
target_compile_options(g2d_unit_testing PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu11>)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

if (COMPILER_SUPPORTS_CXX17)
	target_compile_options(g2d_unit_testing PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
else()
	message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} does not support "
		"c++17, please use a different compiler")
endif()

target_compile_definitions(g2d_unit_testing PRIVATE DO_TESTING)

target_include_directories(g2d_unit_testing
	PUBLIC
		"/usr/local/include"
)

target_link_directories(g2d_unit_testing
	PUBLIC "/usr/local/lib"
)

target_link_libraries(g2d_unit_testing
	PRIVATE
		g2d utils
	PUBLIC
		"${LINK_LIBS}" check
)

get_filename_component(UTILS_DIR ../ ABSOLUTE)

target_include_directories(g2d_unit_testing
	PRIVATE
		${UTILS_DIR}/include
)

