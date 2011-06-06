# Project name
project(TSCompiler)

cmake_minimum_required(VERSION 2.8)

#find_package(stdlibs REQUIRED)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Automoc4 is also mandatory
#find_package(Automoc4 REQUIRED)

#find_package(Doxygen)
#if (DOXYGEN_FOUND STREQUAL "NO")
#        message(WARNING "Doxygen not found!!")
#endif (DOXYGEN_FOUND STREQUAL "NO")

# Here should come the section where I import the CoCo macro
# <= TODO

add_subdirectory(src)
