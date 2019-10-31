cmake_minimum_required(VERSION 3.9)
project(ngraph.native.benchmarck)

set(CMAKE_CXX_STANDARD 14)


#GoogleBenchmark
find_package(benchmark)

add_executable(

    ../src/layout.cpp
    ../src/layout/quadTree.cpp
    ../demo/ngraph.native.demo/ngraph.native.demo/main.cpp

)

target_link_libraries(${PROJECT_NAME} benchmark)