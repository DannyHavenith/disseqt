find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable( visitors visitors.cpp parsing.cpp)

target_link_libraries(visitors ${GTEST_BOTH_LIBRARIES} disseqt)
target_link_libraries(visitors ${CMAKE_THREAD_LIBS_INIT})


enable_testing()
add_test( visitors visitors)