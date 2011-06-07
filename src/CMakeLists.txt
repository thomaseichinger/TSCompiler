# include as include directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# SOURCES variable including all *cpp files
file(GLOB TSCompiler_SOURCES *.cpp)
file(GLOB TSCompiler_HEADERS *.h)
QT4_WRAP_CPP(TSCompiler_HEADERS_MOC apduviewer.h)

set(COCO_FILES TSCompiler.atg Parser.frame Scanner.frame)

add_executable( tscompiler ${TSCompiler_SOURCES} ${TSCompiler_HEADERS_MOC})
TARGET_LINK_LIBRARIES(tscompiler ${QT_LIBRARIES})

if (CMAKE_COMPILER_IS_GNUCC)
    message("compiler flags will work")
    set_source_files_properties(${TSCompiler_SOURCES} PROPERTIES COMPILE_FLAGS "-Werror -Wall")
endif ()

set_property(DIRECTORY APPEND PROPERTY
             ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_SOURCE_DIR}/install)

install(TARGETS tscompiler
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/lib
)
