# include as include directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# SOURCES variable including all *cpp files
file(GLOB TSCompiler_SOURCES *.cpp)

add_executable( tscompile ${TSCompiler_SOURCES})
