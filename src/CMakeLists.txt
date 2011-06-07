# include as include directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# SOURCES variable including all *cpp files
file(GLOB TSCompiler_SOURCES *.cpp)
file(GLOB TSCompiler_HEADERS *.h)
QT4_WRAP_CPP(TSCompiler_HEADERS_MOC apduviewer.h)

set(COCO_FILES TSCompiler.atg Parser.frame Scanner.frame)

add_executable( tscompiler ${TSCompiler_SOURCES} ${TSCompiler_HEADERS_MOC})
TARGET_LINK_LIBRARIES(tscompiler ${QT_LIBRARIES})
