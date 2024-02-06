cmake_minimum_required(VERSION 3.10)
project(Oimo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(LIB_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(TEST_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin/tests")

file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(COPY ${HEADERS} DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/include")

enable_language(ASM)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S")
add_library(oimo STATIC ${SOURCES} ${ASM_SOURCES})
target_link_libraries(oimo yaml-cpp)
set_target_properties(
    oimo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT}")


file(GLOB TEST_SOURCES "tests/*.cc")
foreach(testsourcefile ${TEST_SOURCES})
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable(${testname} ${testsourcefile})
    target_link_libraries(${testname} oimo)
    set_target_properties(
        ${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TEST_OUTPUT}")
endforeach(testsourcefile ${TEST_SOURCES})