# Project name
project(TSCompiler)

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_MODULE_PATH ${TSCompiler_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
#include(FindCoco)


find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_subdirectory(src)

# Automoc4 is also mandatory
#find_package(Automoc4 REQUIRED)

#find_package(Doxygen)
#if (DOXYGEN_FOUND STREQUAL "NO")
#        message(WARNING "Doxygen not found!!")
#endif (DOXYGEN_FOUND STREQUAL "NO")

# Here should come the section where I import the CoCo macro
# <= TODO
#find_package(Coco)
#if (COCO_FOUND STREQUAL "NO")
#    message(WARNING "Coco not found!")
#endif (COCO_FOUND STREQUAL "NO")

if(WIN32)
    set(COCO_EXECUTEABLE ../Coco/Coco.exe)
else(WIN32)
    set(COCO_EXECUTEABLE ../Coco/Coco)
endif(WIN32)

add_custom_target(coco ${COCO_EXECUTEABLE} ${CMAKE_SOURCE_DIR}/src/TSCompiler.atg DEPENDS ${COCO_FILES})

add_dependencies(tscompiler coco)

