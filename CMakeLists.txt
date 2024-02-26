cmake_minimum_required(VERSION 3.10)
project(Oimo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0 -DOIMO_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(LIB_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(TEST_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(COPY ${HEADERS} DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/include")

enable_language(ASM)
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S")
add_library(oimo STATIC ${SOURCES} ${ASM_SOURCES})
find_package(yaml-cpp REQUIRED)
target_link_libraries(oimo yaml-cpp)
set_target_properties(
    oimo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT}")

file(GLOB TEST_PROTO_SOURCES "tests/protobuf/*.cc")
file(GLOB TEST_SOURCES "tests/*.cc")
find_package(Protobuf REQUIRED)
foreach(testsourcefile ${TEST_SOURCES})
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable(${testname} ${testsourcefile} ${TEST_PROTO_SOURCES})
    target_link_libraries(${testname} oimo protobuf::libprotobuf)
    set_target_properties(
        ${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT}")
endforeach(testsourcefile ${TEST_SOURCES})