cmake_minimum_required( VERSION 2.8)

project( disseqt)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=512")

add_executable( disseqt 
    disseqt_grammar.cpp  
    lexer.cpp  
    parser.cpp
    )


