find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable( combined_tests 
    visitors.cpp parsing.cpp queries.cpp)

target_link_libraries(combined_tests ${GTEST_BOTH_LIBRARIES} disseqt)
target_link_libraries(combined_tests ${CMAKE_THREAD_LIBS_INIT})


enable_testing()
add_test( combined_tests combined_tests)