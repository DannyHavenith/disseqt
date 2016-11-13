find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable( combined_tests 
    parsing.cpp queries.cpp)

target_link_libraries(combined_tests ${GTEST_BOTH_LIBRARIES} disseqt)
target_link_libraries(combined_tests ${CMAKE_THREAD_LIBS_INIT})


add_test( 
    NAME combined_tests 
    COMMAND combined_tests)
