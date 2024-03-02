cmake_minimum_required(VERSION 3.20)

project(radiation-transmission)

set(CMAKE_CXX_STANDARD 20)

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package(Geant4 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${ROOT_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
