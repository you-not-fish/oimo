cmake_minimum_required(VERSION 3.10)
project(Oimo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 将src中的源代码编译，在lib文件夹中生成库
file(GLOB SOURCES "src/*.cc")
add_library(oimo STATIC ${SOURCES})

# 将头文件放到include文件夹中
target_include_directories(oimo PUBLIC include)

# 将tests中的代码编译，在bin/tests下生成可执行文件
file(GLOB TEST_SOURCES "tests/*.cc")
foreach(testsourcefile ${TEST_SOURCES})
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable(${testname} ${testsourcefile})
    target_link_libraries(${testname} oimo)
    set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin/tests")
endforeach(testsourcefile ${TEST_SOURCES})