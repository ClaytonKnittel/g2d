cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message("Setting build type to 'RelWithDebInfo' as none was specified")
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
		"RelWithDebInfo" "MinSizeRel")
endif()

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


############################################################
# Utils Library
############################################################

project(g2d VERSION 0.1)


############################################################
# Options
############################################################

set(BUILD_SHARED_LIBRARY ON CACHE BOOL "When enabled, builds libg2d as a
	shared library, otherwise building as a static library")
set(ENABLE_TESTING OFF CACHE BOOL "When enabled, build all unit tests. The
	unit tests can then be run from within the build directory with
	'make run_tests'.")
set(ENABLE_AVX "CHECK" CACHE STRING "When enabled, builds certain methods using the
	AVX instruction set. Can disable this manually, but AVX support is
	automatically checked.")


############################################################
# Build Rules
############################################################

file(GLOB_RECURSE C_SRC
	"${PROJECT_SOURCE_DIR}/src/*.c"
)
file(GLOB_RECURSE ASM_SRC
	"${PROJECT_SOURCE_DIR}/src/*.s"
)

# remove arch_test dir
list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")
list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")

if(${BUILD_SHARED_LIBRARY})
	add_library(g2d SHARED
		${C_SRC} ${ASM_SRC}
	)
else()
	add_library(g2d STATIC
		${C_SRC} ${ASM_SRC}
	)
endif()

if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64")
	enable_language(ASM)
	if(CMAKE_ASM_COMPILER_WORKS)
		set(can_use_assembler TRUE)
	endif()
endif()

if(NOT can_use_assembler)
	message(SEND_ERROR "No assembler found")
endif()

target_compile_options(g2d PRIVATE
	$<$<NOT:$<COMPILE_LANGUAGE:ASM>>:-Wpedantic -Wall -Wextra
	-Wno-unused-function -Wno-format -march=native -mtune=native>
)
target_compile_options(g2d PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu11>)

if("${CMAKE_ASM_COMPILER_ID}" MATCHES "GNU")
	set(ASM_OPTIONS -Wa,--64 -Wa,-msyntax=intel -Wa,-mnaked-reg)
elseif("${CMAKE_ASM_COMPILER_ID}" MATCHES "Clang")
	set(ASM_OPTIONS -masm=intel)
else()
	message(SEND_ERROR "Unsupported assembler (${CMAKE_ASM_COMPILER_ID})")
endif()

target_compile_options(g2d PRIVATE $<$<COMPILE_LANGUAGE:ASM>:${ASM_OPTIONS}>)

if("${ENABLE_AVX}" MATCHES "CHECK")
	set(OLD_CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}")
	set(CMAKE_ASM_FLAGS "${ASM_OPTIONS}")

	# check to see if this architecture supports AVX
	try_compile(AVX_SUPPORTED ${CMAKE_CURRENT_BINARY_DIR} SOURCES
		${PROJECT_SOURCE_DIR}/src/arch_test/test_avx.s
		${PROJECT_SOURCE_DIR}/src/arch_test/test_empty.c)

	if (AVX_SUPPORTED)
		message(VERBOSE "AVX is supported on this architecture/compiler config")
	else()
		message(VERBOSE "AVX is not supported on this architecture/compiler config")
	endif()
	set(CMAKE_ASM_FLAGS "${OLD_CMAKE_ASM_FLAGS}")
	unset(OLD_CMAKE_ASM_FLAGS)

	if(${AVX_SUPPORTED})
		set(ENABLE_AVX "ON" CACHE STRING "When enabled, builds certain methods using the
			AVX instruction set. Can disable this manually, but AVX support is
			automatically checked." FORCE)
	else()
		set(ENABLE_AVX "OFF" CACHE STRING "When enabled, builds certain methods using the
			AVX instruction set. Can disable this manually, but AVX support is
			automatically checked." FORCE)
	endif()
elseif("${ENABLE_AVX}" MATCHES "ON")
	set(AVX_SUPPORTED ON)
elseif("${ENABLE_AVX}" MATCHES "OFF")
	set(AVX_SUPPORTED OFF)
else()
	message(SEND_ERROR "ENABLE_AVX must be one of \"ON\", \"OFF\", or \"CHECK\"")
endif()

if (${ENABLE_TESTING})
	target_compile_definitions(g2d PRIVATE DO_TESTING)
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug" OR "${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo")
	target_compile_definitions(g2d PRIVATE DEBUG)
endif()

add_subdirectory(modules/utils)

find_library(glfw glfw REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype MODULE REQUIRED)

target_include_directories(g2d
	PRIVATE
		$<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${PROJECT_SOURCE_DIR}/include>
		${PROJECT_SOURCE_DIR}/modules/utils/include
		${FREETYPE_INCLUDE_DIRS}
		"/usr/local/include"
)

target_link_libraries(g2d
	PRIVATE
		${FREETYPE_LIBRARIES} glfw GLEW ${OPENGL_LIBRARIES}
	PUBLIC
		"${LINK_LIBS}"
)

if (${ENABLE_TESTING})
	find_library(check check REQUIRED)

	target_link_directories(g2d
		PUBLIC "/usr/local/lib"
	)

	target_link_libraries(g2d
		PUBLIC
			check "${LINK_LIBS}"
	)

    enable_testing()

	add_subdirectory(test)
endif()


############################################################
# Installation Rules
############################################################

# Library
if(${BUILD_SHARED_LIBRARY})
	install(TARGETS g2d LIBRARY DESTINATION lib)
else()
	install(TARGETS g2d ARCHIVE DESTINATION lib)
endif()

# Header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include)

# make uninstall
add_custom_target("uninstall" COMMENT "Uninstall installed files")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in
            install_manifest.txt to be uninstalled!
)

