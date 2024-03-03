cmake_minimum_required(VERSION 3.20)

project(radiation-transmission)

set(CMAKE_CXX_STANDARD 20)

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package(Geant4 REQUIRED)

include(FetchContent)

FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(CLI11)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${ROOT_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS})

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} CLI11::CLI11)
