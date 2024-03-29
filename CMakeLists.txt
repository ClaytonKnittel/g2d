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

# add the cmake directory to the cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


############################################################
# g2d Library
############################################################

project(g2d VERSION 0.1)


############################################################
# Options
############################################################

set(BUILD_SHARED_LIBS ON CACHE BOOL "When enabled, builds libg2d as a
	shared library, otherwise building as a static library")
set(ENABLE_TESTING OFF CACHE BOOL "When enabled, build all unit tests. The
	unit tests can then be run from within the build directory with
	'make run_tests'.")
set(BUILD_EXAMPLES OFF CACHE BOOL "When enabled, build all examples. The
	examples can then be run from within the build directory with
	'make run_<example_name>'.")


############################################################
# Parameters
############################################################

set(METAL_PLATFORMS Darwin)
set(OPENGL_PLATFORMS Linux)
#set(METAL_PLATFORMS)
#set(OPENGL_PLATFORMS Darwin Linux)

# check for graphics library support
set(USE_METAL OFF)
set(USE_OPENGL OFF)

if(CMAKE_SYSTEM_NAME IN_LIST METAL_PLATFORMS)
	set(USE_METAL ON)
elseif(CMAKE_SYSTEM_NAME IN_LIST OPENGL_PLATFORMS)
	set(USE_OPENGL ON)
else()
	message(SEND_ERROR "System not supported: ${CMAKE_SYSTEM_NAME}")
endif()

set(AUDIO_TOOLBOX_PLATFORMS Darwin)
set(OPENAL_PLATFORMS Linux)

# check for graphics library support
set(USE_AUDIO_TOOLBOX OFF)
set(USE_OPENAL OFF)

if(CMAKE_SYSTEM_NAME IN_LIST AUDIO_TOOLBOX_PLATFORMS)
	set(USE_AUDIO_TOOLBOX ON)
elseif(CMAKE_SYSTEM_NAME IN_LIST OPENAL_PLATFORMS)
	set(USE_OPENAL ON)
else()
	message(SEND_ERROR "System not supported: ${CMAKE_SYSTEM_NAME}")
endif()


############################################################
# g2d Metal Shaders Library
############################################################

if(USE_METAL)

project(g2d_metal_shaders)

file(GLOB_RECURSE METAL_SHADERS_SRC
	"${PROJECT_SOURCE_DIR}/src/*.metal"
)

add_library(g2d_metal_shaders SHARED
  ${METAL_SHADERS_SRC}
)

set_target_properties(g2d_metal_shaders PROPERTIES SUFFIX ".metallib")
enable_language(METAL)

# When building in Debug mode, enable Metal validation layer.
if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	set(METAL_VALIDATION_ENV METAL_DEVICE_WRAPPER_TYPE=1
		METAL_ERROR_MODE=5 METAL_DEBUG_ERROR_MODE=5)
else()
	set(METAL_VALIDATION_ENV)
endif()

endif()


############################################################
# Build Rules
############################################################

file(GLOB_RECURSE C_SRC
	"${PROJECT_SOURCE_DIR}/src/*.c"
)
file(GLOB_RECURSE CXX_SRC
	"${PROJECT_SOURCE_DIR}/src/*.cc"
)
file(GLOB_RECURSE OBJC_SRC
	"${PROJECT_SOURCE_DIR}/src/*.m"
)
file(GLOB_RECURSE ASM_SRC
	"${PROJECT_SOURCE_DIR}/src/*.s"
)

if(USE_METAL)
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/metal_shader_lib_source.c"
		COMMAND sh ${PROJECT_SOURCE_DIR}/binutils/generate_file_contents_array_source.sh
			${CMAKE_CURRENT_BINARY_DIR}/metal_shader_lib_source.c
			$<TARGET_FILE:g2d_metal_shaders>
			g_metal_shader_lib_source
		DEPENDS
			${PROJECT_SOURCE_DIR}/binutils/generate_file_contents_array_source.sh
			$<TARGET_FILE:g2d_metal_shaders>
	)

	list(APPEND C_SRC "${CMAKE_CURRENT_BINARY_DIR}/metal_shader_lib_source.c")
endif()

# remove arch_test dir
list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")
list(FILTER CXX_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")
list(FILTER OBJC_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")
list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/arch_test/*")

# remove opengl/metal specific directories if not in use
if(NOT USE_METAL)
	list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/metal/*")
	list(FILTER CXX_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/metal/*")
	list(FILTER OBJC_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/metal/*")
	list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/metal/*")
endif()

if(NOT USE_OPENGL)
	list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/opengl/*")
	list(FILTER CXX_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/opengl/*")
	list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/opengl/*")
endif()

# remove openal/AudioToolbox specific directories if not in use
if(NOT USE_AUDIO_TOOLBOX)
	list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/audio_toolbox/*")
	list(FILTER CXX_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/audio_toolbox/*")
	list(FILTER OBJC_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/audio_toolbox/*")
	list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/audio_toolbox/*")
endif()

if(NOT USE_OPENAL)
	list(FILTER C_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/openal/*")
	list(FILTER CXX_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/openal/*")
	list(FILTER ASM_SRC EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/src/openal/*")
endif()


if(${BUILD_SHARED_LIBS})
	add_library(g2d SHARED
		${C_SRC} ${CXX_SRC} ${OBJC_SRC} ${ASM_SRC}
	)
else()
	add_library(g2d STATIC
		${C_SRC} ${CXX_SRC} ${OBJC_SRC} ${ASM_SRC}
	)
endif()

if("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" AND NOT "${OBJC_SRC}" STREQUAL "")
	enable_language(OBJC)
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
target_compile_options(g2d
	PRIVATE
		$<$<COMPILE_LANGUAGE:CXX>:-Wno-extra-semi -Wno-c99-extensions>
	PUBLIC
		$<$<COMPILE_LANGUAGE:CXX>:-Wno-nested-anon-types -Wno-ignored-qualifiers
		-Wno-gnu-anonymous-struct -Wno-unused-parameter -Wno-newline-eof
		-Wno-c++11-extensions -Wno-c++14-extensions -Wno-c++17-extensions
		-Wno-gnu-zero-variadic-macro-arguments>
)

if("${CMAKE_ASM_COMPILER_ID}" MATCHES "GNU")
	set(ASM_OPTIONS -Wa,--64 -Wa,-msyntax=intel -Wa,-mnaked-reg)
elseif("${CMAKE_ASM_COMPILER_ID}" MATCHES "Clang")
	set(ASM_OPTIONS -masm=intel)
else()
	message(SEND_ERROR "Unsupported assembler (${CMAKE_ASM_COMPILER_ID})")
endif()

target_compile_options(g2d PRIVATE $<$<COMPILE_LANGUAGE:ASM>:${ASM_OPTIONS}>)

if (${ENABLE_TESTING})
	target_compile_definitions(g2d PRIVATE DO_TESTING)
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug" OR "${CMAKE_BUILD_TYPE}" MATCHES "RelWithDebInfo")
	target_compile_definitions(g2d PRIVATE DEBUG_BUILD)
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release")
	target_compile_definitions(g2d PRIVATE RELEASE_BUILD)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

if (NOT COMPILER_SUPPORTS_CXX17)
	message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} does not support "
		"c++17, please use a different compiler")
endif()

set_property(TARGET g2d PROPERTY CXX_STANDARD 17)

if(USE_METAL)
	target_compile_definitions(g2d PUBLIC USE_METAL)
elseif(USE_OPENGL)
	target_compile_definitions(g2d PUBLIC USE_OPENGL)

	find_library(glfw glfw REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
endif()

if(USE_AUDIO_TOOLBOX)
	target_compile_definitions(g2d PUBLIC USE_AUDIO_TOOLBOX)
elseif(USE_OPENAL)
	target_compile_definitions(g2d PUBLIC USE_OPENAL)

	find_package(OpenAL MODULE REQUIRED)
endif()

find_package(Freetype MODULE REQUIRED)

target_include_directories(g2d
	PRIVATE
		${FREETYPE_INCLUDE_DIRS}
		"/usr/local/include"
	PUBLIC
		$<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(g2d
	PRIVATE
		${FREETYPE_LIBRARIES}
	PUBLIC
		"${LINK_LIBS}"
		utils
)

if(USE_METAL)
	target_link_libraries(g2d
		PUBLIC
			"-framework Foundation"
			"-framework QuartzCore"
			"-framework Metal"
			"-framework MetalKit"
			"-framework AppKit"
	)
elseif(USE_OPENGL)
	target_link_libraries(g2d
		# TODO change this to PRIVATE
		PUBLIC
			glfw GLEW ${OPENGL_LIBRARIES}
	)
endif()

if(USE_AUDIO_TOOLBOX)
	target_link_libraries(g2d
		PUBLIC
			"-framework Foundation"
			"-framework AudioToolbox"
	)
elseif(USE_OPENAL)
	target_include_directories(g2d
		PRIVATE
			${OPENAL_INCLUDE_DIR}
	)

	target_link_libraries(g2d
		PRIVATE
			${OPENAL_LIBRARY}
	)
endif()

if (${ENABLE_TESTING})
	find_library(check check REQUIRED)

	target_link_directories(g2d
		PUBLIC "/usr/local/lib"
	)

	target_link_libraries(g2d
		PUBLIC
			check "${LINK_LIBS}"
	)

	add_subdirectory(test/unit)
endif()

if (${BUILD_EXAMPLES})
  add_subdirectory(examples/test)
endif()


############################################################
# Installation Rules
############################################################

# Library
if(${BUILD_SHARED_LIBS})
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


############################################################
# Submodule Dependencies
############################################################

set(ENABLE_TESTING OFF)

add_subdirectory(modules/utils)

