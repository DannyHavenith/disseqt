add_library( disseqt 
    disseqt_grammar.cpp  
    lexer.cpp  
    parser.cpp
    )

target_include_directories( disseqt PRIVATE ${CONAN_INCLUDE_DIRS})

