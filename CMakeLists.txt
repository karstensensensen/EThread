cmake_minimum_required(VERSION 3.0.0)
project(EThread VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

set(SRC 
    src/EThread.cpp
    src/LThread.cpp
    include/ippsrc/EThread.ipp
    include/ippsrc/LThread.ipp
)

set(INCLUDE
    include/EThread.h
    include/LThread.h
)

add_library(${PROJECT_NAME} STATIC ${SRC} ${INCLUDE} ETH.h)

target_include_directories(${PROJECT_NAME} PUBLIC ./)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE} ETH.h)

option(BUILD_TEST "builds test project" OFF)

if(${BUILD_TEST})

add_executable(test_${PROJECT_NAME} tests/test.cpp)

target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})
target_include_directories(test_${PROJECT_NAME} PUBLIC ./)

endif()
