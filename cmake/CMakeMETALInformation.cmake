# This file sets the basic flags for the METAL compiler

set(CMAKE_METAL_COMPILE_OBJECT
	"<CMAKE_METAL_COMPILER> -sdk macosx metal -c <SOURCE> -o <OBJECT>"
)

set(CMAKE_METAL_CREATE_SHARED_LIBRARY
	"<CMAKE_METAL_COMPILER> -sdk macosx metallib <OBJECTS> -o <TARGET>"
)

