cmake_minimum_required( VERSION 2.8)
enable_testing()

project( disseqt)
include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=512")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-std=c++11)

    
include_directories( ${disseqt_SOURCE_DIR}/disseqt)

add_subdirectory( disseqt)
add_subdirectory(test)

add_executable( analyze_queries
    analyze_queries.cpp
    )

target_link_libraries(     
    analyze_queries
    disseqt)


