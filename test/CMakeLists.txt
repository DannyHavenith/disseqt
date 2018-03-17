find_package(Threads REQUIRED)
add_executable( combined_tests 
    parsing.cpp 
    queries.cpp
    main.cpp)

target_link_libraries(combined_tests ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(combined_tests disseqt ${CONAN_LIBS})

target_include_directories( combined_tests PRIVATE ${CONAN_INCLUDE_DIRS})

enable_testing()
add_test( 
    NAME combined_tests 
    COMMAND combined_tests)
