
find_program(CMAKE_METAL_COMPILER
	NAMES "xcrun"
	DOC "metal shader compiler"
)

set(CMAKE_METAL_SOURCE_FILE_EXTENSIONS metal)
set(CMAKE_METAL_OUTPUT_EXTENSION .air)
set(CMAKE_METAL_COMPILER_ENV_VAR "")

configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeMETALCompiler.cmake.in
	${CMAKE_PLATFORM_INFO_DIR}/CMakeMETALCompiler.cmake )

