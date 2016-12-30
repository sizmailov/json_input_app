cmake_minimum_required(VERSION 3.1.3)
project(json_input_app)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# boost iostreams and system are required
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS iostreams system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 

# external dependencies
# 1) concise_json_schema
add_subdirectory(external/concise_json_schema)
include_directories(${concise_json_schema_INCLUDE_DIRS})

# 2) delegate
include_directories(external/delegate)

# self includes
include_directories(include)

add_subdirectory(test)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp )

add_library(${PROJECT_NAME} ${SOURCE_FILES})

add_executable(${PROJECT_NAME}_example example.cpp)
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})

set(${PROJECT_NAME}_INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/include
        ${concise_json_schema_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)


target_link_libraries(${PROJECT_NAME} concise_json_schema)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
