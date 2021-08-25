cmake_minimum_required(VERSION 3.0.0)
project(EThread VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

include(CTest)

set(SRC 
    src/EThread.cpp
)

set(INCLUDE
    include/EThread.h
)

add_library(EThread ${SRC} ${INCLUDE})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
